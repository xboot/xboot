#include <graphic/surface.h>
#include <fb/fb.h>
#include <GL/vgafb.h>
#include <GL/gl.h>

/* for GLContext definition */
#include "zgl.h"

struct vgafb_context {
    GLContext *gl_context;
    struct fb *fb_dev;
};

static int vgafb_resize_viewport(GLContext *ctx, int *xsize, int *ysize);

bool_t vgafb_create_context(struct vgafb_context **ctx )
{
    struct vgafb_context *c;

    if ((c = (struct vgafb_context*)gl_malloc(sizeof(struct vgafb_context))) == NULL)
        return 1;

    c->gl_context = NULL;

    *ctx = c;
    return 0;
}

bool_t vgafb_destroy_context(struct vgafb_context *ctx)
{
    if (ctx->gl_context != NULL)
        glClose();

    free(ctx);

    return 0;
}

/* connect a framebuffer device to a context */
bool_t vgafb_make_current(struct vgafb_context *ctx, struct fb *fb_dev)
{

    if (ctx->gl_context == NULL)
    {
        int i;

        /* Open a ZBuffer
         *  (320x200 pixels, 8 bits color index with palette) */
        ZBuffer *zb;

        unsigned char color_indexes[ZB_NB_COLORS];
        int palette[ZB_NB_COLORS];

        /* values correspond to indexes */
        for (i=0; i<ZB_NB_COLORS; i++)
            color_indexes[i]=i;

    	struct fb * fb;
    	struct surface_t * screen;

    	fb = search_framebuffer("fb");
    	screen = &fb->info->surface;
    	fb_dev = fb;

        if ((zb = ZB_open(800, 480, ZB_MODE_RGBA, ZB_NB_COLORS, color_indexes, palette, 0)) == NULL)
            return 1;





        /* Init TinyGL interpreter */
        glInit(zb);
        ctx->gl_context = gl_get_context();
        ctx->gl_context->opaque = (void*)ctx;

        /* set the viewport
         *  we do not provide this now, since we can't resize */
        //ctx->gl_context->gl_resize_viewport = vgafb_resize_viewport;
        //ctx->gl_context->viewport.xsize = -1;
        //ctx->gl_context->viewport.ysize = -1;
        //glViewport(0, 0, 320, 200);
    }

    ctx->fb_dev = fb_dev;

    return 0;
}

bool_t vgafb_swap_buffer()
{
    GLContext *gl_context;
    struct vgafb_context *ctx;

    /* retrieve current vgafb_context */
    gl_context = gl_get_context();
    ctx = (struct vgafb_context*)gl_context->opaque;

    /* get framebuffer ram address */
    struct surface_t * screen;
    screen = &ctx->fb_dev->info->surface;

    /* Update framebuffer (the linesize is xsize*2 since tinygl works on 16 bits values */
    int linesize = gl_context->zb->xsize*4;
    ZB_copyFrameBuffer(gl_context->zb, screen->pixels, linesize);

    return 0;
}

static int vgafb_resize_viewport(GLContext *ctx, int *xsize, int *ysize)
{
    /* we can authorize only these values */
    *xsize = 320;
    *ysize = 200;

    return 0;
}
