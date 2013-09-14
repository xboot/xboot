---
layout: post
title: Jekyll Markdown编辑参考示例
description: Markdown语法参考示例，在写作时可从此处拷贝。
categories: articles
key: Jekyll, Markdown
---

###"BLACKQUERY"标签：
> 人什么时候碰到麻烦？把简单的事情想复杂了；或者把复杂的事情想简单了。
> > 给我一个支点，我能撑起整个地球。给我一个画点函数，我能绘制整个世界。
> > > 只开风气，不为人师。胡言乱语，不知所云。

###"H"标签：
#提前优化是万恶之源(h1)

##提前优化是万恶之源(h2)

###提前优化是万恶之源(h3)

####提前优化是万恶之源(h4)

#####提前优化是万恶之源(h5)

#####提前优化是万恶之源(h6)

###"P"标签：

好代码本身就是最好的文档。当你需要添加一个注释时，你应该考虑如何修改代码才能不需要注释。——Steve McConnell，Code Complete 作者

###"LI"标签：

* 简约美，简单即是美，真切有力
* 结构美，可分可合，结构富于美感
* 境界美，一念动，万念相生，一波动，万波相随

###"代码高亮"

{% highlight c %}
/*
 * printk - Format a string, using utf-8 stream
 */
int printk(const char * fmt, ...)
{
	va_list ap;
	char *p, *buf;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	if(len < 0)
		return 0;
	buf = malloc(len + 1);
	if(!buf)
		return 0;
	len = vsnprintf(buf, len + 1, fmt, ap);
	va_end(ap);

	p = buf;
	len = 0;
	while(*p != '\0')
	{
		if(!console_stdout_putc(*p))
			break;
		p++;
		len++;
	}

	free(buf);
	return len;
}
{% endhighlight %}

###"IMG"标签：图片提供 3 种样式，分别是<code class="code">img-center</code>, <code class="code">img-right</code>, <code class="code">img-left</code>
<img src="/images/psb.jpg" class="img-center" width: 400px; alt="thumb" >

###"TABLE"标签：

<table width="100%">
	<tbody>
		<tr>
			<th width="20%">命令</th>
			<th width="80%">作用（解释)</th>
		</tr>
		<tr>
			<td>
				<code class="v-code">:w</code>
			</td>
			<td>保存</td>
		</tr>
		<tr>
			<td>
				<code class="v-code">:wq</code>,
				<code class="v-code">:x</code>
			</td>
			<td>保存并关闭</td>
		</tr>
		<tr>
			<td>
				<code class="v-code">:q</code>
			</td>
			<td>关闭（已保存）</td>
		</tr>
		<tr>
			<td>
				<code class="v-code">:q!</code>
			</td>
			<td>强制关闭</td>
		</tr>
	</tbody>
</table>

