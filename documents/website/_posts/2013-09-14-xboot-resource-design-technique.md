---
layout: post
title: xboot中resource设计技术详解
description: 在设计模式里，我们经常听到各种OO思想，面向对象比较符合人的思维，也因此而大行其道。本质上任何一个对象，都是由属性和方法组成，换另一种说法，就是由数据和驱动构成。再具体到设备驱动(模块组件)而言就是由device和driver构成。其中device是属性，描述这个设备，driver是方法，按一定的方法操作设备。为了实现模块的组态化，我们需要让设备与驱动的分离，比如一个驱动，通过传递不同的设备描述自动生成对应的设备驱动，在xboot中，resource是描述设备属性，driver是驱动，device是最终生成的设备驱动(模块组件)，通过名字进行匹配。因此，系统中已提供大量的通用驱动，只需提供具体的设备描述，就可自动生成一个设备驱动(模块组件)，这样的设计已将组态化推向极限了。
categories: articles
key: xboot, kobj
---

###引言
> 在各个板子支持包(BSP)目录下，有个resource目录，里面含有大量以"res-"开头".c"作后缀的文件，都是些各种模块的属性描述，其中也有个别模块，只有一个resource_t对象，没有绑定任何外部数据，这样的resource基本意味着其所对应的driver就是一自成一体的模块，不需任何数据描述。此时还需要提供resource的唯一的用途，就是是否自动生成该模块。

###resource_t结构体定义
resource_t结构体拥有名字，序列号，外部数据指针以及kboj对象，详细定义如下:
{% highlight c %}
struct resource_t
{
	/* The resource name */
	char * name;

	/* The resource id */
	int id;

	/* The resource data */
	void * data;

	/* Kobj binding */
	struct kobj_t * kobj;
};
{% endhighlight %}

###resource_list_t定义如下：
resource_list_t主要用于管理系统中注册的resource_t，为一全局链表，声明如下:
{% highlight c %}
struct resource_list_t {
	struct resource_t * res;
	struct list_head entry;
};
{% endhighlight %}

###自动分配资源id
其为一内部函数，根据所提供的资源名称，自动分配一个该名称下未使用的id序列，分配策略是由小到大，按是否空闲分配。
{% highlight c %}
static int resource_alloc_free_id(const char * name)
{
	struct resource_list_t * pos, * n;
	int id = 0;

	if(!name)
		return 0;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(strcmp(pos->res->name, name) == 0)
			id++;
	}

	return id;
}
{% endhighlight %}

###搜索资源
同样为一个内部函数，根据所提供的资源名称及资源id，搜索资源对象，如存在返回该对象，如不存在返回空。
{% highlight c %}
static struct resource_t * search_resource_with_id(const char * name, int id)
{
	struct resource_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(pos->res->id == id)
		{
			if(strcmp(pos->res->name, name) == 0)
				return pos->res;
		}
	}

	return NULL;
}
{% endhighlight %}

###注册资源
将一个静态资源注册到系统中，如该名称下的id序列已占用，则自动分配一个新id，并自动生成一组kobj对象。成功返回真，否则返回假。实现如下：
{% highlight c %}
bool_t register_resource(struct resource_t * res)
{
	struct resource_list_t * rl;
	char name[64];

	if(!res || !res->name)
		return FALSE;

	if(res->id < 0)
	{
		res->id = resource_alloc_free_id(res->name);
	}
	else
	{
		if(search_resource_with_id(res->name, res->id))
			return FALSE;
	}

	rl = malloc(sizeof(struct resource_list_t));
	if(!rl)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);
	res->kobj = kobj_alloc_directory(name);
	kobj_add_regular(res->kobj, "name", resource_read_name, NULL, res);
	kobj_add_regular(res->kobj, "id", resource_read_id, NULL, res);
	kobj_add(search_resource_kobj(res), res->kobj);
	rl->res = res;

	spin_lock_irq(&__resource_list_lock);
	list_add_tail(&rl->entry, &(__resource_list.entry));
	spin_unlock_irq(&__resource_list_lock);

	return TRUE;
}
{% endhighlight %}

###卸载资源
卸载一个资源，为“注册资源”的逆过程。同样，成功返回真，否则返回假。
{% highlight c %}
bool_t unregister_resource(struct resource_t * res)
{
	struct resource_list_t * pos, * n;

	if(!res || !res->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(pos->res == res)
		{
			spin_lock_irq(&__resource_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__resource_list_lock);

			kobj_remove(search_resource_kobj(res), pos->res->kobj);
			kobj_remove_self(res->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}
{% endhighlight %}

###递归资源
依据所提供的资源名称，自动递归所以同名的资源，并执行回调函数"fn",该回调函数所传递的第一个参数即为该资源对象。主要用途是在driver中自动递归同名resource，并生成的模块。
{% highlight c %}
void resource_for_each_with_name(const char * name, bool_t (*fn)(struct resource_t *))
{
	struct resource_list_t * pos, * n;

	if(!name || !fn)
		return;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(strcmp(pos->res->name, name) == 0)
		{
			if(fn(pos->res))
				LOG("Resource iterator with '%s.%d'", pos->res->name, pos->res->id);
			else
				LOG("Fail to resource iterator with '%s.%d'", pos->res->name, pos->res->id);
		}
	}
}
{% endhighlight %}

###总结
resouce对象本质上就是一堆数据，用于描述各种设备，只要我们正确描述了该设备，那么理论上该设备驱动就已经实现完了。敏捷开发，我想意义就在与此吧。进一步理解，可参见各个BSP包下的resource目录。resouce API归纳如下：
{% highlight c %}
bool_t register_resource(struct resource_t * res);
bool_t unregister_resource(struct resource_t * res);
void resource_for_each_with_name(const char * name, bool_t (*fn)(struct resource_t *));
{% endhighlight %}
