/*
 * framework/core/l-easing.c
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

#include <xboot.h>
#include <framework/core/l-easing.h>

/*
 * t = elapsed time
 * b = begin value
 * c = change value (ending - beginning)
 * d = duration (total time)
 * func = easing function will be invoked in '__call' method
 */
struct leasing_t {
	double b;
	double c;
	double d;
	double ax, bx, cx;
	double ay, by, cy;
	double start, end;
	double (*func)(struct leasing_t * e, double t);
};

static double linear(struct leasing_t * e, double t)
{
	return e->c * t / e->d + e->b;
}

static double sine_in(struct leasing_t * e, double t)
{
	return -e->c * cos(t / e->d * M_PI_2) + e->c + e->b;
}

static double sine_out(struct leasing_t * e, double t)
{
	return e->c * sin(t / e->d * M_PI_2) + e->b;
}

static double sine_in_out(struct leasing_t * e, double t)
{
	return -e->c / 2 * (cos(M_PI * t / e->d) - 1) + e->b;
}

static double quad_in(struct leasing_t * e, double t)
{
	t = t / e->d;
	return e->c * pow(t, 2) + e->b;
}

static double quad_out(struct leasing_t * e, double t)
{
	t = t / e->d;
	return -e->c * t * (t - 2) + e->b;
}

static double quad_in_out(struct leasing_t * e, double t)
{
	double r;
	t = t / e->d * 2;
	if(t < 1)
		r = e->c / 2 * pow(t, 2) + e->b;
	else
		r = -e->c / 2 * ((t - 1) * (t - 3) - 1) + e->b;
	return r;
}

static double cubic_in(struct leasing_t * e, double t)
{
	t = t / e->d;
	return e->c * pow(t, 3) + e->b;
}

static double cubic_out(struct leasing_t * e, double t)
{
	t = t / e->d - 1;
	return e->c * (pow(t, 3) + 1) + e->b;
}

static double cubic_in_out(struct leasing_t * e, double t)
{
	double r;
	t = t / e->d * 2;
	if(t < 1)
	{
		r =  e->c / 2 * pow(t, 3) + e->b;
	}
	else
	{
		t = t - 2;
		r = e->c / 2 * (pow(t, 3) + 2) + e->b;
	}
	return r;
}

static double quart_in(struct leasing_t * e, double t)
{
	t = t / e->d;
	return e->c * pow(t, 4) + e->b;
}

static double quart_out(struct leasing_t * e, double t)
{
	t = t / e->d - 1;
	return -e->c * (pow(t, 4) - 1) + e->b;
}

static double quart_in_out(struct leasing_t * e, double t)
{
	double r;
	t = t / e->d * 2;
	if(t < 1)
	{
		r =  e->c / 2 * pow(t, 4) + e->b;
	}
	else
	{
		t = t - 2;
		r = -e->c / 2 * (pow(t, 4) - 2) + e->b;
	}
	return r;
}

static double quint_in(struct leasing_t * e, double t)
{
	t = t / e->d;
	return e->c * pow(t, 5) + e->b;
}

static double quint_out(struct leasing_t * e, double t)
{
	t = t / e->d - 1;
	return e->c * (pow(t, 5) + 1) + e->b;
}

static double quint_in_out(struct leasing_t * e, double t)
{
	double r;
	t = t / e->d * 2;
	if(t < 1)
	{
		r = e->c / 2 * pow(t, 5) + e->b;
	}
	else
	{
		t = t - 2;
		r = e->c / 2 * (pow(t, 5) + 2) + e->b;
	}
	return r;
}

static double expo_in(struct leasing_t * e, double t)
{
	double r;
	if(t == 0)
		r = e->b;
	else
		r = e->c * pow(2, 10 * (t / e->d - 1)) + e->b - e->c * 0.001;
	return r;
}

static double expo_out(struct leasing_t * e, double t)
{
	double r;
	if(t == e->d)
		r = e->b + e->c;
	else
		r = e->c * 1.001 * (-pow(2, -10 * t / e->d) + 1) + e->b;
	return r;
}

static double expo_in_out(struct leasing_t * e, double t)
{
	double r;
	if(t == 0)
	{
		r = e->b;
	}
	else if(t == e->d)
	{
		r = e->b + e->c;
	}
	else
	{
		t = t / e->d * 2;
		if(t < 1)
		{
			r = e->c / 2 * pow(2, 10 * (t - 1)) + e->b - e->c * 0.0005;
		}
		else
		{
			t = t - 1;
			r = e->c / 2 * 1.0005 * (-pow(2, -10 * t) + 2) + e->b;
		}
	}
	return r;
}

static double circ_in(struct leasing_t * e, double t)
{
	t = t / e->d;
	return -e->c * (sqrt(1 - pow(t, 2)) - 1) + e->b;
}

static double circ_out(struct leasing_t * e, double t)
{
	t = t / e->d - 1;
	return e->c * sqrt(1 - pow(t, 2)) + e->b;
}

static double circ_in_out(struct leasing_t * e, double t)
{
	double r;
	t = t / e->d * 2;
	if(t < 1)
	{
		r = -e->c / 2 * (sqrt(1 - t * t) - 1) + e->b;
	}
	else
	{
		t = t - 2;
		r = e->c / 2 * (sqrt(1 - t * t) + 1) + e->b;
	}
	return r;
}

static double back_in(struct leasing_t * e, double t)
{
	double s = 1.70158;
	t = t / e->d;
	return e->c * t * t * ((s + 1) * t - s) + e->b;
}

static double back_out(struct leasing_t * e, double t)
{
	double s = 1.70158;
	t = t / e->d - 1;
	return e->c * (t * t * ((s + 1) * t + s) + 1) + e->b;
}

static double back_in_out(struct leasing_t * e, double t)
{
	double s = 1.70158;
	double r;
	s = s * 1.525;
	t = t / e->d * 2;
	if(t < 1)
	{
		r = e->c / 2 * (t * t * ((s + 1) * t - s)) + e->b;
	}
	else
	{
		t = t - 2;
		r = e->c / 2 * (t * t * ((s + 1) * t + s) + 2) + e->b;
	}
	return r;
}

static double elastic_in(struct leasing_t * e, double t)
{
	double r;
	if(t == 0)
	{
		r = e->b;
	}
	else
	{
		t = t / e->d;
		if(t == 1)
		{
			r = e->b + e->c;
		}
		else
		{
			double p = e->d * 0.3;
			double s = p / 4;
			double a = e->c;
			t = t - 1;
			r = -(a * pow(2, 10 * t) * sin((t * e->d - s) * (2 * M_PI) / p)) + e->b;
		}
	}
	return r;
}

static double elastic_out(struct leasing_t * e, double t)
{
	double r;
	if(t == 0)
	{
		r = e->b;
	}
	else
	{
		t = t / e->d;
		if(t == 1)
		{
			r = e->b + e->c;
		}
		else
		{
			double p = e->d * 0.3;
			double s = p / 4;
			double a = e->c;
			r = a * pow(2, -10 * t) * sin((t * e->d - s) * (2 * M_PI) / p) + e->c + e->b;
		}
	}
	return r;
}

static double elastic_in_out(struct leasing_t * e, double t)
{
	double r;
	if(t == 0)
	{
		r = e->b;
	}
	else
	{
		t = t / e->d * 2;
		if(t == 2)
		{
			r = e->b + e->c;
		}
		else
		{
			double p = e->d * (0.3 * 1.5);
			double a = e->c;
			double s = p / 4;
			if(t < 1)
			{
				t = t - 1;
				r = -0.5 * (a * pow(2, 10 * t) * sin((t * e->d - s) * (2 * M_PI) / p)) + e->b;
			}
			else
			{
				t = t - 1;
				r = a * pow(2, -10 * t) * sin((t * e->d - s) * (2 * M_PI) / p) * 0.5 + e->c	+ e->b;
			}
		}
	}
	return r;
}

static inline double __bounce_out(double t, double b, double c, double d)
{
	double r;
	t = t / d;
	if(t < 1 / 2.75)
	{
		r = c * (7.5625 * t * t) + b;
	}
	else if(t < 2 / 2.75)
	{
		t = t - (1.5 / 2.75);
		r = c * (7.5625 * t * t + 0.75) + b;
	}
	else if(t < 2.5 / 2.75)
	{
		t = t - (2.25 / 2.75);
		r = c * (7.5625 * t * t + 0.9375) + b;
	}
	else
	{
		t = t - (2.625 / 2.75);
		r = c * (7.5625 * t * t + 0.984375) + b;
	}
	return r;
}

static inline double __bounce_in(double t, double b, double c, double d)
{
	return c - __bounce_out(d - t, 0, c, d) + b;
}

static double bounce_in(struct leasing_t * e, double t)
{
	return __bounce_in(t, e->b, e->c, e->d);
}

static double bounce_out(struct leasing_t * e, double t)
{
	return __bounce_out(t, e->b, e->c, e->d);
}

static double bounce_in_out(struct leasing_t * e, double t)
{
	double r;
	if (t < e->d / 2)
		r = __bounce_in(t * 2, 0, e->c, e->d) * 0.5 + e->b;
	else
		r = __bounce_out(t * 2 - e->d, 0, e->c, e->d) * 0.5 + e->c * 0.5 + e->b;
	return r;
}

static inline double sample_curve_x(struct leasing_t * e, double t)
{
    return ((e->ax * t + e->bx) * t + e->cx) * t;
}

static inline double sample_curve_y(struct leasing_t * e, double t)
{
    return ((e->ay * t + e->by) * t + e->cy) * t;
}

static inline double sample_curve_derivative_x(struct leasing_t * e, double t)
{
    return (3.0 * e->ax * t + 2.0 * e->bx) * t + e->cx;
}

static inline double solve_curve_x(struct leasing_t * e, double t)
{
	double t0, t1, t2;
	double x2, d2;
	int i;
	for(t2 = t, i = 0; i < 8; i++)
	{
		x2 = sample_curve_x(e, t2) - t;
		if(fabs(x2) < 1e-7)
			return t2;
		d2 = sample_curve_derivative_x(e, t2);
		if(fabs(d2) < 1e-6)
			break;
		t2 = t2 - x2 / d2;
	}
	t0 = 0.0;
	t1 = 1.0;
	t2 = t;
	while(t0 < t1)
	{
		x2 = sample_curve_x(e, t2);
		if(fabs(x2 - t) < 1e-7)
			return t2;
		if(t > x2)
			t0 = t2;
		else
			t1 = t2;
		t2 = (t1 - t0) * 0.5 + t0;
	}
	return t2;
}

static double cubic_bezier(struct leasing_t * e, double t)
{
	double r;
	if(t < 0.0)
		r = 0.0 + e->start * t;
	else if(t > 1.0)
		r = 1.0 + e->end * (t - 1.0);
	else
		r = sample_curve_y(e, solve_curve_x(e, t));
	return e->c * r / e->d + e->b;
}

static int l_new(lua_State * L)
{
	struct leasing_t * e = lua_newuserdata(L, sizeof(struct leasing_t));
	e->b = luaL_optnumber(L, 1, 0);
	e->c = luaL_optnumber(L, 2, 1);
	e->d = luaL_optnumber(L, 3, 1);
	if(lua_isstring(L, 4))
	{
		const char * type = luaL_optstring(L, 4, "linear");
		switch(shash(type))
		{
		case 0x0b7641e0: /* "linear" */
			e->func = linear;
			break;
		case 0x95154938: /* "sine-in" */
			e->func = sine_in;
			break;
		case 0x37be8b19: /* "sine-out" */
			e->func = sine_out;
			break;
		case 0x44a1b9fd: /* "sine-in-out" */
			e->func = sine_in_out;
			break;
		case 0x1632ec54: /* "quad-in" */
			e->func = quad_in;
			break;
		case 0xdc9091b5: /* "quad-out" */
			e->func = quad_out;
			break;
		case 0x93d38b19: /* "quad-in-out" */
			e->func = quad_in_out;
			break;
		case 0xf3c312af: /* "cubic-in" */
			e->func = cubic_in;
			break;
		case 0x6c258370: /* "cubic-out" */
			e->func = cubic_out;
			break;
		case 0x6f7566f4: /* "cubic-in-out" */
			e->func = cubic_in_out;
			break;
		case 0xddb3bd56: /* "quart-in" */
			e->func = quart_in;
			break;
		case 0x942b82f7: /* "quart-out" */
			e->func = quart_out;
			break;
		case 0xe1790d1b: /* "quart-in-out" */
			e->func = quart_in_out;
			break;
		case 0xf014a05a: /* "quint-in" */
			e->func = quint_in;
			break;
		case 0xf2a8c67b: /* "quint-out" */
			e->func = quint_out;
			break;
		case 0x2fdbd21f: /* "quint-in-out" */
			e->func = quint_in_out;
			break;
		case 0x828d04e5: /* "expo-in" */
			e->func = expo_in;
			break;
		case 0xd42dbc66: /* "expo-out" */
			e->func = expo_out;
			break;
		case 0x59b9842a: /* "expo-in-out" */
			e->func = expo_in_out;
			break;
		case 0xc5b8c66a: /* "circ-in" */
			e->func = circ_in;
			break;
		case 0x7cd1ae8b: /* "circ-out" */
			e->func = circ_out;
			break;
		case 0xe844802f: /* "circ-in-out" */
			e->func = circ_in_out;
			break;
		case 0x650a381a: /* "back-in" */
			e->func = back_in;
			break;
		case 0x0651563b: /* "back-out" */
			e->func = back_out;
			break;
		case 0xd15749df: /* "back-in-out" */
			e->func = back_in_out;
			break;
		case 0x21ed4e6e: /* "elastic-in" */
			e->func = elastic_in;
			break;
		case 0x5f97370f: /* "elastic-out" */
			e->func = elastic_out;
			break;
		case 0xdbc56a33: /* "elastic-in-out" */
			e->func = elastic_in_out;
			break;
		case 0x3077fd85: /* "bounce-in" */
			e->func = bounce_in;
			break;
		case 0x3f77c906: /* "bounce-out" */
			e->func = bounce_out;
			break;
		case 0x7fafccca: /* "bounce-in-out" */
			e->func = bounce_in_out;
			break;
		default:
			e->func = linear;
			break;
		}
	}
	else if(lua_istable(L, 4) && (lua_rawlen(L, 4) == 4))
	{
		double x1, y1;
		double x2, y2;
		lua_rawgeti(L, 4, 1); x1 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 4, 2); y1 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 4, 3); x2 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 4, 4); y2 = lua_tonumber(L, -1); lua_pop(L, 1);
		e->cx = 3.0 * x1;
		e->bx = 3.0 * (x2 - x1) - e->cx;
		e->ax = 1.0 - e->cx - e->bx;
		e->cy = 3.0 * y1;
		e->by = 3.0 * (y2 - y1) - e->cy;
		e->ay = 1.0 - e->cy - e->by;
		if(x1 > 0)
			e->start = y1 / x1;
		else if(!y1 && (x2 > 0))
			e->start = y2 / x2;
		else
			e->start = 0;
		if(x2 < 1)
			e->end = (y2 - 1) / (x2 - 1);
		else if((x2 == 1) && (x1 < 1))
			e->end = (y1 - 1) / (x1 - 1);
		else
			e->end = 0;
		e->func = cubic_bezier;
	}
	else
	{
		e->func = linear;
	}
	luaL_setmetatable(L, MT_EASING);
	return 1;
}

static const luaL_Reg l_easing[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_easing_call(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = e->func(e, t);
	lua_pushnumber(L, r);
	return 1;
}

static const luaL_Reg m_easing[] = {
	{"__call",	m_easing_call},
	{NULL,		NULL}
};

int luaopen_easing(lua_State * L)
{
	luaL_newlib(L, l_easing);
	luahelper_create_metatable(L, MT_EASING, m_easing);
	return 1;
}
