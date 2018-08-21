/*
 * driver/vibrator/vibrator.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <vibrator/vibrator.h>

static ssize_t vibrator_read_state(struct kobj_t * kobj, void * buf, size_t size)
{
	struct vibrator_t * vib = (struct vibrator_t *)kobj->priv;
	return sprintf(buf, "%d", vibrator_get_state(vib));
}

static ssize_t vibrator_write_state(struct kobj_t * kobj, void * buf, size_t size)
{
	struct vibrator_t * vib = (struct vibrator_t *)kobj->priv;
	vibrator_set_state(vib, strtol(buf, NULL, 0));
	return size;
}

static ssize_t vibrator_write_play(struct kobj_t * kobj, void * buf, size_t size)
{
	struct vibrator_t * vib = (struct vibrator_t *)kobj->priv;
	vibrator_play(vib, buf);
	return size;
}

struct vibrator_t * search_vibrator(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_VIBRATOR);
	if(!dev)
		return NULL;
	return (struct vibrator_t *)dev->priv;
}

struct vibrator_t * search_first_vibrator(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_VIBRATOR);
	if(!dev)
		return NULL;
	return (struct vibrator_t *)dev->priv;
}

bool_t register_vibrator(struct device_t ** device, struct vibrator_t * vib)
{
	struct device_t * dev;

	if(!vib || !vib->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(vib->name);
	dev->type = DEVICE_TYPE_VIBRATOR;
	dev->priv = vib;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "state", vibrator_read_state, vibrator_write_state, vib);
	kobj_add_regular(dev->kobj, "play", NULL, vibrator_write_play, vib);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_vibrator(struct vibrator_t * vib)
{
	struct device_t * dev;

	if(!vib || !vib->name)
		return FALSE;

	dev = search_device(vib->name, DEVICE_TYPE_VIBRATOR);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void vibrator_set_state(struct vibrator_t * vib, int state)
{
	if(vib && vib->set)
		vib->set(vib, (state > 0) ? 1 : 0);
}

int vibrator_get_state(struct vibrator_t * vib)
{
	if(vib && vib->get)
		return vib->get(vib);
	return 0;
}

void vibrator_vibrate(struct vibrator_t * vib, int state, int millisecond)
{
	if(vib && vib->vibrate)
		vib->vibrate(vib, (state > 0) ? 1 : 0, (millisecond > 0) ? millisecond : 0);
}

static const char * morse_code(char c)
{
	switch(c)
	{
	case '0':
		return "-----";
	case '1':
		return ".----";
	case '2':
		return "..---";
	case '3':
		return "...--";
	case '4':
		return "....-";
	case '5':
		return ".....";
	case '6':
		return "-....";
	case '7':
		return "--...";
	case '8':
		return "---..";
	case '9':
		return "----.";

	case 'a':
	case 'A':
		return ".-";
	case 'b':
	case 'B':
		return "-...";
	case 'c':
	case 'C':
		return "-.-.";
	case 'd':
	case 'D':
		return "-..";
	case 'e':
	case 'E':
		return ".";
	case 'f':
	case 'F':
		return "..-.";
	case 'g':
	case 'G':
		return "--.";
	case 'h':
	case 'H':
		return "....";
	case 'i':
	case 'I':
		return "..";
	case 'j':
	case 'J':
		return ".---";
	case 'k':
	case 'K':
		return "-.-";
	case 'l':
	case 'L':
		return ".-..";
	case 'm':
	case 'M':
		return "--";
	case 'n':
	case 'N':
		return "-.";
	case 'o':
	case 'O':
		return "---";
	case 'p':
	case 'P':
		return ".--.";
	case 'q':
	case 'Q':
		return "--.-";
	case 'r':
	case 'R':
		return ".-.";
	case 's':
	case 'S':
		return "...";
	case 't':
	case 'T':
		return "-";
	case 'u':
	case 'U':
		return "..-";
	case 'v':
	case 'V':
		return "...-";
	case 'w':
	case 'W':
		return ".--";
	case 'x':
	case 'X':
		return "-..-";
	case 'y':
	case 'Y':
		return "-.--";
	case 'z':
	case 'Z':
		return "--..";

	case '.':
		return ".-.-.-";
	case ',':
		return "--..--";
	case '?':
		return "..--..";
	case '\'':
		return ".----.";
	case '!':
		return "-.-.--";
	case '/':
		return "-..-.";
	case '(':
		return "-.--.-";
	case ')':
		return "-.--.-";
	case '&':
		return ".-...";
	case ':':
		return "---...";
	case ';':
		return "-.-.-.";
	case '=':
		return "-...-";
	case '+':
		return ".-.-.";
	case '-':
		return "-....-";
	case '_':
		return "..--.-";
	case '\"':
		return ".-..-.";
	case '$':
		return "...-..-";
	case '@':
		return ".--.-.";

	default:
		break;
	}
	return "";
}

void vibrator_play(struct vibrator_t * vib, const char * morse)
{
	char * p = (char *)morse;
	char * q;

	vibrator_vibrate(vib, 0, 0);
	if(!p)
		return;

	while(*p)
	{
		if(isspace(*p))
		{
			vibrator_vibrate(vib, 0, 100 * 7);
			continue;
		}

		q = (char *)morse_code(*p);
		while(*q)
		{
			if(*q == '.')
				vibrator_vibrate(vib, 1, 100 * 1);
			else if(*q == '-')
				vibrator_vibrate(vib, 1, 100 * 3);

			vibrator_vibrate(vib, 0, 100);
			q++;
		}

		vibrator_vibrate(vib, 0, 100 * 3);
		p++;
	}
}
