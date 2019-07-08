/*
 * kernel/graphic/color.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <stdlib.h>
#include <string.h>
#include <shash.h>
#include <graphic/color.h>

static inline int hex_to_bin(char c)
{
	if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if((c >= '0') && (c <= '9'))
		return c - '0';
	if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

static inline double hex_color(const char * s, int l, int r)
{
	int len = r - l;

	if(len == 1)
		return (double)((hex_to_bin(s[l]) << 4) | hex_to_bin(s[l])) / 255.0;
	else if(len == 2)
		return (double)((hex_to_bin(s[l]) << 4) | hex_to_bin(s[l + 1])) / 255.0;
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
					c->a = 1.0;
			}
			else if(l == 6 || l == 8)
			{
				c->r = hex_color(s, 0, 2);
				c->g = hex_color(s, 2, 4);
				c->b = hex_color(s, 4, 6);
				if(l == 8)
					c->a = hex_color(s, 6, 8);
				else
					c->a = 1.0;
			}
			else
			{
				c->r = 1.0;
				c->g = 1.0;
				c->b = 1.0;
				c->a = 1.0;
			}
		}
		else if(isdigit(*s))
		{
			c->r = (double)strtoul(s, 0, 0) / 255.0;
			s = strchr(s, ',');
			if(!s)
			{
				c->g = 1.0;
				c->b = 1.0;
				c->a = 1.0;
				return;
			}
			c->g = (double)strtoul(++s, 0, 0) / 255.0;
			s = strchr(s, ',');
			if(!s)
			{
				c->b = 1.0;
				c->a = 1.0;
				return;
			}
			c->b = (double)strtoul(++s, 0, 0) / 255.0;
			s = strchr(s, ',');
			if(!s)
				c->a = 1.0;
			else
				c->a = (double)strtoul(++s, 0, 0) / 255.0;
		}
		else
		{
			switch(shash(s))
			{
			case 0x41f60f4b: /* "aliceblue" */
				c->r = 0.941176; c->g = 0.972549; c->b = 1.000000;
				break;
			case 0x3a2d20fd: /* "antiquewhite" */
				c->r = 0.980392; c->g = 0.921569; c->b = 0.843137;
				break;
			case 0x7c94306d: /* "aqua" */
				c->r = 0.000000; c->g = 1.000000; c->b = 1.000000;
				break;
			case 0x52e1f409: /* "aquamarine" */
				c->r = 0.498039; c->g = 1.000000; c->b = 0.831373;
				break;
			case 0x0f1f300c: /* "azure" */
				c->r = 0.941176; c->g = 1.000000; c->b = 1.000000;
				break;
			case 0x0f259021: /* "beige" */
				c->r = 0.960784; c->g = 0.960784; c->b = 0.862745;
				break;
			case 0xf4259f0e: /* "bisque" */
				c->r = 1.000000; c->g = 0.894118; c->b = 0.768627;
				break;
			case 0x0f294442: /* "black" */
				c->r = 0.000000; c->g = 0.000000; c->b = 0.000000;
				break;
			case 0x25a17751: /* "blanchedalmond" */
				c->r = 1.000000; c->g = 0.921569; c->b = 0.803922;
				break;
			case 0x7c94a78d: /* "blue" */
				c->r = 0.000000; c->g = 0.000000; c->b = 1.000000;
				break;
			case 0x59f4db60: /* "blueviolet" */
				c->r = 0.541176; c->g = 0.168627; c->b = 0.886275;
				break;
			case 0x0f2cccad: /* "brown" */
				c->r = 0.647059; c->g = 0.164706; c->b = 0.164706;
				break;
			case 0xd00306ac: /* "burlywood" */
				c->r = 0.870588; c->g = 0.721569; c->b = 0.529412;
				break;
			case 0x88f15cae: /* "cadetblue" */
				c->r = 0.372549; c->g = 0.619608; c->b = 0.627451;
				break;
			case 0xfb91543b: /* "chartreuse" */
				c->r = 0.498039; c->g = 1.000000; c->b = 0.000000;
				break;
			case 0x487f4c37: /* "chocolate" */
				c->r = 0.823529; c->g = 0.411765; c->b = 0.117647;
				break;
			case 0x0f3d49f6: /* "coral" */
				c->r = 1.000000; c->g = 0.498039; c->b = 0.313725;
				break;
			case 0x68196cee: /* "cornflowerblue" */
				c->r = 0.392157; c->g = 0.584314; c->b = 0.929412;
				break;
			case 0x4b9c706a: /* "cornsilk" */
				c->r = 1.000000; c->g = 0.972549; c->b = 0.862745;
				break;
			case 0xda1afde0: /* "crimson" */
				c->r = 0.862745; c->g = 0.078431; c->b = 0.235294;
				break;
			case 0x7c9568b0: /* "cyan" */
				c->r = 0.000000; c->g = 1.000000; c->b = 1.000000;
				break;
			case 0x01ef64af: /* "darkblue" */
				c->r = 0.000000; c->g = 0.000000; c->b = 0.545098;
				break;
			case 0x01f025d2: /* "darkcyan" */
				c->r = 0.000000; c->g = 0.545098; c->b = 0.545098;
				break;
			case 0xc6d90285: /* "darkgoldenrod" */
				c->r = 0.721569; c->g = 0.525490; c->b = 0.043137;
				break;
			case 0x01f2399a: /* "darkgray" */
				c->r = 0.662745; c->g = 0.662745; c->b = 0.662745;
				break;
			case 0x40397bb8: /* "darkgreen" */
				c->r = 0.000000; c->g = 0.392157; c->b = 0.000000;
				break;
			case 0x01f23a1e: /* "darkgrey" */
				c->r = 0.662745; c->g = 0.662745; c->b = 0.662745;
				break;
			case 0x407c51af: /* "darkkhaki" */
				c->r = 0.741176; c->g = 0.717647; c->b = 0.419608;
				break;
			case 0xdae143e4: /* "darkmagenta" */
				c->r = 0.545098; c->g = 0.000000; c->b = 0.545098;
				break;
			case 0x092c7a97: /* "darkolivegreen" */
				c->r = 0.333333; c->g = 0.419608; c->b = 0.184314;
				break;
			case 0x5a102c03: /* "darkorange" */
				c->r = 1.000000; c->g = 0.549020; c->b = 0.000000;
				break;
			case 0x5a112b80: /* "darkorchid" */
				c->r = 0.600000; c->g = 0.196078; c->b = 0.800000;
				break;
			case 0x000f4622: /* "darkred" */
				c->r = 0.545098; c->g = 0.000000; c->b = 0.000000;
				break;
			case 0x623732f1: /* "darksalmon" */
				c->r = 0.913725; c->g = 0.588235; c->b = 0.478431;
				break;
			case 0xe6a4e091: /* "darkseagreen" */
				c->r = 0.560784; c->g = 0.737255; c->b = 0.560784;
				break;
			case 0x4e741068: /* "darkslateblue" */
				c->r = 0.282353; c->g = 0.239216; c->b = 0.545098;
				break;
			case 0x4e76e553: /* "darkslategray" */
				c->r = 0.184314; c->g = 0.309804; c->b = 0.309804;
				break;
			case 0x4e76e5d7: /* "darkslategrey" */
				c->r = 0.184314; c->g = 0.309804; c->b = 0.309804;
				break;
			case 0x28082838: /* "darkturquoise" */
				c->r = 0.000000; c->g = 0.807843; c->b = 0.819608;
				break;
			case 0x69c9107a: /* "darkviolet" */
				c->r = 0.580392; c->g = 0.000000; c->b = 0.827451;
				break;
			case 0x17e761b5: /* "deeppink" */
				c->r = 1.000000; c->g = 0.078431; c->b = 0.576471;
				break;
			case 0x84780222: /* "deepskyblue" */
				c->r = 0.000000; c->g = 0.749020; c->b = 1.000000;
				break;
			case 0x125bdeb2: /* "dimgray" */
				c->r = 0.411765; c->g = 0.411765; c->b = 0.411765;
				break;
			case 0x125bdf36: /* "dimgrey" */
				c->r = 0.411765; c->g = 0.411765; c->b = 0.411765;
				break;
			case 0x8208b222: /* "dodgerblue" */
				c->r = 0.117647; c->g = 0.564706; c->b = 1.000000;
				break;
			case 0x7ce7a736: /* "firebrick" */
				c->r = 0.698039; c->g = 0.133333; c->b = 0.133333;
				break;
			case 0xa97767c6: /* "floralwhite" */
				c->r = 1.000000; c->g = 0.980392; c->b = 0.941176;
				break;
			case 0x8eda0a29: /* "forestgreen" */
				c->r = 0.133333; c->g = 0.545098; c->b = 0.133333;
				break;
			case 0xc799dc48: /* "fuchsia" */
				c->r = 1.000000; c->g = 0.000000; c->b = 1.000000;
				break;
			case 0xf0b2b209: /* "gainsboro" */
				c->r = 0.862745; c->g = 0.862745; c->b = 0.862745;
				break;
			case 0x44ab668b: /* "ghostwhite" */
				c->r = 0.972549; c->g = 0.972549; c->b = 1.000000;
				break;
			case 0x7c97710b: /* "gold" */
				c->r = 1.000000; c->g = 0.843137; c->b = 0.000000;
				break;
			case 0xaaf0c023: /* "goldenrod" */
				c->r = 0.854902; c->g = 0.647059; c->b = 0.125490;
				break;
			case 0x7c977c78: /* "gray" */
				c->r = 0.501961; c->g = 0.501961; c->b = 0.501961;
				break;
			case 0x0f871a56: /* "green" */
				c->r = 0.000000; c->g = 0.501961; c->b = 0.000000;
				break;
			case 0xc0a3f4f2: /* "greenyellow" */
				c->r = 0.678431; c->g = 1.000000; c->b = 0.184314;
				break;
			case 0x7c977cfc: /* "grey" */
				c->r = 0.501961; c->g = 0.501961; c->b = 0.501961;
				break;
			case 0xdef14de8: /* "honeydew" */
				c->r = 0.941176; c->g = 1.000000; c->b = 0.941176;
				break;
			case 0x54c73bc2: /* "hotpink" */
				c->r = 1.000000; c->g = 0.411765; c->b = 0.705882;
				break;
			case 0x4b374f13: /* "indianred" */
				c->r = 0.803922; c->g = 0.360784; c->b = 0.360784;
				break;
			case 0x04cbd87f: /* "indigo" */
				c->r = 0.294118; c->g = 0.000000; c->b = 0.509804;
				break;
			case 0x0fada91e: /* "ivory" */
				c->r = 1.000000; c->g = 1.000000; c->b = 0.941176;
				break;
			case 0x0fc9f04d: /* "khaki" */
				c->r = 0.941176; c->g = 0.901961; c->b = 0.549020;
				break;
			case 0x6cec8bb6: /* "lavender" */
				c->r = 0.901961; c->g = 0.901961; c->b = 0.980392;
				break;
			case 0x4d30e8b4: /* "lavenderblush" */
				c->r = 1.000000; c->g = 0.941176; c->b = 0.960784;
				break;
			case 0x6bffad68: /* "lawngreen" */
				c->r = 0.486275; c->g = 0.988235; c->b = 0.000000;
				break;
			case 0x1aa3ab7d: /* "lemonchiffon" */
				c->r = 1.000000; c->g = 0.980392; c->b = 0.803922;
				break;
			case 0xf14e2ce5: /* "lightblue" */
				c->r = 0.678431; c->g = 0.847059; c->b = 0.901961;
				break;
			case 0x1b277a4e: /* "lightcoral" */
				c->r = 0.941176; c->g = 0.501961; c->b = 0.501961;
				break;
			case 0xf14eee08: /* "lightcyan" */
				c->r = 0.878431; c->g = 1.000000; c->b = 1.000000;
				break;
			case 0x269c7ed7: /* "lightgoldenrodyellow" */
				c->r = 0.980392; c->g = 0.980392; c->b = 0.823529;
				break;
			case 0xf15101d0: /* "lightgray" */
				c->r = 0.827451; c->g = 0.827451; c->b = 0.827451;
				break;
			case 0x1b714aae: /* "lightgreen" */
				c->r = 0.564706; c->g = 0.933333; c->b = 0.564706;
				break;
			case 0xf1510254: /* "lightgrey" */
				c->r = 0.827451; c->g = 0.827451; c->b = 0.827451;
				break;
			case 0xf155cc8f: /* "lightpink" */
				c->r = 1.000000; c->g = 0.713725; c->b = 0.756863;
				break;
			case 0xa468e0a7: /* "lightsalmon" */
				c->r = 1.000000; c->g = 0.627451; c->b = 0.478431;
				break;
			case 0x7bf8d3c7: /* "lightseagreen" */
				c->r = 0.125490; c->g = 0.698039; c->b = 0.666667;
				break;
			case 0x49bdb6bc: /* "lightskyblue" */
				c->r = 0.529412; c->g = 0.807843; c->b = 0.980392;
				break;
			case 0x8e493f49: /* "lightslategray" */
				c->r = 0.466667; c->g = 0.533333; c->b = 0.600000;
				break;
			case 0x8e493fcd: /* "lightslategrey" */
				c->r = 0.466667; c->g = 0.533333; c->b = 0.600000;
				break;
			case 0x01bf4e82: /* "lightsteelblue" */
				c->r = 0.690196; c->g = 0.768627; c->b = 0.870588;
				break;
			case 0xb2b03239: /* "lightyellow" */
				c->r = 1.000000; c->g = 1.000000; c->b = 0.878431;
				break;
			case 0x7c9a158c: /* "lime" */
				c->r = 0.000000; c->g = 1.000000; c->b = 0.000000;
				break;
			case 0xb749873d: /* "limegreen" */
				c->r = 0.196078; c->g = 0.803922; c->b = 0.196078;
				break;
			case 0x0fdccbbb: /* "linen" */
				c->r = 0.980392; c->g = 0.941176; c->b = 0.901961;
				break;
			case 0xb4110202: /* "magenta" */
				c->r = 1.000000; c->g = 0.000000; c->b = 1.000000;
				break;
			case 0x0d3d0451: /* "maroon" */
				c->r = 0.501961; c->g = 0.000000; c->b = 0.000000;
				break;
			case 0x5393448a: /* "mediumaquamarine" */
				c->r = 0.400000; c->g = 0.803922; c->b = 0.666667;
				break;
			case 0xd877eb4e: /* "mediumblue" */
				c->r = 0.000000; c->g = 0.000000; c->b = 0.803922;
				break;
			case 0xf4d5d5df: /* "mediumorchid" */
				c->r = 0.729412; c->g = 0.333333; c->b = 0.827451;
				break;
			case 0xf769a41e: /* "mediumpurple" */
				c->r = 0.576471; c->g = 0.439216; c->b = 0.858824;
				break;
			case 0x453d9eb0: /* "mediumseagreen" */
				c->r = 0.235294; c->g = 0.701961; c->b = 0.443137;
				break;
			case 0x80249267: /* "mediumslateblue" */
				c->r = 0.482353; c->g = 0.407843; c->b = 0.933333;
				break;
			case 0x814643ca: /* "mediumspringgreen" */
				c->r = 0.000000; c->g = 0.980392; c->b = 0.603922;
				break;
			case 0x59b8aa37: /* "mediumturquoise" */
				c->r = 0.282353; c->g = 0.819608; c->b = 0.800000;
				break;
			case 0x3be46a94: /* "mediumvioletred" */
				c->r = 0.780392; c->g = 0.082353; c->b = 0.521569;
				break;
			case 0x5f9313a1: /* "midnightblue" */
				c->r = 0.098039; c->g = 0.098039; c->b = 0.439216;
				break;
			case 0x9b7533e5: /* "mintcream" */
				c->r = 0.960784; c->g = 1.000000; c->b = 0.980392;
				break;
			case 0x1de6ab94: /* "mistyrose" */
				c->r = 1.000000; c->g = 0.894118; c->b = 0.882353;
				break;
			case 0x62609d92: /* "moccasin" */
				c->r = 1.000000; c->g = 0.894118; c->b = 0.709804;
				break;
			case 0xe2bc6625: /* "navajowhite" */
				c->r = 1.000000; c->g = 0.870588; c->b = 0.678431;
				break;
			case 0x7c9b0d83: /* "navy" */
				c->r = 0.000000; c->g = 0.000000; c->b = 0.501961;
				break;
			case 0x677b8e19: /* "oldlace" */
				c->r = 0.992157; c->g = 0.960784; c->b = 0.901961;
				break;
			case 0x1014a744: /* "olive" */
				c->r = 0.501961; c->g = 0.501961; c->b = 0.000000;
				break;
			case 0xcd1770fd: /* "olivedrab" */
				c->r = 0.419608; c->g = 0.556863; c->b = 0.137255;
				break;
			case 0x13119e61: /* "orange" */
				c->r = 1.000000; c->g = 0.647059; c->b = 0.000000;
				break;
			case 0xdc4c011c: /* "orangered" */
				c->r = 1.000000; c->g = 0.270588; c->b = 0.000000;
				break;
			case 0x13129dde: /* "orchid" */
				c->r = 0.854902; c->g = 0.439216; c->b = 0.839216;
				break;
			case 0x46e1ce45: /* "palegoldenrod" */
				c->r = 0.933333; c->g = 0.909804; c->b = 0.666667;
				break;
			case 0xda326778: /* "palegreen" */
				c->r = 0.596078; c->g = 0.984314; c->b = 0.596078;
				break;
			case 0xa810f3f8: /* "paleturquoise" */
				c->r = 0.686275; c->g = 0.933333; c->b = 0.933333;
				break;
			case 0x8a3cb455: /* "palevioletred" */
				c->r = 0.858824; c->g = 0.439216; c->b = 0.576471;
				break;
			case 0xc670dd19: /* "papayawhip" */
				c->r = 1.000000; c->g = 0.937255; c->b = 0.835294;
				break;
			case 0x37e01157: /* "peachpuff" */
				c->r = 1.000000; c->g = 0.854902; c->b = 0.725490;
				break;
			case 0x7c9c36c1: /* "peru" */
				c->r = 0.803922; c->g = 0.521569; c->b = 0.247059;
				break;
			case 0x7c9c4737: /* "pink" */
				c->r = 1.000000; c->g = 0.752941; c->b = 0.796078;
				break;
			case 0x7c9c54e3: /* "plum" */
				c->r = 0.866667; c->g = 0.627451; c->b = 0.866667;
				break;
			case 0x547b961e: /* "powderblue" */
				c->r = 0.690196; c->g = 0.878431; c->b = 0.901961;
				break;
			case 0x15a66c1d: /* "purple" */
				c->r = 0.501961; c->g = 0.000000; c->b = 0.501961;
				break;
			case 0x0b88a540: /* "red" */
				c->r = 1.000000; c->g = 0.000000; c->b = 0.000000;
				break;
			case 0xf7e975fa: /* "rosybrown" */
				c->r = 0.737255; c->g = 0.560784; c->b = 0.560784;
				break;
			case 0x8e773554: /* "royalblue" */
				c->r = 0.254902; c->g = 0.411765; c->b = 0.882353;
				break;
			case 0x92bbf35a: /* "saddlebrown" */
				c->r = 0.545098; c->g = 0.270588; c->b = 0.074510;
				break;
			case 0x1b38a54f: /* "salmon" */
				c->r = 0.980392; c->g = 0.501961; c->b = 0.447059;
				break;
			case 0xe10b172c: /* "sandybrown" */
				c->r = 0.956863; c->g = 0.643137; c->b = 0.376471;
				break;
			case 0xe5cc626f: /* "seagreen" */
				c->r = 0.180392; c->g = 0.545098; c->b = 0.341176;
				break;
			case 0xe6a00d96: /* "seashell" */
				c->r = 1.000000; c->g = 0.960784; c->b = 0.933333;
				break;
			case 0x1bc596c3: /* "sienna" */
				c->r = 0.627451; c->g = 0.321569; c->b = 0.176471;
				break;
			case 0x1bc98e5a: /* "silver" */
				c->r = 0.752941; c->g = 0.752941; c->b = 0.752941;
				break;
			case 0x9a861064: /* "skyblue" */
				c->r = 0.529412; c->g = 0.807843; c->b = 0.921569;
				break;
			case 0x328bce06: /* "slateblue" */
				c->r = 0.415686; c->g = 0.352941; c->b = 0.803922;
				break;
			case 0x328ea2f1: /* "slategray" */
				c->r = 0.439216; c->g = 0.501961; c->b = 0.564706;
				break;
			case 0x328ea375: /* "slategrey" */
				c->r = 0.439216; c->g = 0.501961; c->b = 0.564706;
				break;
			case 0x7c9e01cc: /* "snow" */
				c->r = 1.000000; c->g = 0.980392; c->b = 0.980392;
				break;
			case 0x6a6ae329: /* "springgreen" */
				c->r = 0.000000; c->g = 1.000000; c->b = 0.498039;
				break;
			case 0xa604b22a: /* "steelblue" */
				c->r = 0.274510; c->g = 0.509804; c->b = 0.705882;
				break;
			case 0x0b88ad48: /* "tan" */
				c->r = 0.823529; c->g = 0.705882; c->b = 0.549020;
				break;
			case 0x7c9e660b: /* "teal" */
				c->r = 0.000000; c->g = 0.501961; c->b = 0.501961;
				break;
			case 0xdf68be82: /* "thistle" */
				c->r = 0.847059; c->g = 0.749020; c->b = 0.847059;
				break;
			case 0x1e8b7ef9: /* "tomato" */
				c->r = 1.000000; c->g = 0.388235; c->b = 0.278431;
				break;
			case 0x0c1fe5d6: /* "turquoise" */
				c->r = 0.250980; c->g = 0.878431; c->b = 0.815686;
				break;
			case 0x22ca82d8: /* "violet" */
				c->r = 0.933333; c->g = 0.509804; c->b = 0.933333;
				break;
			case 0x10a3261e: /* "wheat" */
				c->r = 0.960784; c->g = 0.870588; c->b = 0.701961;
				break;
			case 0x10a33986: /* "white" */
				c->r = 1.000000; c->g = 1.000000; c->b = 1.000000;
				break;
			case 0x2580cae5: /* "whitesmoke" */
				c->r = 0.960784; c->g = 0.960784; c->b = 0.960784;
				break;
			case 0x297ff6e1: /* "yellow" */
				c->r = 1.000000; c->g = 1.000000; c->b = 0.000000;
				break;
			case 0xda4a85b2: /* "yellowgreen" */
				c->r = 0.603922; c->g = 0.803922; c->b = 0.196078;
				break;
			default:
				c->r = 1.000000; c->g = 1.000000; c->b = 1.000000;
				break;
			}
			c->a = 1.000000;
		}
	}
}
