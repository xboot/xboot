---
layout: post
title: xboot中kobj设计技术详解
description: kobj顾名思意，即内核object，其层次结构是一颗倒置的树，用来管理包括各种驱动模块，核心组件，运行时状态等。在文件系统中，通过sysfs接口绑定kobj节点，来实现对该节点的读写操作。sysfs接口有两种最基本的用途：1，组件的外部操作接口，比如应用可以通过此接口操作设备。2，调试驱动模块，或查看运行时状态。
categories: articles
key: xboot, kobj
---

###引言
> 实现kobj可以对任何形态的组件实现高度抽象，是各种组件的基石，系统中提供了一组API，以方便管理kobj。理解kobj，对后面理解xboot中的各种组件而言有相当大的帮助，建议细心阅读。

###kobj节点类型
kobj有两种基本类型，一个是目录，其可以挂接子节点，另一个是文件，代表一颗树的末端，不可拥有子节点，但提供了读或者写该节点的操作，声明如下:
{% highlight c %}
enum kobj_type_t {
	KOBJ_TYPE_DIR,
	KOBJ_TYPE_REG,
};
{% endhighlight %}

###kobj结构体定义
kobj结构体拥有名字，节点类型，父节点指针，当前节点链表，子节点链表头，节点锁，读写接口函数及一个私有数据指针，详细定义如下:
{% highlight c %}
struct kobj_t
{
	/* kobj name */
	char * name;

	/* kobj type DIR or REG */
	enum kobj_type_t type;

	/* kobj's parent */
	struct kobj_t * parent;

	/* kobj's entry */
	struct list_head entry;

	/* kobj's children */
	struct list_head children;

	/* kobj lock */
	spinlock_t lock;

	/* kobj read */
	ssize_t (*read)(struct kobj_t * kobj, void * buf, size_t size);

	/* kobj write */
	ssize_t (*write)(struct kobj_t * kobj, void * buf, size_t size);

	/* private data */
	void * priv;
};
{% endhighlight %}

###分配kobj节点
其为一内部函数，主要是分配一段内存，然后利用传递的参数进行初始化，并返回分配的kobj节点。
{% highlight c %}
static struct kobj_t * __kobj_alloc(const char * name, enum kobj_type_t type, kobj_read_t read, kobj_write_t write, void * priv)
{
	struct kobj_t * kobj;

	if(!name)
		return NULL;

	kobj = malloc(sizeof(struct kobj_t));
	if(!kobj)
		return NULL;

	kobj->name = strdup(name);
	kobj->type = type;
	kobj->parent = kobj;
	init_list_head(&kobj->entry);
	init_list_head(&kobj->children);
	spin_lock_init(&kobj->lock);
	kobj->read = read;
	kobj->write = write;
	kobj->priv = priv;

	return kobj;
}
{% endhighlight %}

###分配目录或文件节点
其为函数为内部函数__kobj_alloc的二次封装，快速分配指定类型的节点。分配一个目录节点实现如下：
{% highlight c %}
struct kobj_t * kobj_alloc_directory(const char * name)
{
	return __kobj_alloc(name, KOBJ_TYPE_DIR, NULL, NULL, NULL);
}
{% endhighlight %}

分配一个文件节点实现如下：
{% highlight c %}
struct kobj_t * kobj_alloc_regular(const char * name, kobj_read_t read, kobj_write_t write, void * priv)
{
	return __kobj_alloc(name, KOBJ_TYPE_REG, read, write, priv);
}
{% endhighlight %}

###释放kobj节点
将动态分配的内存进行回收，实现如下：
{% highlight c %}
bool_t kobj_free(struct kobj_t * kobj)
{
	if(!kobj)
		return FALSE;

	free(kobj->name);
	free(kobj);
	return TRUE;
}
{% endhighlight %}

###搜索kobj节点
从一个父节点中搜索一个名为name的子节点。
{% highlight c %}
struct kobj_t * kobj_search(struct kobj_t * parent, const char * name)
{
	struct kobj_t * pos, * n;

	if(!parent)
		return NULL;

	if(parent->type != KOBJ_TYPE_DIR)
		return NULL;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(parent->children), entry)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}

	return NULL;
}
{% endhighlight %}

###搜索kobj节点，如不存在该子节点则创建之。
与上一个函数类似，不同之处在于未搜索到名为name的子节点时，自动创建一个子节点，该节点为目录类型。
{% highlight c %}
struct kobj_t * kobj_search_directory_with_create(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;

	if(!parent)
		return NULL;

	if(parent->type != KOBJ_TYPE_DIR)
		return NULL;

	if(!name)
		return NULL;

	kobj = kobj_search(parent, name);
	if(!kobj)
	{
		kobj = kobj_alloc_directory(name);
		if(!kobj)
			return NULL;

		if(!kobj_add(parent, kobj))
		{
			kobj_free(kobj);
			return NULL;
		}
	}
	else if(kobj->type != KOBJ_TYPE_DIR)
	{
		return NULL;
	}

	return kobj;
}
{% endhighlight %}

###添加kobj节点
将一个节点，可以是目录节点或文件节点，添加至父目录节点，成功返回真，否则返回假。
{% highlight c %}
bool_t kobj_add(struct kobj_t * parent, struct kobj_t * kobj)
{
	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!kobj)
		return FALSE;

	if(kobj_search(parent, kobj->name))
		return FALSE;

	spin_lock_irq(&parent->lock);
	spin_lock_irq(&kobj->lock);

	kobj->parent = parent;
	list_add_tail(&kobj->entry, &parent->children);

	spin_unlock_irq(&kobj->lock);
	spin_unlock_irq(&parent->lock);

	return TRUE;
}
{% endhighlight %}

###删除kobj节点
从一个父目录节点中删除一个子节点，为“添加kobj节点”的逆过程。同样，成功返回真，否则返回假。
{% highlight c %}
bool_t kobj_remove(struct kobj_t * parent, struct kobj_t * kobj)
{
	struct kobj_t * pos, * n;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!kobj)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(parent->children), entry)
	{
		if(pos == kobj)
		{
			spin_lock_irq(&parent->lock);
			spin_lock_irq(&kobj->lock);

			pos->parent = pos;
			list_del(&(pos->entry));

			spin_unlock_irq(&kobj->lock);
			spin_unlock_irq(&parent->lock);

			return TRUE;
		}
	}

	return FALSE;
}
{% endhighlight %}

###快捷函数之添加目录节点或文件节点
此接口实现的目的是为了快速添加子节点，其根据所传递的参数，自动创建节点并添加到父节点上，添加子目录节点：
{% highlight c %}
bool_t kobj_add_directory(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!name)
		return FALSE;

	if(kobj_search(parent, name))
		return FALSE;

	kobj = kobj_alloc_directory(name);
	if(!kobj)
		return FALSE;

	if(!kobj_add(parent, kobj))
		kobj_free(kobj);

	return TRUE;
}

{% endhighlight %}

添加子文件节点：
{% highlight c %}
bool_t kobj_add_regular(struct kobj_t * parent, const char * name, kobj_read_t read, kobj_write_t write, void * priv)
{
	struct kobj_t * kobj;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!name)
		return FALSE;

	if(kobj_search(parent, name))
		return FALSE;

	kobj = kobj_alloc_regular(name, read, write, priv);
	if(!kobj)
		return FALSE;

	if(!kobj_add(parent, kobj))
		kobj_free(kobj);

	return TRUE;
}
{% endhighlight %}

###递归删除节点
此接口会将自身及其所以子节点以递归方式删除：
{% highlight c %}
bool_t kobj_remove_self(struct kobj_t * kobj)
{
	struct kobj_t * parent;
	struct kobj_t * pos, * n;
	bool_t ret;

	if(!kobj)
		return FALSE;

	if(kobj->type == KOBJ_TYPE_DIR)
	{
		list_for_each_entry_safe(pos, n, &(kobj->children), entry)
		{
			kobj_remove_self(pos);
		}
	}

	parent = kobj->parent;
	if(parent && (parent != kobj))
	{
		ret = kobj_remove(parent, kobj);
		if(ret)
			kobj_free(kobj);
		return ret;
	}

	kobj_free(kobj);
	return TRUE;
}
{% endhighlight %}

###根节点
系统启动时，会自动创建一个根节点，名为"kobj",该节点为一个全局静态变量，是顶层目录节点，在mount文件系统时会挂接到sysfs接口。
{% highlight c %}
void do_init_kobj(void)
{
	__kobj_root = kobj_alloc_directory("kobj");
}
{% endhighlight %}

###总结
对于如何使用kobj,请参见各种驱动模块的实现，每个驱动模块都含有一个kobj对象，在注册设备时，会依据设备的名称及类型自动创建一组kobj对象。如何通过文件系统访问kobj，则参见虚拟文件系统中的sysfs文件系统的实现。综上所述，kobj API归纳如下：
{% highlight c %}
struct kobj_t * kobj_get_root(void);
struct kobj_t * kobj_search(struct kobj_t * parent, const char * name);
struct kobj_t * kobj_search_directory_with_create(struct kobj_t * parent, const char * name);
struct kobj_t * kobj_alloc_directory(const char * name);
struct kobj_t * kobj_alloc_regular(const char * name, kobj_read_t read, kobj_write_t write, void * priv);
bool_t kobj_free(struct kobj_t * kobj);
bool_t kobj_add(struct kobj_t * parent, struct kobj_t * kobj);
bool_t kobj_remove(struct kobj_t * parent, struct kobj_t * kobj);
bool_t kobj_add_directory(struct kobj_t * parent, const char * name);
bool_t kobj_add_regular(struct kobj_t * parent, const char * name, kobj_read_t read, kobj_write_t write, void * priv);
bool_t kobj_remove_self(struct kobj_t * kobj);
{% endhighlight %}
