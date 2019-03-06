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

static int l_new(lua_State * L)
{
	struct leasing_t * e = lua_newuserdata(L, sizeof(struct leasing_t));
	const char * type = luaL_optstring(L, 4, "linear");
	e->b = luaL_optnumber(L, 1, 0);
	e->c = luaL_optnumber(L, 2, 1);
	e->d = luaL_optnumber(L, 3, 1);
	e->func = linear;
	if(type)
	{
		switch(type[0])
		{
		case 'b':
			if(strcmp(type, "back-in") == 0)
				e->func = back_in;
			else if(strcmp(type, "back-in-out") == 0)
				e->func = back_in_out;
			else if(strcmp(type, "back-out") == 0)
				e->func = back_out;
			else if(strcmp(type, "bounce-in") == 0)
				e->func = bounce_in;
			else if(strcmp(type, "bounce-in-out") == 0)
				e->func = bounce_in_out;
			else if(strcmp(type, "bounce-out") == 0)
				e->func = bounce_out;
			break;
		case 'c':
			if(strcmp(type, "circ-in") == 0)
				e->func = circ_in;
			else if(strcmp(type, "circ-in-out") == 0)
				e->func = circ_in_out;
			else if(strcmp(type, "circ-out") == 0)
				e->func = circ_out;
			else if(strcmp(type, "cubic-in") == 0)
				e->func = cubic_in;
			else if(strcmp(type, "cubic-in-out") == 0)
				e->func = cubic_in_out;
			else if(strcmp(type, "cubic-out") == 0)
				e->func = cubic_out;
			break;
		case 'e':
			if(strcmp(type, "elastic-in") == 0)
				e->func = elastic_in;
			else if(strcmp(type, "elastic-in-out") == 0)
				e->func = elastic_in_out;
			else if(strcmp(type, "elastic-out") == 0)
				e->func = elastic_out;
			else if(strcmp(type, "expo-in") == 0)
				e->func = expo_in;
			else if(strcmp(type, "expo-in-out") == 0)
				e->func = expo_in_out;
			else if(strcmp(type, "expo-out") == 0)
				e->func = expo_out;
			break;
		case 'q':
			if(strcmp(type, "quad-in") == 0)
				e->func = quad_in;
			else if(strcmp(type, "quad-in-out") == 0)
				e->func = quad_in_out;
			else if(strcmp(type, "quad-out") == 0)
				e->func = quad_out;
			else if(strcmp(type, "quart-in") == 0)
				e->func = quart_in;
			else if(strcmp(type, "quart-in-out") == 0)
				e->func = quart_in_out;
			else if(strcmp(type, "quart-out") == 0)
				e->func = quart_out;
			else if(strcmp(type, "quint-in") == 0)
				e->func = quint_in;
			else if(strcmp(type, "quint-in-out") == 0)
				e->func = quint_in_out;
			else if(strcmp(type, "quint-out") == 0)
				e->func = quint_out;
			break;
		case 's':
			if(strcmp(type, "sine-in") == 0)
				e->func = sine_in;
			else if(strcmp(type, "sine-in-out") == 0)
				e->func = sine_in_out;
			else if(strcmp(type, "sine-out") == 0)
				e->func = sine_out;
			break;
		case 'l':
			if(strcmp(type, "linear") == 0)
				e->func = linear;
			break;
		default:
			break;
		}
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
