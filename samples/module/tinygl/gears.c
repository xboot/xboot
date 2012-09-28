/* gears.c */

/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Brian Paul
 */

#include <math.h>
#include <stdlib.h>

#include <GL/gl.h>
static GLfloat Xrot, Xstep;
static GLfloat Yrot, Ystep;
static GLfloat Zrot, Zstep;
static GLfloat Step = 1.0;
static GLfloat Scale = 1.5;
static GLuint Object;

static GLuint make_object( void )
{
    GLuint list;

    list = glGenLists( 1 );

    glNewList( list, GL_COMPILE );

    glBegin( GL_LINE_LOOP );
    glColor3f( 1.0, 1.0, 1.0 );
    glVertex3f(  1.0,  0.5, -0.4 );
    glColor3f( 1.0, 0.0, 0.0 );
    glVertex3f(  1.0, -0.5, -0.4 );
    glColor3f( 0.0, 1.0, 0.0 );
    glVertex3f( -1.0, -0.5, -0.4 );
    glColor3f( 0.0, 0.0, 1.0 );
    glVertex3f( -1.0,  0.5, -0.4 );
    glEnd();

    glColor3f( 1.0, 1.0, 1.0 );

    glBegin( GL_LINE_LOOP );
    glVertex3f(  1.0,  0.5, 0.4 );
    glVertex3f(  1.0, -0.5, 0.4 );
    glVertex3f( -1.0, -0.5, 0.4 );
    glVertex3f( -1.0,  0.5, 0.4 );
    glEnd();

    glBegin( GL_LINES );
    glVertex3f(  1.0,  0.5, -0.4 );   glVertex3f(  1.0,  0.5, 0.4 );
    glVertex3f(  1.0, -0.5, -0.4 );   glVertex3f(  1.0, -0.5, 0.4 );
    glVertex3f( -1.0, -0.5, -0.4 );   glVertex3f( -1.0, -0.5, 0.4 );
    glVertex3f( -1.0,  0.5, -0.4 );   glVertex3f( -1.0,  0.5, 0.4 );
    glEnd();


    glEndList();

    return list;
}

void reshape( int width, int height )
{
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
    glMatrixMode(GL_MODELVIEW);
}


void draw( void )
{
    glClear( GL_COLOR_BUFFER_BIT );

    glPushMatrix();

    glTranslatef( 0.0, 0.0, -10.0 );
    glScalef( Scale, Scale, Scale );
    if (Xstep) {
        glRotatef( Xrot, 1.0, 0.0, 0.0 );
    }
    else if (Ystep) {
        glRotatef( Yrot, 0.0, 1.0, 0.0 );
    }
    else {
        glRotatef( Zrot, 0.0, 0.0, 1.0 );
    }

    glCallList( Object );

    glPopMatrix();

    glFlush();
    vgafb_swap_buffer();
}

void idle( void )
{
    Xrot += Xstep + 1;
    Yrot += Ystep + 2;
    Zrot += Zstep + 3;

    if (Xrot>=360.0) {
        Xrot = Xstep = 0.0;
        Ystep = Step + 0;
    }
    else if (Yrot>=360.0) {
        Yrot = Ystep = 0.0;
        Zstep = Step + 0;
    }
    else if (Zrot>=360.0) {
        Zrot = Zstep = 0.0;
       Xstep = Step + 0;
    }
    draw();
}

void init(void)
{
    Object = make_object();
    glCullFace( GL_BACK );
    /*   glEnable( GL_CULL_FACE );*/
    glDisable( GL_DITHER );
    glShadeModel( GL_FLAT );
    /*   glEnable( GL_DEPTH_TEST ); */

    Xrot = Yrot = Zrot = 0.0;
    Xstep = Step;
    Ystep = Zstep = 0.0;
}

void module_main(void)
{
	int count = 0;
	struct vgafb_context *ctx = NULL;
	printk("=====1======\r\n");

	if (vgafb_create_context(&ctx) != 0)
        return;
	printk("=====2======\r\n");
	vgafb_make_current(ctx, 0);
	printk("=====3======\r\n");
    init();
	printk("=====4xx======\r\n");
	reshape(400,200);
	printk("=====4======\r\n");
    while(1)
    {
		//printk("=====5======\r\n");
    	idle();
	if(count++ > 100)
		return;
    }
}
