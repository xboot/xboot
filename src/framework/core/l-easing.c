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
 * For all easing functions:
 *
 * t = elapsed time
 * b = begin value
 * c = change value (ending - beginning)
 * d = duration (total time)
 * func = easing function will be invoked in 'easing' method
 */
struct leasing_t {
	double b;
	double c;
	double d;
	int (*func)(lua_State *L);
};

static int m_linear(lua_State * L);
static int m_in_sine(lua_State * L);
static int m_out_sine(lua_State * L);
static int m_in_out_sine(lua_State * L);
static int m_in_quad(lua_State * L);
static int m_out_quad(lua_State * L);
static int m_in_out_quad(lua_State * L);
static int m_in_cubic(lua_State * L);
static int m_out_cubic(lua_State * L);
static int m_in_out_cubic(lua_State * L);
static int m_in_quart(lua_State * L);
static int m_out_quart(lua_State * L);
static int m_in_out_quart(lua_State * L);
static int m_in_quint(lua_State * L);
static int m_out_quint(lua_State * L);
static int m_in_out_quint(lua_State * L);
static int m_in_expo(lua_State * L);
static int m_out_expo(lua_State * L);
static int m_in_out_expo(lua_State * L);
static int m_in_circ(lua_State * L);
static int m_out_circ(lua_State * L);
static int m_in_out_circ(lua_State * L);
static int m_in_back(lua_State * L);
static int m_out_back(lua_State * L);
static int m_in_out_back(lua_State * L);
static int m_in_elastic(lua_State * L);
static int m_out_elastic(lua_State * L);
static int m_in_out_elastic(lua_State * L);
static int m_in_bounce(lua_State * L);
static int m_out_bounce(lua_State * L);
static int m_in_out_bounce(lua_State * L);

static int l_new(lua_State * L)
{
	struct leasing_t * e = lua_newuserdata(L, sizeof(struct leasing_t));
	const char * type = luaL_optstring(L, 4, "linear");
	e->b = luaL_optnumber(L, 1, 0);
	e->c = luaL_optnumber(L, 2, 1);
	e->d = luaL_optnumber(L, 3, 1);
	if(strcmp(type, "linear") == 0)
		e->func = m_linear;
	else if(strcmp(type, "inSine") == 0)
		e->func = m_in_sine;
	else if(strcmp(type, "outSine") == 0)
		e->func = m_out_sine;
	else if(strcmp(type, "inOutSine") == 0)
		e->func = m_in_out_sine;
	else if(strcmp(type, "inQuad") == 0)
		e->func = m_in_quad;
	else if(strcmp(type, "outQuad") == 0)
		e->func = m_out_quad;
	else if(strcmp(type, "inOutQuad") == 0)
		e->func = m_in_out_quad;
	else if(strcmp(type, "inCubic") == 0)
		e->func = m_in_cubic;
	else if(strcmp(type, "outCubic") == 0)
		e->func = m_out_cubic;
	else if(strcmp(type, "inOutCubic") == 0)
		e->func = m_in_out_cubic;
	else if(strcmp(type, "inQuart") == 0)
		e->func = m_in_quart;
	else if(strcmp(type, "outQuart") == 0)
		e->func = m_out_quart;
	else if(strcmp(type, "inOutQuart") == 0)
		e->func = m_in_out_quart;
	else if(strcmp(type, "inQuint") == 0)
		e->func = m_in_quint;
	else if(strcmp(type, "outQuint") == 0)
		e->func = m_out_quint;
	else if(strcmp(type, "inOutQuint") == 0)
		e->func = m_in_out_quint;
	else if(strcmp(type, "inExpo") == 0)
		e->func = m_in_expo;
	else if(strcmp(type, "outExpo") == 0)
		e->func = m_out_expo;
	else if(strcmp(type, "inOutExpo") == 0)
		e->func = m_in_out_expo;
	else if(strcmp(type, "inCirc") == 0)
		e->func = m_in_circ;
	else if(strcmp(type, "outCirc") == 0)
		e->func = m_out_circ;
	else if(strcmp(type, "inOutCirc") == 0)
		e->func = m_in_out_circ;
	else if(strcmp(type, "inBack") == 0)
		e->func = m_in_back;
	else if(strcmp(type, "outBack") == 0)
		e->func = m_out_back;
	else if(strcmp(type, "inOutBack") == 0)
		e->func = m_in_out_back;
	else if(strcmp(type, "inElastic") == 0)
		e->func = m_in_elastic;
	else if(strcmp(type, "outElastic") == 0)
		e->func = m_out_elastic;
	else if(strcmp(type, "inOutElastic") == 0)
		e->func = m_in_out_elastic;
	else if(strcmp(type, "inBounce") == 0)
		e->func = m_in_bounce;
	else if(strcmp(type, "outBounce") == 0)
		e->func = m_out_bounce;
	else if(strcmp(type, "inOutBounce") == 0)
		e->func = m_in_out_bounce;
	else
		e->func = m_linear;
	luaL_setmetatable(L, MT_EASING);
	return 1;
}

static const luaL_Reg l_easing[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_invoke_easing(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	return e->func(L);
}

static int m_linear(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = e->c * t / e->d + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_sine(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = -e->c * cos(t / e->d * M_PI_2) + e->c + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_sine(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = e->c * sin(t / e->d * M_PI_2) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_sine(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = -e->c / 2 * (cos(M_PI * t / e->d) - 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_quad(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d;
	r = e->c * pow(t, 2) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_quad(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d;
	r = -e->c * t * (t - 2) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_quad(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d * 2;
	if(t < 1)
		r =  e->c / 2 * pow(t, 2) + e->b;
	else
		r = -e->c / 2 * ((t - 1) * (t - 3) - 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_cubic(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d;
	r = e->c * pow(t, 3) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_cubic(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d - 1;
	r = e->c * (pow(t, 3) + 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_cubic(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d * 2;
	if(t < 1)
		r =  e->c / 2 * pow(t, 3) + e->b;
	else
	{
		t = t - 2;
		r = e->c / 2 * (pow(t, 3) + 2) + e->b;
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_quart(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d;
	r = e->c * pow(t, 4) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_quart(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d - 1;
	r = -e->c * (pow(t, 4) - 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_quart(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d * 2;
	if(t < 1)
		r =  e->c / 2 * pow(t, 4) + e->b;
	else
	{
		t = t - 2;
		r = -e->c / 2 * (pow(t, 4) - 2) + e->b;
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_quint(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d;
	r = e->c * pow(t, 5) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_quint(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d - 1;
	r = e->c * (pow(t, 5) + 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_quint(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d * 2;
	if(t < 1)
		r = e->c / 2 * pow(t, 5) + e->b;
	else
	{
		t = t - 2;
		r = e->c / 2 * (pow(t, 5) + 2) + e->b;
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_expo(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if(t == 0)
		r = e->b;
	else
		r = e->c * pow(2, 10 * (t / e->d - 1)) + e->b - e->c * 0.001;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_expo(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if(t == e->d)
		r = e->b + e->c;
	else
		r = e->c * 1.001 * (-pow(2, -10 * t / e->d) + 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_expo(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if(t == 0)
		r = e->b;
	else if(t == e->d)
		r = e->b + e->c;
	else
	{
		t = t / e->d * 2;
		if(t < 1)
			r = e->c / 2 * pow(2, 10 * (t - 1)) + e->b - e->c * 0.0005;
		else
		{
			t = t - 1;
			r = e->c / 2 * 1.0005 * (-pow(2, -10 * t) + 2) + e->b;
		}
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_circ(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d;
	r = -e->c * (sqrt(1 - pow(t, 2)) - 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_circ(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d - 1;
	r = e->c * sqrt(1 - pow(t, 2)) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_circ(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	t = t / e->d * 2;
	if(t < 1)
		r = -e->c / 2 * (sqrt(1 - t * t) - 1) + e->b;
	else
	{
		t = t - 2;
		r = e->c / 2 * (sqrt(1 - t * t) + 1) + e->b;
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_back(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double s = 1.70158;
	double r;
	t = t / e->d;
	r = e->c * t * t * ((s + 1) * t - s) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_back(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double s = 1.70158;
	double r;
	t = t / e->d - 1;
	r = e->c * (t * t * ((s + 1) * t + s) + 1) + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_back(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double s = 1.70158;
	double r;
	s = s * 1.525;
	t = t / e->d * 2;
	if(t < 1)
		r = e->c / 2 * (t * t * ((s + 1) * t - s)) + e->b;
	else
	{
		t = t - 2;
		r = e->c / 2 * (t * t * ((s + 1) * t + s) + 2) + e->b;
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_elastic(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if(t == 0)
		r = e->b;
	else
	{
		t = t / e->d;
		if(t == 1)
			r = e->b + e->c;
		else
		{
			double p = e->d * 0.3;
			double s = p / 4;
			double a = e->c;
			t = t - 1;
			r = -(a * pow(2, 10 * t) * sin((t * e->d - s) * (2 * M_PI) / p)) + e->b;
		}
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_elastic(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if(t == 0)
		r = e->b;
	else
	{
		t = t / e->d;
		if(t == 1)
			r = e->b + e->c;
		else
		{
			double p = e->d * 0.3;
			double s = p / 4;
			double a = e->c;
			r = a * pow(2, -10 * t) * sin((t * e->d - s) * (2 * M_PI) / p) + e->c + e->b;
		}
	}
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_elastic(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if(t == 0)
		r = e->b;
	else
	{
		t = t / e->d * 2;
		if(t == 2)
			r = e->b + e->c;
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
	lua_pushnumber(L, r);
	return 1;
}

static double __out_bounce(double t, double b, double c, double d)
{
	t = t / d;
	if(t < 1 / 2.75)
		return c * (7.5625 * t * t) + b;
	else if(t < 2 / 2.75)
	{
		t = t - (1.5 / 2.75);
		return c * (7.5625 * t * t + 0.75) + b;
	}
	else if(t < 2.5 / 2.75)
	{
		t = t - (2.25 / 2.75);
		return c * (7.5625 * t * t + 0.9375) + b;
	}
	else
	{
		t = t - (2.625 / 2.75);
		return c * (7.5625 * t * t + 0.984375) + b;
	}
}

static double __in_bounce(double t, double b, double c, double d)
{
	return c - __out_bounce(d - t, 0, c, d) + b;
}

static int m_in_bounce(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = __in_bounce(t, e->b, e->c, e->d);
	lua_pushnumber(L, r);
	return 1;
}

static int m_out_bounce(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = __out_bounce(t, e->b, e->c, e->d);
	lua_pushnumber(L, r);
	return 1;
}

static int m_in_out_bounce(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r;
	if (t < e->d / 2)
		r = __in_bounce(t * 2, 0, e->c, e->d) * 0.5 + e->b;
	else
		r = __out_bounce(t * 2 - e->d, 0, e->c, e->d) * 0.5 + e->c * 0.5 + e->b;
	lua_pushnumber(L, r);
	return 1;
}

static const luaL_Reg m_easing[] = {
	{"easing",			m_invoke_easing},
	{"linear",			m_linear},
	{"inSine",			m_in_sine},
	{"outSine",			m_out_sine},
	{"inOutSine",		m_in_out_sine},
	{"inQuad",			m_in_quad},
	{"outQuad",			m_out_quad},
	{"inOutQuad",		m_in_out_quad},
	{"inCubic",			m_in_cubic},
	{"outCubic",		m_out_cubic},
	{"inOutCubic",		m_in_out_cubic},
	{"inQuart",			m_in_quart},
	{"outQuart",		m_out_quart},
	{"inOutQuart",		m_in_out_quart},
	{"inQuint",			m_in_quint},
	{"outQuint",		m_out_quint},
	{"inOutQuint",		m_in_out_quint},
	{"inExpo",			m_in_expo},
	{"outExpo",			m_out_expo},
	{"inOutExpo",		m_in_out_expo},
	{"inCirc",			m_in_circ},
	{"outCirc",			m_out_circ},
	{"inOutCirc",		m_in_out_circ},
	{"inBack",			m_in_back},
	{"outBack",			m_out_back},
	{"inOutBack",		m_in_out_back},
	{"inElastic",		m_in_elastic},
	{"outElastic",		m_out_elastic},
	{"inOutElastic",	m_in_out_elastic},
	{"inBounce",		m_in_bounce},
	{"outBounce",		m_out_bounce},
	{"inOutBounce",		m_in_out_bounce},
	{NULL,				NULL}
};

int luaopen_easing(lua_State * L)
{
	luaL_newlib(L, l_easing);
	luahelper_create_metatable(L, MT_EASING, m_easing);
	return 1;
}
