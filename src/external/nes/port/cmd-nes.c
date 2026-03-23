#include <xboot.h>
#include <command/command.h>
#include <framebuffer/framebuffer.h>
#include "nes.h"

// Basic wiring for the NES emulator directly to framebuffer.
static struct framebuffer_t * fb;
static void * fb_pixels;
static int fb_width, fb_height, fb_bytes_per_pixel;

static void usage(void)
{
	printf("usage:\r\n");
	printf("    nes <rom_address>\r\n");
}

int nes_draw(int x1, int y1, int x2, int y2, nes_color_t* color_data){
	if(!fb_pixels) return 0;

	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
	int i, j;

    // Calculate centering offsets
    int offset_x = (fb_width - 256) / 2;
    int offset_y = (fb_height - 240) / 2;
    if (offset_x < 0) offset_x = 0;
    if (offset_y < 0) offset_y = 0;

    // Clip to framebuffer limits
    int draw_width = width;
    if (x1 + offset_x + width > fb_width) draw_width = fb_width - x1 - offset_x;
    int draw_height = height;
    if (y1 + offset_y + height > fb_height) draw_height = fb_height - y1 - offset_y;

    uint32_t * dst = (uint32_t *)fb_pixels;
    for (j = 0; j < draw_height; j++) {
        for (i = 0; i < draw_width; i++) {
            dst[(y1 + offset_y + j) * fb_width + (x1 + offset_x + i)] = color_data[j * width + i];
        }
    }
    return 0;
}

static int do_nes(int argc, char ** argv)
{
	if(argc != 2) {
		usage();
		return -1;
	}

	const char * rom_path = argv[1];


	int fd = vfs_open(rom_path, O_RDONLY, 0);
	if(fd < 0) {
		printf("failed to open rom file '%s'\r\n", rom_path);
		return -1;
	}

	struct vfs_stat_t st;
	if (vfs_fstat(fd, &st) < 0) {
		printf("failed to stat rom file\r\n");
		vfs_close(fd);
		return -1;
	}

	uint8_t * rom_buf = malloc(st.st_size);
	if(!rom_buf) {
		printf("failed to allocate memory for rom\r\n");
		vfs_close(fd);
		return -1;
	}

	if(vfs_read(fd, rom_buf, st.st_size) != st.st_size) {
		printf("failed to read rom file\r\n");
		free(rom_buf);
		vfs_close(fd);
		return -1;
	}
	vfs_close(fd);

	nes_t * nes_ptr = nes_init();
	if(!nes_ptr) {
		printf("nes_init failed\r\n");
		free(rom_buf);
		return -1;
	}

	if(nes_load_rom(nes_ptr, rom_buf) < 0) {
		printf("nes_load_rom failed\r\n");
		free(rom_buf);
		return -1;
	}



    fb = search_first_framebuffer();
    if(!fb) {
        printf("Failed to find framebuffer\r\n");
        return -1;
    }

    fb_width = fb->width;
    fb_height = fb->height;
    fb_bytes_per_pixel = 4; // xboot surfaces are always 32-bit ARGB8888

    struct surface_t * s = fb->create ? fb->create(fb) : NULL;
    if(s) {
        fb_pixels = s->pixels;
    }

	printf("Starting NES emulator (Direct FB)... Press 'Ctrl+C' to exit\r\n");

	// Game loop
	while(1) {
		nes_frame(nes_ptr);

        if (fb->present) {
            fb->present(fb, s, NULL);
        }

		task_yield(); // Let other tasks run
	}

    if (s && fb->destroy) {
        fb->destroy(fb, s);
    }

	fb_pixels = NULL;
	return 0;
}


static struct command_t cmd_nes = {
	.name	= "nes",
	.desc	= "NES emulator",
	.usage	= usage,
	.exec	= do_nes,
};

static __init void nes_cmd_init(void)
{
	register_command(&cmd_nes);
}

static __exit void nes_cmd_exit(void)
{
	unregister_command(&cmd_nes);
}

command_initcall(nes_cmd_init);
command_exitcall(nes_cmd_exit);
