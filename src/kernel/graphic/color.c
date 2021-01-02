/*
 * kernel/graphic/color.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <shash.h>
#include <graphic/color.h>

static inline unsigned char hex_to_bin(char c)
{
	if((c >= '0') && (c <= '9'))
		return c - '0';
	else if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	else if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	return 0;
}

static inline unsigned char hex_color(const char * s, int l, int r)
{
	int len = r - l;

	if(len == 1)
		return (hex_to_bin(s[l]) << 4) | hex_to_bin(s[l]);
	else if(len == 2)
		return (hex_to_bin(s[l]) << 4) | hex_to_bin(s[l + 1]);
	return 0;
}

void color_init_string(struct color_t * c, const char * s)
{
	const char * e;
	int l;

	if(c)
	{
		while(*s && isspace(*s))
			s++;
		if(*s == '#')
		{
			s++;
			l = 0;
			e = s;
			while(isxdigit(*e))
			{
				e++;
				l++;
			}
			if(l == 3 || l == 4)
			{
				c->r = hex_color(s, 0, 1);
				c->g = hex_color(s, 1, 2);
				c->b = hex_color(s, 2, 3);
				if(l == 4)
					c->a = hex_color(s, 3, 4);
				else
					c->a = 0xff;
			}
			else if(l == 6 || l == 8)
			{
				c->r = hex_color(s, 0, 2);
				c->g = hex_color(s, 2, 4);
				c->b = hex_color(s, 4, 6);
				if(l == 8)
					c->a = hex_color(s, 6, 8);
				else
					c->a = 0xff;
			}
			else
			{
				c->r = 0xff;
				c->g = 0xff;
				c->b = 0xff;
				c->a = 0xff;
			}
		}
		else if(isdigit(*s))
		{
			c->r = strtoul(s, 0, 0);
			s = strchr(s, ',');
			if(!s)
			{
				c->g = 0xff;
				c->b = 0xff;
				c->a = 0xff;
				return;
			}
			c->g = strtoul(++s, 0, 0);
			s = strchr(s, ',');
			if(!s)
			{
				c->b = 0xff;
				c->a = 0xff;
				return;
			}
			c->b = strtoul(++s, 0, 0);
			s = strchr(s, ',');
			if(!s)
				c->a = 0xff;
			else
				c->a = strtoul(++s, 0, 0);
		}
		else
		{
			switch(shash(s))
			{
			case 0x41f60f4b: /* "aliceblue" */
				c->r = 0xf0; c->g = 0xf8; c->b = 0xff;
				break;
			case 0x3a2d20fd: /* "antiquewhite" */
				c->r = 0xfa; c->g = 0xeb; c->b = 0xd7;
				break;
			case 0x7c94306d: /* "aqua" */
				c->r = 0x00; c->g = 0xff; c->b = 0xff;
				break;
			case 0x52e1f409: /* "aquamarine" */
				c->r = 0x7f; c->g = 0xff; c->b = 0xd4;
				break;
			case 0x0f1f300c: /* "azure" */
				c->r = 0xf0; c->g = 0xff; c->b = 0xff;
				break;
			case 0x0f259021: /* "beige" */
				c->r = 0xf5; c->g = 0xf5; c->b = 0xdc;
				break;
			case 0xf4259f0e: /* "bisque" */
				c->r = 0xff; c->g = 0xe4; c->b = 0xc4;
				break;
			case 0x0f294442: /* "black" */
				c->r = 0x00; c->g = 0x00; c->b = 0x00;
				break;
			case 0x25a17751: /* "blanchedalmond" */
				c->r = 0xff; c->g = 0xeb; c->b = 0xcd;
				break;
			case 0x7c94a78d: /* "blue" */
				c->r = 0x00; c->g = 0x00; c->b = 0xff;
				break;
			case 0x59f4db60: /* "blueviolet" */
				c->r = 0x8a; c->g = 0x2b; c->b = 0xe2;
				break;
			case 0x0f2cccad: /* "brown" */
				c->r = 0xa5; c->g = 0x2a; c->b = 0x2a;
				break;
			case 0xd00306ac: /* "burlywood" */
				c->r = 0xde; c->g = 0xb8; c->b = 0x87;
				break;
			case 0x88f15cae: /* "cadetblue" */
				c->r = 0x5f; c->g = 0x9e; c->b = 0xa0;
				break;
			case 0xfb91543b: /* "chartreuse" */
				c->r = 0x7f; c->g = 0xff; c->b = 0x00;
				break;
			case 0x487f4c37: /* "chocolate" */
				c->r = 0xd2; c->g = 0x69; c->b = 0x1e;
				break;
			case 0x0f3d49f6: /* "coral" */
				c->r = 0xff; c->g = 0x7f; c->b = 0x50;
				break;
			case 0x68196cee: /* "cornflowerblue" */
				c->r = 0x64; c->g = 0x95; c->b = 0xed;
				break;
			case 0x4b9c706a: /* "cornsilk" */
				c->r = 0xff; c->g = 0xf8; c->b = 0xdc;
				break;
			case 0xda1afde0: /* "crimson" */
				c->r = 0xdc; c->g = 0x14; c->b = 0x3c;
				break;
			case 0x7c9568b0: /* "cyan" */
				c->r = 0x00; c->g = 0xff; c->b = 0xff;
				break;
			case 0x01ef64af: /* "darkblue" */
				c->r = 0x00; c->g = 0x00; c->b = 0x8b;
				break;
			case 0x01f025d2: /* "darkcyan" */
				c->r = 0x00; c->g = 0x8b; c->b = 0x8b;
				break;
			case 0xc6d90285: /* "darkgoldenrod" */
				c->r = 0xb8; c->g = 0x86; c->b = 0x0b;
				break;
			case 0x01f2399a: /* "darkgray" */
				c->r = 0xa9; c->g = 0xa9; c->b = 0xa9;
				break;
			case 0x40397bb8: /* "darkgreen" */
				c->r = 0x00; c->g = 0x64; c->b = 0x00;
				break;
			case 0x01f23a1e: /* "darkgrey" */
				c->r = 0xa9; c->g = 0xa9; c->b = 0xa9;
				break;
			case 0x407c51af: /* "darkkhaki" */
				c->r = 0xbd; c->g = 0xb7; c->b = 0x6b;
				break;
			case 0xdae143e4: /* "darkmagenta" */
				c->r = 0x8b; c->g = 0x00; c->b = 0x8b;
				break;
			case 0x092c7a97: /* "darkolivegreen" */
				c->r = 0x55; c->g = 0x6b; c->b = 0x2f;
				break;
			case 0x5a102c03: /* "darkorange" */
				c->r = 0xff; c->g = 0x8c; c->b = 0x00;
				break;
			case 0x5a112b80: /* "darkorchid" */
				c->r = 0x99; c->g = 0x32; c->b = 0xcc;
				break;
			case 0x000f4622: /* "darkred" */
				c->r = 0x8b; c->g = 0x00; c->b = 0x00;
				break;
			case 0x623732f1: /* "darksalmon" */
				c->r = 0xe9; c->g = 0x96; c->b = 0x7a;
				break;
			case 0xe6a4e091: /* "darkseagreen" */
				c->r = 0x8f; c->g = 0xbc; c->b = 0x8f;
				break;
			case 0x4e741068: /* "darkslateblue" */
				c->r = 0x48; c->g = 0x3d; c->b = 0x8b;
				break;
			case 0x4e76e553: /* "darkslategray" */
				c->r = 0x2f; c->g = 0x4f; c->b = 0x4f;
				break;
			case 0x4e76e5d7: /* "darkslategrey" */
				c->r = 0x2f; c->g = 0x4f; c->b = 0x4f;
				break;
			case 0x28082838: /* "darkturquoise" */
				c->r = 0x00; c->g = 0xce; c->b = 0xd1;
				break;
			case 0x69c9107a: /* "darkviolet" */
				c->r = 0x94; c->g = 0x00; c->b = 0xd3;
				break;
			case 0x17e761b5: /* "deeppink" */
				c->r = 0xff; c->g = 0x14; c->b = 0x93;
				break;
			case 0x84780222: /* "deepskyblue" */
				c->r = 0x00; c->g = 0xbf; c->b = 0xff;
				break;
			case 0x125bdeb2: /* "dimgray" */
				c->r = 0x69; c->g = 0x69; c->b = 0x69;
				break;
			case 0x125bdf36: /* "dimgrey" */
				c->r = 0x69; c->g = 0x69; c->b = 0x69;
				break;
			case 0x8208b222: /* "dodgerblue" */
				c->r = 0x1e; c->g = 0x90; c->b = 0xff;
				break;
			case 0x7ce7a736: /* "firebrick" */
				c->r = 0xb2; c->g = 0x22; c->b = 0x22;
				break;
			case 0xa97767c6: /* "floralwhite" */
				c->r = 0xff; c->g = 0xfa; c->b = 0xf0;
				break;
			case 0x8eda0a29: /* "forestgreen" */
				c->r = 0x22; c->g = 0x8b; c->b = 0x22;
				break;
			case 0xc799dc48: /* "fuchsia" */
				c->r = 0xff; c->g = 0x00; c->b = 0xff;
				break;
			case 0xf0b2b209: /* "gainsboro" */
				c->r = 0xdc; c->g = 0xdc; c->b = 0xdc;
				break;
			case 0x44ab668b: /* "ghostwhite" */
				c->r = 0xf8; c->g = 0xf8; c->b = 0xff;
				break;
			case 0x7c97710b: /* "gold" */
				c->r = 0xff; c->g = 0xd7; c->b = 0x00;
				break;
			case 0xaaf0c023: /* "goldenrod" */
				c->r = 0xda; c->g = 0xa5; c->b = 0x20;
				break;
			case 0x7c977c78: /* "gray" */
				c->r = 0x80; c->g = 0x80; c->b = 0x80;
				break;
			case 0x0f871a56: /* "green" */
				c->r = 0x00; c->g = 0x80; c->b = 0x00;
				break;
			case 0xc0a3f4f2: /* "greenyellow" */
				c->r = 0xad; c->g = 0xff; c->b = 0x2f;
				break;
			case 0x7c977cfc: /* "grey" */
				c->r = 0x80; c->g = 0x80; c->b = 0x80;
				break;
			case 0xdef14de8: /* "honeydew" */
				c->r = 0xf0; c->g = 0xff; c->b = 0xf0;
				break;
			case 0x54c73bc2: /* "hotpink" */
				c->r = 0xff; c->g = 0x69; c->b = 0xb4;
				break;
			case 0x4b374f13: /* "indianred" */
				c->r = 0xcd; c->g = 0x5c; c->b = 0x5c;
				break;
			case 0x04cbd87f: /* "indigo" */
				c->r = 0x4b; c->g = 0x00; c->b = 0x82;
				break;
			case 0x0fada91e: /* "ivory" */
				c->r = 0xff; c->g = 0xff; c->b = 0xf0;
				break;
			case 0x0fc9f04d: /* "khaki" */
				c->r = 0xf0; c->g = 0xe6; c->b = 0x8c;
				break;
			case 0x6cec8bb6: /* "lavender" */
				c->r = 0xe6; c->g = 0xe6; c->b = 0xfa;
				break;
			case 0x4d30e8b4: /* "lavenderblush" */
				c->r = 0xff; c->g = 0xf0; c->b = 0xf5;
				break;
			case 0x6bffad68: /* "lawngreen" */
				c->r = 0x7c; c->g = 0xfc; c->b = 0x00;
				break;
			case 0x1aa3ab7d: /* "lemonchiffon" */
				c->r = 0xff; c->g = 0xfa; c->b = 0xcd;
				break;
			case 0xf14e2ce5: /* "lightblue" */
				c->r = 0xad; c->g = 0xd8; c->b = 0xe6;
				break;
			case 0x1b277a4e: /* "lightcoral" */
				c->r = 0xf0; c->g = 0x80; c->b = 0x80;
				break;
			case 0xf14eee08: /* "lightcyan" */
				c->r = 0xe0; c->g = 0xff; c->b = 0xff;
				break;
			case 0x269c7ed7: /* "lightgoldenrodyellow" */
				c->r = 0xfa; c->g = 0xfa; c->b = 0xd2;
				break;
			case 0xf15101d0: /* "lightgray" */
				c->r = 0xd3; c->g = 0xd3; c->b = 0xd3;
				break;
			case 0x1b714aae: /* "lightgreen" */
				c->r = 0x90; c->g = 0xee; c->b = 0x90;
				break;
			case 0xf1510254: /* "lightgrey" */
				c->r = 0xd3; c->g = 0xd3; c->b = 0xd3;
				break;
			case 0xf155cc8f: /* "lightpink" */
				c->r = 0xff; c->g = 0xb6; c->b = 0xc1;
				break;
			case 0xa468e0a7: /* "lightsalmon" */
				c->r = 0xff; c->g = 0xa0; c->b = 0x7a;
				break;
			case 0x7bf8d3c7: /* "lightseagreen" */
				c->r = 0x20; c->g = 0xb2; c->b = 0xaa;
				break;
			case 0x49bdb6bc: /* "lightskyblue" */
				c->r = 0x87; c->g = 0xce; c->b = 0xfa;
				break;
			case 0x8e493f49: /* "lightslategray" */
				c->r = 0x77; c->g = 0x88; c->b = 0x99;
				break;
			case 0x8e493fcd: /* "lightslategrey" */
				c->r = 0x77; c->g = 0x88; c->b = 0x99;
				break;
			case 0x01bf4e82: /* "lightsteelblue" */
				c->r = 0xb0; c->g = 0xc4; c->b = 0xde;
				break;
			case 0xb2b03239: /* "lightyellow" */
				c->r = 0xff; c->g = 0xff; c->b = 0xe0;
				break;
			case 0x7c9a158c: /* "lime" */
				c->r = 0x00; c->g = 0xff; c->b = 0x00;
				break;
			case 0xb749873d: /* "limegreen" */
				c->r = 0x32; c->g = 0xcd; c->b = 0x32;
				break;
			case 0x0fdccbbb: /* "linen" */
				c->r = 0xfa; c->g = 0xf0; c->b = 0xe6;
				break;
			case 0xb4110202: /* "magenta" */
				c->r = 0xff; c->g = 0x00; c->b = 0xff;
				break;
			case 0x0d3d0451: /* "maroon" */
				c->r = 0x80; c->g = 0x00; c->b = 0x00;
				break;
			case 0x5393448a: /* "mediumaquamarine" */
				c->r = 0x66; c->g = 0xcd; c->b = 0xaa;
				break;
			case 0xd877eb4e: /* "mediumblue" */
				c->r = 0x00; c->g = 0x00; c->b = 0xcd;
				break;
			case 0xf4d5d5df: /* "mediumorchid" */
				c->r = 0xba; c->g = 0x55; c->b = 0xd3;
				break;
			case 0xf769a41e: /* "mediumpurple" */
				c->r = 0x93; c->g = 0x70; c->b = 0xdb;
				break;
			case 0x453d9eb0: /* "mediumseagreen" */
				c->r = 0x3c; c->g = 0xb3; c->b = 0x71;
				break;
			case 0x80249267: /* "mediumslateblue" */
				c->r = 0x7b; c->g = 0x68; c->b = 0xee;
				break;
			case 0x814643ca: /* "mediumspringgreen" */
				c->r = 0x00; c->g = 0xfa; c->b = 0x9a;
				break;
			case 0x59b8aa37: /* "mediumturquoise" */
				c->r = 0x48; c->g = 0xd1; c->b = 0xcc;
				break;
			case 0x3be46a94: /* "mediumvioletred" */
				c->r = 0xc7; c->g = 0x15; c->b = 0x85;
				break;
			case 0x5f9313a1: /* "midnightblue" */
				c->r = 0x19; c->g = 0x19; c->b = 0x70;
				break;
			case 0x9b7533e5: /* "mintcream" */
				c->r = 0xf5; c->g = 0xff; c->b = 0xfa;
				break;
			case 0x1de6ab94: /* "mistyrose" */
				c->r = 0xff; c->g = 0xe4; c->b = 0xe1;
				break;
			case 0x62609d92: /* "moccasin" */
				c->r = 0xff; c->g = 0xe4; c->b = 0xb5;
				break;
			case 0xe2bc6625: /* "navajowhite" */
				c->r = 0xff; c->g = 0xde; c->b = 0xad;
				break;
			case 0x7c9b0d83: /* "navy" */
				c->r = 0x00; c->g = 0x00; c->b = 0x80;
				break;
			case 0x677b8e19: /* "oldlace" */
				c->r = 0xfd; c->g = 0xf5; c->b = 0xe6;
				break;
			case 0x1014a744: /* "olive" */
				c->r = 0x80; c->g = 0x80; c->b = 0x00;
				break;
			case 0xcd1770fd: /* "olivedrab" */
				c->r = 0x6b; c->g = 0x8e; c->b = 0x23;
				break;
			case 0x13119e61: /* "orange" */
				c->r = 0xff; c->g = 0xa5; c->b = 0x00;
				break;
			case 0xdc4c011c: /* "orangered" */
				c->r = 0xff; c->g = 0x45; c->b = 0x00;
				break;
			case 0x13129dde: /* "orchid" */
				c->r = 0xda; c->g = 0x70; c->b = 0xd6;
				break;
			case 0x46e1ce45: /* "palegoldenrod" */
				c->r = 0xee; c->g = 0xe8; c->b = 0xaa;
				break;
			case 0xda326778: /* "palegreen" */
				c->r = 0x98; c->g = 0xfb; c->b = 0x98;
				break;
			case 0xa810f3f8: /* "paleturquoise" */
				c->r = 0xaf; c->g = 0xee; c->b = 0xee;
				break;
			case 0x8a3cb455: /* "palevioletred" */
				c->r = 0xdb; c->g = 0x70; c->b = 0x93;
				break;
			case 0xc670dd19: /* "papayawhip" */
				c->r = 0xff; c->g = 0xef; c->b = 0xd5;
				break;
			case 0x37e01157: /* "peachpuff" */
				c->r = 0xff; c->g = 0xda; c->b = 0xb9;
				break;
			case 0x7c9c36c1: /* "peru" */
				c->r = 0xcd; c->g = 0x85; c->b = 0x3f;
				break;
			case 0x7c9c4737: /* "pink" */
				c->r = 0xff; c->g = 0xc0; c->b = 0xcb;
				break;
			case 0x7c9c54e3: /* "plum" */
				c->r = 0xdd; c->g = 0xa0; c->b = 0xdd;
				break;
			case 0x547b961e: /* "powderblue" */
				c->r = 0xb0; c->g = 0xe0; c->b = 0xe6;
				break;
			case 0x15a66c1d: /* "purple" */
				c->r = 0x80; c->g = 0x00; c->b = 0x80;
				break;
			case 0x0b88a540: /* "red" */
				c->r = 0xff; c->g = 0x00; c->b = 0x00;
				break;
			case 0xf7e975fa: /* "rosybrown" */
				c->r = 0xbc; c->g = 0x8f; c->b = 0x8f;
				break;
			case 0x8e773554: /* "royalblue" */
				c->r = 0x41; c->g = 0x69; c->b = 0xe1;
				break;
			case 0x92bbf35a: /* "saddlebrown" */
				c->r = 0x8b; c->g = 0x45; c->b = 0x13;
				break;
			case 0x1b38a54f: /* "salmon" */
				c->r = 0xfa; c->g = 0x80; c->b = 0x72;
				break;
			case 0xe10b172c: /* "sandybrown" */
				c->r = 0xf4; c->g = 0xa4; c->b = 0x60;
				break;
			case 0xe5cc626f: /* "seagreen" */
				c->r = 0x2e; c->g = 0x8b; c->b = 0x57;
				break;
			case 0xe6a00d96: /* "seashell" */
				c->r = 0xff; c->g = 0xf5; c->b = 0xee;
				break;
			case 0x1bc596c3: /* "sienna" */
				c->r = 0xa0; c->g = 0x52; c->b = 0x2d;
				break;
			case 0x1bc98e5a: /* "silver" */
				c->r = 0xc0; c->g = 0xc0; c->b = 0xc0;
				break;
			case 0x9a861064: /* "skyblue" */
				c->r = 0x87; c->g = 0xce; c->b = 0xeb;
				break;
			case 0x328bce06: /* "slateblue" */
				c->r = 0x6a; c->g = 0x5a; c->b = 0xcd;
				break;
			case 0x328ea2f1: /* "slategray" */
				c->r = 0x70; c->g = 0x80; c->b = 0x90;
				break;
			case 0x328ea375: /* "slategrey" */
				c->r = 0x70; c->g = 0x80; c->b = 0x90;
				break;
			case 0x7c9e01cc: /* "snow" */
				c->r = 0xff; c->g = 0xfa; c->b = 0xfa;
				break;
			case 0x6a6ae329: /* "springgreen" */
				c->r = 0x00; c->g = 0xff; c->b = 0x7f;
				break;
			case 0xa604b22a: /* "steelblue" */
				c->r = 0x46; c->g = 0x82; c->b = 0xb4;
				break;
			case 0x0b88ad48: /* "tan" */
				c->r = 0xd2; c->g = 0xb4; c->b = 0x8c;
				break;
			case 0x7c9e660b: /* "teal" */
				c->r = 0x00; c->g = 0x80; c->b = 0x80;
				break;
			case 0xdf68be82: /* "thistle" */
				c->r = 0xd8; c->g = 0xbf; c->b = 0xd8;
				break;
			case 0x1e8b7ef9: /* "tomato" */
				c->r = 0xff; c->g = 0x63; c->b = 0x47;
				break;
			case 0x0c1fe5d6: /* "turquoise" */
				c->r = 0x40; c->g = 0xe0; c->b = 0xd0;
				break;
			case 0x22ca82d8: /* "violet" */
				c->r = 0xee; c->g = 0x82; c->b = 0xee;
				break;
			case 0x10a3261e: /* "wheat" */
				c->r = 0xf5; c->g = 0xde; c->b = 0xb3;
				break;
			case 0x10a33986: /* "white" */
				c->r = 0xff; c->g = 0xff; c->b = 0xff;
				break;
			case 0x2580cae5: /* "whitesmoke" */
				c->r = 0xf5; c->g = 0xf5; c->b = 0xf5;
				break;
			case 0x297ff6e1: /* "yellow" */
				c->r = 0xff; c->g = 0xff; c->b = 0x00;
				break;
			case 0xda4a85b2: /* "yellowgreen" */
				c->r = 0x9a; c->g = 0xcd; c->b = 0x32;
				break;
			default:
				c->r = 0xff; c->g = 0xff; c->b = 0xff;
				break;
			}
			c->a = 0xff;
		}
	}
}

void color_set_hsva(struct color_t * c, float h, float s, float v, float a)
{
	float r, g, b;
	float p, q, t, f;
	int i;

	if(s <= 0.0f)
	{
		c->r = c->g = c->b = roundf(v * 255.0f);
		c->a = roundf(a * 255.0f);
	}
	else
	{
		h = h / (60.0f / 360.0f);
		i = (int)h;
		f = h - (float)i;
		p = v * (1.0f - s);
		q = v * (1.0f - (s * f));
		t = v * (1.0f - s * (1.0f - f));
		switch(i)
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
			r = v;
			g = p;
			b = q;
			break;
		default:
			r = v;
			g = t;
			b = p;
			break;
		}
		c->r = roundf(r * 255.0f);
		c->g = roundf(g * 255.0f);
		c->b = roundf(b * 255.0f);
		c->a = roundf(a * 255.0f);
	}
}

void color_get_hsva(struct color_t * c, float * h, float * s, float * v, float * a)
{
	float r = c->r / 255.0f;
	float g = c->g / 255.0f;
	float b = c->b / 255.0f;
	float k = 0.0f;
	float chroma;
	float t;

	if(g < b)
	{
		t = g;
		g = b;
		b = t;
		k = -1.f;
	}
	if(r < g)
	{
		t = r;
		r = g;
		g = t;
		k = -2.f / 6.0f - k;
	}
	chroma = r - ((g < b) ? g : b);
	*h = fabsf(k + (g - b) / (6.0f * chroma + 1e-20f));
	*s = chroma / (r + 1e-20f);
	*v = r;
	if(a)
		*a = c->a / 255.0f;
}

void color_random(struct color_t * c, float s, float v, float a)
{
	float h = (float)rand() * (1.0f / ((float)RAND_MAX + 1.0f)) + 0.618f;
	color_set_hsva(c, fmodf(h, 1.0f), s, v, a);
}
