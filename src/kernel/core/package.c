/*
 * kernel/core/package.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <xboot/package.h>

static struct surface_t * __package_icon = NULL;
static struct surface_t * __package_panel = NULL;
struct hmap_t * __package_list = NULL;

static struct package_t * package_alloc(const char * path, const char * lang)
{
	struct package_t * pkg = NULL;
	struct xfs_context_t * ctx;
	struct xfs_file_t * file;
	struct json_value_t * v, * w, * t;
	char * json, * p;
	size_t len;
	int i, j;

	ctx = xfs_alloc(path, 0);
	if(ctx && xfs_isfile(ctx, "main.lua"))
	{
		pkg = malloc(sizeof(struct package_t));
		if(pkg)
		{
			memset(pkg, 0, sizeof(struct package_t));
			pkg->path = strdup(path);

			file = xfs_open_read(ctx, "manifest.json");
			if(file && (len = xfs_length(file)) > 0)
			{
				json = malloc(len + 1);
				if(json && (len = xfs_read(file, json, len)) > 0)
				{
					v = json_parse(json, len, NULL);
					if(v && (v->type == JSON_OBJECT))
					{
						for(i = 0; i < v->u.object.length; i++)
						{
							if(v->u.object.values[i].value->type == JSON_OBJECT)
							{
								if(lang && (strcmp(v->u.object.values[i].name, lang) == 0))
								{
									w = v->u.object.values[i].value;
									if(w && (w->type == JSON_OBJECT))
									{
										for(j = 0; j < w->u.object.length; j++)
										{
											if(w->u.object.values[j].value->type == JSON_STRING)
											{
												p = w->u.object.values[j].name;
												if(strcmp(p, "name") == 0)
												{
													t = w->u.object.values[j].value;
													if(t && (t->type == JSON_STRING))
														pkg->name = strdup(t->u.string.ptr);
												}
												else if(strcmp(p, "description") == 0)
												{
													t = w->u.object.values[j].value;
													if(t && (t->type == JSON_STRING))
														pkg->desc = strdup(t->u.string.ptr);
												}
											}
										}
									}
								}
							}
						}

						if(!pkg->name && !pkg->desc)
						{
							for(i = 0; i < v->u.object.length; i++)
							{
								if(v->u.object.values[i].value->type == JSON_STRING)
								{
									p = v->u.object.values[i].name;
									if(strcmp(p, "name") == 0)
									{
										t = v->u.object.values[i].value;
										if(t && (t->type == JSON_STRING))
											pkg->name = strdup(t->u.string.ptr);
									}
									else if(strcmp(p, "description") == 0)
									{
										t = v->u.object.values[i].value;
										if(t && (t->type == JSON_STRING))
											pkg->desc = strdup(t->u.string.ptr);
									}
								}
							}
						}

						for(i = 0; i < v->u.object.length; i++)
						{
							if(v->u.object.values[i].value->type == JSON_STRING)
							{
								p = v->u.object.values[i].name;
								if(strcmp(p, "developer") == 0)
								{
									t = v->u.object.values[i].value;
									if(t && (t->type == JSON_STRING))
										pkg->developer = strdup(t->u.string.ptr);
								}
								else if(strcmp(p, "version") == 0)
								{
									t = v->u.object.values[i].value;
									if(t && (t->type == JSON_STRING))
										pkg->version = strdup(t->u.string.ptr);
								}
								else if(strcmp(p, "url") == 0)
								{
									t = v->u.object.values[i].value;
									if(t && (t->type == JSON_STRING))
										pkg->url = strdup(t->u.string.ptr);
								}
							}
						}
					}
					json_free(v);
				}
				free(json);
			}
			xfs_close(file);

			if(!pkg->name)
			{
				p = strdup(path);
				pkg->name = strdup(basename(p));
				free(p);
			}

			pkg->icon = surface_alloc_from_xfs(ctx, "icon.png");
			pkg->panel = surface_alloc_from_xfs(ctx, "panel.png");
		}
		xfs_free(ctx);
	}
	return pkg;
}

static void package_free(struct package_t * pkg)
{
	if(pkg)
	{
		if(pkg->path)
			free((void *)pkg->path);
		if(pkg->name)
			free((void *)pkg->name);
		if(pkg->desc)
			free((void *)pkg->desc);
		if(pkg->developer)
			free((void *)pkg->developer);
		if(pkg->version)
			free((void *)pkg->version);
		if(pkg->url)
			free((void *)pkg->url);
		if(pkg->icon)
			surface_free(pkg->icon);
		if(pkg->panel)
			surface_free(pkg->panel);
		free(pkg);
	}
}

struct package_t * package_search(const char * path)
{
	return hmap_search(__package_list, path);
}

const char * package_get_path(struct package_t * pkg)
{
	return pkg->path;
}

const char * package_get_name(struct package_t * pkg)
{
	return pkg->name;
}

const char * package_get_desc(struct package_t * pkg)
{
	return pkg->desc ? pkg->desc : "None";
}

const char * package_get_developer(struct package_t * pkg)
{
	return pkg->developer ? pkg->developer : "Unkown";
}

const char * package_get_version(struct package_t * pkg)
{
	return pkg->version ? pkg->version : "0.0.0";
}

const char * package_get_url(struct package_t * pkg)
{
	return pkg->url ? pkg->url : "http://";
}

struct surface_t * package_get_icon(struct package_t * pkg)
{
	return pkg->icon ? pkg->icon : __package_icon;
}

struct surface_t * package_get_panel(struct package_t * pkg)
{
	return pkg->panel ? pkg->panel : __package_panel;
}

static void hmap_entry_callback(struct hmap_entry_t * e)
{
	if(e)
		package_free(e->value);
}

void package_rescan(void)
{
	struct package_t * pkg;
	struct vfs_stat_t st;
	struct vfs_dirent_t dir;
	struct slist_t * sl, * e;
	const char * lang = setting_get("language", NULL);
	const char * path;
	int fd;

	hmap_clear(__package_list, hmap_entry_callback);
	sl = slist_alloc();
	path = "/application";
	if(vfs_stat(path, &st) >= 0 && S_ISDIR(st.st_mode))
	{
		if((fd = vfs_opendir(path)) >= 0)
		{
			while(vfs_readdir(fd, &dir) >= 0)
			{
				if(dir.d_name && (dir.d_name[0] == '.'))
					continue;
				slist_add(sl, NULL, "%s/%s", path, dir.d_name);
			}
			vfs_closedir(fd);
		}
	}
	path = "/private/application";
	if(vfs_stat(path, &st) >= 0 && S_ISDIR(st.st_mode))
	{
		if((fd = vfs_opendir(path)) >= 0)
		{
			while(vfs_readdir(fd, &dir) >= 0)
			{
				if(dir.d_name && (dir.d_name[0] == '.'))
					continue;
				slist_add(sl, NULL, "%s/%s", path, dir.d_name);
			}
			vfs_closedir(fd);
		}
	}
	slist_sort(sl);
	slist_for_each_entry(e, sl)
	{
		pkg = package_alloc(e->key, lang);
		if(pkg)
			hmap_add(__package_list, e->key, pkg);
	}
	slist_free(sl);
}

void do_init_package(void)
{
	struct xfs_context_t * ctx;

	ctx = xfs_alloc("/private/framework", 0);
	if(ctx)
	{
		__package_icon = surface_alloc_from_xfs(ctx, "assets/images/icon.png");
		__package_panel = surface_alloc_from_xfs(ctx, "assets/images/panel.png");
		xfs_free(ctx);
	}
	__package_list = hmap_alloc(0);
	package_rescan();
}
