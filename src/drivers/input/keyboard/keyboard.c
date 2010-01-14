/*
 * drivers/input/keyboard/keyboard.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <fifo.h>
#include <xboot/chrdev.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <xboot/resource.h>
#include <terminal/terminal.h>
#include <time/timer.h>
#include <input/keyboard/keyboard.h>

/*
 * defined the struct of keyboard terminal information.
 */
struct keyboard_terminal_info
{
	/* the tty name. */
	char name[32+1];

	/* keyboard driver */
	struct keyboard_driver * drv;

	/* the keyboard's fifo */
	struct fifo * fifo;
};

/*
 * keyboard open
 */
static x_s32 keyboard_open(struct chrdev * dev)
{
	return 0;
}

/*
 * keyboard seek
 */
static x_s32 keyboard_seek(struct chrdev * dev, x_s32 offset)
{
	return (-1);
}

/*
 * keyboard read
 */
static x_s32 keyboard_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct keyboard_driver * drv = (struct keyboard_driver *)(dev->driver);
	enum keycode code;

	if(drv->read)
	{
		if(drv->read(&code))
		{
			memcpy(buf, (&code), sizeof(enum keycode));
			return (sizeof(enum keycode));
		}
	}

	return 0;
}

/*
 * keyboard write.
 */
static x_s32 keyboard_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	return -1;
}

/*
 * keyboard flush
 */
static x_s32 keyboard_flush(struct chrdev * dev)
{
	struct keyboard_driver * drv = (struct keyboard_driver *)(dev->driver);

	if(drv->flush)
		(drv->flush)();

	return 0;
}

/*
 * keyboard ioctl
 */
static x_s32 keyboard_ioctl(struct chrdev * dev, x_u32 cmd, x_u32 arg)
{
	struct keyboard_driver * drv = (struct keyboard_driver *)(dev->driver);

	if(drv->ioctl)
		return ((drv->ioctl)(cmd, arg));

	return -1;
}

/*
 * terminal read
 */
static x_s32 keyboard_term_read(struct terminal * term, x_u8 * buf, x_s32 count)
{
	struct keyboard_terminal_info * info = term->priv;
	enum keycode code;
	x_u8 key[8];
	x_u32 len;

	if(info->drv->read(&code))
	{
		switch(code)
		{
		case KEY_NULL:
			len=0;
			break;

		case KEY_0:
			key[0] = '0';
			len = 1;
			break;

		case KEY_1:
			key[0] = '1';
			len = 1;
			break;

		case KEY_2:
			key[0] = '2';
			len = 1;
			break;

		case KEY_3:
			key[0] = '3';
			len = 1;
			break;

		case KEY_4:
			key[0] = '4';
			len = 1;
			break;

		case KEY_5:
			key[0] = '5';
			len = 1;
			break;

		case KEY_6:
			key[0] = '6';
			len = 1;
			break;

		case KEY_7:
			key[0] = '7';
			len = 1;
			break;

		case KEY_8:
			key[0] = '8';
			len = 1;
			break;

		case KEY_9:
			key[0] = '9';
			len = 1;
			break;

		case KEY_a:
			key[0] = 'a';
			len = 1;
			break;

		case KEY_b:
			key[0] = 'b';
			len = 1;
			break;

		case KEY_c:
			key[0] = 'c';
			len = 1;
			break;

		case KEY_d:
			key[0] = 'd';
			len = 1;
			break;

		case KEY_e:
			key[0] = 'e';
			len = 1;
			break;

		case KEY_f:
			key[0] = 'f';
			len = 1;
			break;

		case KEY_g:
			key[0] = 'g';
			len = 1;
			break;

		case KEY_h:
			key[0] = 'h';
			len = 1;
			break;

		case KEY_i:
			key[0] = 'i';
			len = 1;
			break;

		case KEY_j:
			key[0] = 'j';
			len = 1;
			break;

		case KEY_k:
			key[0] = 'k';
			len = 1;
			break;

		case KEY_l:
			key[0] = 'l';
			len = 1;
			break;

		case KEY_m:
			key[0] = 'm';
			len = 1;
			break;

		case KEY_n:
			key[0] = 'n';
			len = 1;
			break;

		case KEY_o:
			key[0] = 'o';
			len = 1;
			break;

		case KEY_p:
			key[0] = 'p';
			len = 1;
			break;
		case KEY_q:
			key[0] = 'q';
			len = 1;
			break;

		case KEY_r:
			key[0] = 'r';
			len = 1;
			break;

		case KEY_s:
			key[0] = 's';
			len = 1;
			break;

		case KEY_t:
			key[0] = 't';
			len = 1;
			break;

		case KEY_u:
			key[0] = 'u';
			len = 1;
			break;

		case KEY_v:
			key[0] = 'v';
			len = 1;
			break;

		case KEY_w:
			key[0] = 'w';
			len = 1;
			break;

		case KEY_x:
			key[0] = 'x';
			len = 1;
			break;

		case KEY_y:
			key[0] = 'y';
			len = 1;
			break;

		case KEY_z:
			key[0] = 'z';
			len = 1;
			break;

		case KEY_A:
			key[0] = 'A';
			len = 1;
			break;

		case KEY_B:
			key[0] = 'B';
			len = 1;
			break;

		case KEY_C:
			key[0] = 'C';
			len = 1;
			break;

		case KEY_D:
			key[0] = 'D';
			len = 1;
			break;

		case KEY_E:
			key[0] = 'E';
			len = 1;
			break;

		case KEY_F:
			key[0] = 'F';
			len = 1;
			break;

		case KEY_G:
			key[0] = 'G';
			len = 1;
			break;

		case KEY_H:
			key[0] = 'H';
			len = 1;
			break;

		case KEY_I:
			key[0] = 'I';
			len = 1;
			break;

		case KEY_J:
			key[0] = 'J';
			len = 1;
			break;

		case KEY_K:
			key[0] = 'K';
			len = 1;
			break;

		case KEY_L:
			key[0] = 'L';
			len = 1;
			break;

		case KEY_M:
			key[0] = 'M';
			len = 1;
			break;

		case KEY_N:
			key[0] = 'N';
			len = 1;
			break;

		case KEY_O:
			key[0] = 'O';
			len = 1;
			break;

		case KEY_P:
			key[0] = 'P';
			len = 1;
			break;

		case KEY_Q:
			key[0] = 'Q';
			len = 1;
			break;

		case KEY_R:
			key[0] = 'R';
			len = 1;
			break;

		case KEY_S:
			key[0] = 'S';
			len = 1;
			break;

		case KEY_T:
			key[0] = 'T';
			len = 1;
			break;

		case KEY_U:
			key[0] = 'U';
			len = 1;
			break;

		case KEY_V:
			key[0] = 'V';
			len = 1;
			break;

		case KEY_W:
			key[0] = 'W';
			len = 1;
			break;

		case KEY_X:
			key[0] = 'X';
			len = 1;
			break;

		case KEY_Y:
			key[0] = 'Y';
			len = 1;
			break;

		case KEY_Z:
			key[0] = 'Z';
			len = 1;
			break;

		case KEY_F1:
		case KEY_F2:
		case KEY_F3:
		case KEY_F4:
		case KEY_F5:
		case KEY_F6:
		case KEY_F7:
		case KEY_F8:
		case KEY_F9:
		case KEY_F10:
		case KEY_F11:
		case KEY_F12:
			len = 0;
			break;

		case KEY_UNQUOTE:
			key[0] = '`';
			len = 1;
			break;

		case KEY_WAVE:
			key[0] = '~';
			len = 1;
			break;

		case KEY_EXCLAMATION_MARK:
			key[0] = '!';
			len = 1;
			break;

		case KEY_AT:
			key[0] = '@';
			len = 1;
			break;

		case KEY_POUNDSIGN:
			key[0] = '#';
			len = 1;
			break;

		case KEY_DOLLAR:
			key[0] = '$';
			len = 1;
			break;

		case KEY_PERCENT:
			key[0] = '%';
			len = 1;
			break;

		case KEY_POWER:
			key[0] = '^';
			len = 1;
			break;

		case KEY_AND:
			key[0] = '&';
			len = 1;
			break;

		case KEY_STAR:
			key[0] = '*';
			len = 1;
			break;

		case KEY_MINUS:
			key[0] = '-';
			len = 1;
			break;

		case KEY_PLUS:
			key[0] = '+';
			len = 1;
			break;

		case KEY_EQUAL:
			key[0] = '=';
			len = 1;
			break;

		case KEY_OR:
			key[0] = '|';
			len = 1;
			break;

		case KEY_SLASH:
			key[0] = '/';
			len = 1;
			break;

		case KEY_BACKLASH:
			key[0] = '\\';
			len = 1;
			break;

		case KEY_UNDERLINE:
			key[0] = '_';
			len = 1;
			break;

		case KEY_COMMA:
			key[0] = ',';
			len = 1;
			break;

		case KEY_PERIOD:
			key[0] = '.';
			len = 1;
			break;

		case KEY_QUESTION_MARK:
			key[0] = '?';
			len = 1;
			break;

		case KEY_COLON:
			key[0] = ':';
			len = 1;
			break;

		case KEY_SEMICOLON:
			key[0] = ';';
			len = 1;
			break;

		case KEY_SINGLE_QUOTES:
			key[0] = '\'';
			len = 1;
			break;

		case KEY_DOUBLE_QUOTES:
			key[0] = '\"';
			len = 1;
			break;

		case KEY_ANGLE_BRACKET_LEFT:
			key[0] = '<';
			len = 1;
			break;

		case KEY_ANGLE_BRACKET_RIGHT:
			key[0] = '>';
			len = 1;
			break;

		case KEY_ROUND_BRACKET_LEFT:
			key[0] = '(';
			len = 1;
			break;

		case KEY_ROUND_BRACKET_RIGHT:
			key[0] = ')';
			len = 1;
			break;

		case KEY_SQUARE_BRACKET_LEFT:
			key[0] = '[';
			len = 1;
			break;

		case KEY_SQUARE_BRACKET_RIGHT:
			key[0] = ']';
			len = 1;
			break;

		case KEY_BRACE_LEFT:
			key[0] = '{';
			len = 1;
			break;

		case KEY_BRACE_RIGHT:
			key[0] = '}';
			len = 1;
			break;

		case KEY_CTRL_0:
		case KEY_CTRL_1:
		case KEY_CTRL_2:
		case KEY_CTRL_3:
		case KEY_CTRL_4:
		case KEY_CTRL_5:
		case KEY_CTRL_6:
		case KEY_CTRL_7:
		case KEY_CTRL_8:
		case KEY_CTRL_9:
			len = 0;
			break;

		case KEY_CTRL_A:
			key[0] = 1;
			len = 1;
			break;

		case KEY_CTRL_B:
			key[0] = 2;
			len = 1;
			break;

		case KEY_CTRL_C:
			key[0] = 3;
			len = 1;
			break;

		case KEY_CTRL_D:
			key[0] = 4;
			len = 1;
			break;

		case KEY_CTRL_E:
			key[0] = 5;
			len = 1;
			break;

		case KEY_CTRL_F:
			key[0] = 6;
			len = 1;
			break;

		case KEY_CTRL_G:
		case KEY_CTRL_H:
		case KEY_CTRL_I:
		case KEY_CTRL_J:
			len = 0;
			break;

		case KEY_CTRL_K:
			key[0] = 11;
			len = 1;
			break;

		case KEY_CTRL_L:
		case KEY_CTRL_M:
		case KEY_CTRL_N:
		case KEY_CTRL_O:
		case KEY_CTRL_P:
		case KEY_CTRL_Q:
			len = 0;
			break;

		case KEY_CTRL_R:
			key[0] = 18;
			len = 1;
			break;

		case KEY_CTRL_S:
		case KEY_CTRL_T:
		case KEY_CTRL_U:
		case KEY_CTRL_V:
		case KEY_CTRL_W:
			len = 0;
			break;

		case KEY_CTRL_X:
			key[0] = 24;
			len = 1;
			break;

		case KEY_CTRL_Y:
		case KEY_CTRL_Z:
			len = 0;
			break;

		case KEY_ALT_0:
		case KEY_ALT_1:
		case KEY_ALT_2:
		case KEY_ALT_3:
		case KEY_ALT_4:
		case KEY_ALT_5:
		case KEY_ALT_6:
		case KEY_ALT_7:
		case KEY_ALT_8:
		case KEY_ALT_9:
			len = 0;
			break;

		case KEY_ALT_A:
		case KEY_ALT_B:
		case KEY_ALT_C:
		case KEY_ALT_D:
		case KEY_ALT_E:
		case KEY_ALT_F:
		case KEY_ALT_G:
		case KEY_ALT_H:
		case KEY_ALT_I:
		case KEY_ALT_J:
		case KEY_ALT_K:
		case KEY_ALT_L:
		case KEY_ALT_M:
		case KEY_ALT_N:
		case KEY_ALT_O:
		case KEY_ALT_P:
		case KEY_ALT_Q:
		case KEY_ALT_R:
		case KEY_ALT_S:
		case KEY_ALT_T:
		case KEY_ALT_U:
		case KEY_ALT_V:
		case KEY_ALT_W:
		case KEY_ALT_X:
		case KEY_ALT_Y:
		case KEY_ALT_Z:
			len = 0;
			break;

		case KEY_ESC:
			key[0] = 27;
			len = 1;
			break;

		case KEY_TAB:
			key[0] = 9;
			len = 1;
			break;

		case KEY_SPACE:
			key[0] = ' ';
			len = 1;
			break;

		case KEY_ENTER:
			key[0] = '\r';
			len = 1;
			break;

		case KEY_BACKSPACE:
			key[0] = 8;
			len = 1;
			break;

		case KEY_UP:
			key[0] = 27;
			key[1] = '[';
			key[2] = 'A';
			len = 3;
			break;

		case KEY_DOWN:
			key[0] = 27;
			key[1] = '[';
			key[2] = 'B';
			len = 3;
			break;

		case KEY_LEFT:
			key[0] = 27;
			key[1] = '[';
			key[2] = 'D';
			len = 3;
			break;

		case KEY_RIGHT:
			key[0] = 27;
			key[1] = '[';
			key[2] = 'C';
			len = 3;
			break;

		case KEY_HOME:
		case KEY_END:
		case KEY_PAGEUP:
		case KEY_PAGEDOWN:
		case KEY_INSERT:
		case KEY_DELETE:
			len = 0;
			break;

		case KEY_STANDBY:
			standby();
			len = 0;
			break;

		case KEY_RESUME:
			resume();
			len = 0;
			break;

		case KEY_HALT:
			halt();
			len = 0;
			break;

		case KEY_RESET:
			reset();
			len = 0;
			break;

		default:
			len = 0;
			break;
		}
		fifo_put(info->fifo, key, len);
	}

	return fifo_get(info->fifo, buf, count);
}

/*
 * keyboard close
 */
static x_s32 keyboard_close(struct chrdev * dev)
{
	return 0;
}

/*
 * register keyboard driver, return true is successed.
 */
x_bool register_keyboard(struct keyboard_driver * drv)
{
	struct chrdev * dev;
	struct terminal * term;
	struct keyboard_terminal_info * info;

	if(!drv || !drv->name || !drv->read)
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= drv->name;
	dev->type		= CHR_DEV_KEYBOARD;
	dev->open 		= keyboard_open;
	dev->seek 		= keyboard_seek;
	dev->read 		= keyboard_read;
	dev->write 		= keyboard_write;
	dev->flush 		= keyboard_flush;
	dev->ioctl 		= keyboard_ioctl;
	dev->close		= keyboard_close;
	dev->driver 	= drv;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_KEYBOARD) == NULL)
	{
		unregister_chrdev(dev->name);
		free(dev);
		return FALSE;
	}

	if(drv->init)
		(drv->init)();

	if(drv->flush)
		(drv->flush)();

	/*
	 * register a terminal
	 */
	term = malloc(sizeof(struct terminal));
	info = malloc(sizeof(struct keyboard_terminal_info));
	if(!term || !info)
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(term);
		free(info);
		return FALSE;
	}

	strcpy((x_s8*)info->name, (x_s8*)"tty-");
	strlcat((x_s8*)info->name, (x_s8*)drv->name, 32+1);
	info->drv = drv;
	info->fifo = fifo_alloc(128);

	term->name = info->name;
	term->priv = info;
	term->type = TERMINAL_KEYBOARD;
	term->getwh = NULL;
	term->setxy = NULL;
	term->getxy = NULL;
	term->cursor_left = NULL;
	term->cursor_right = NULL;
	term->cursor_up = NULL;
	term->cursor_down = NULL;
	term->cursor_home = NULL;
	term->cursor_save = NULL;
	term->cursor_restore = NULL;
	term->cursor_hide = NULL;
	term->cursor_show = NULL;
	term->cursor_flash = NULL;
	term->mode_highlight = NULL;
	term->mode_underline = NULL;
	term->mode_reverse = NULL;
	term->mode_blanking = NULL;
	term->mode_closeall = NULL;
	term->set_color = NULL;
	term->get_color = NULL;
	term->clear_screen = NULL;
	term->read = keyboard_term_read;
	term->write = NULL;

	if(!register_terminal(term))
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(term);
		free(info);
		return FALSE;
	}

	return TRUE;
}

/*
 * unregister keyboard driver
 */
x_bool unregister_keyboard(struct keyboard_driver * drv)
{
	struct chrdev * dev;
	struct keyboard_driver * driver;
	x_s8 term_name[32+1];
	struct terminal * term;
	struct keyboard_terminal_info * info;

	if(!drv || !drv->name)
		return FALSE;

	dev = search_chrdev_with_type(drv->name, CHR_DEV_KEYBOARD);
	if(!dev)
		return FALSE;

	strcpy(term_name, (x_s8*)"tty-");
	strlcat(term_name, (x_s8*)drv->name, 32+1);

	term = search_terminal((char *)term_name);
	if(term)
		info = (struct keyboard_terminal_info *)term->priv;
	else
		return FALSE;

	driver = (struct keyboard_driver *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)();

	if(!unregister_terminal(term))
		return FALSE;

	if(!unregister_chrdev(dev->name))
		return FALSE;

	fifo_free(info->fifo);
	free(info);
	free(term);
	free(dev);

	return TRUE;
}
