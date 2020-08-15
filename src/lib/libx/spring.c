/*
 * libx/spring.c
 */

#include <math.h>
#include <spring.h>

#define SPRING_MAX_DELTA_TIME		(0.064f)
#define SPRING_SOLVER_STEP_TIME		(0.001f)

static inline int spring_atrest(struct spring_t * s)
{
	return (fabs(s->velocity) <= 0.005f) && ((fabs(s->stop - s->position) <= 0.005f) || (s->tension == 0.0f));
}

void spring_init(struct spring_t * s, double start, double stop, double velocity, double tension, double friction)
{
	s->tension = tension;
	s->friction = friction;

	s->start = start;
	s->stop = stop;
	s->position = s->start;
	s->velocity = velocity;
	s->pposition = s->position;
	s->pvelocity = s->velocity;
	s->tposition = s->position;
	s->tvelocity = s->velocity;
	s->accumulator = 0.0f;
}

int spring_step(struct spring_t * s, double dt)
{
	double va, vb, vc, vd;
	double aa, ab, ac, ad;
	double dxdt, dvdt;
	double alpha;

	if(spring_atrest(s))
		return 0;

	if(dt > SPRING_MAX_DELTA_TIME)
		dt = SPRING_MAX_DELTA_TIME;
	s->accumulator += dt;

	while(s->accumulator >= SPRING_SOLVER_STEP_TIME)
	{
		s->accumulator -= SPRING_SOLVER_STEP_TIME;
		if(s->accumulator < SPRING_SOLVER_STEP_TIME)
		{
			s->pposition = s->position;
			s->pvelocity = s->velocity;
		}
		va = s->velocity;
		aa = (s->tension * (s->stop - s->tposition)) - s->friction * s->velocity;

		s->tposition = s->position + va * (SPRING_SOLVER_STEP_TIME * 0.5f);
		s->tvelocity = s->velocity + aa * (SPRING_SOLVER_STEP_TIME * 0.5f);
		vb = s->tvelocity;
		ab = (s->tension * (s->stop - s->tposition)) - s->friction * s->tvelocity;

		s->tposition = s->position + vb * (SPRING_SOLVER_STEP_TIME * 0.5f);
		s->tvelocity = s->velocity + ab * (SPRING_SOLVER_STEP_TIME * 0.5f);
		vc = s->tvelocity;
		ac = (s->tension * (s->stop - s->tposition)) - s->friction * s->tvelocity;

		s->tposition = s->position + vc * SPRING_SOLVER_STEP_TIME;
		s->tvelocity = s->velocity + ac * SPRING_SOLVER_STEP_TIME;
		vd = s->tvelocity;
		ad = (s->tension * (s->stop - s->tposition)) - s->friction * s->tvelocity;

		dxdt = (1.0f / 6.0f) * (va + 2.0f * (vb + vc) + vd);
		dvdt = (1.0f / 6.0f) * (aa + 2.0f * (ab + ac) + ad);
		s->position += dxdt * SPRING_SOLVER_STEP_TIME;
		s->velocity += dvdt * SPRING_SOLVER_STEP_TIME;
	}
	if(s->accumulator > 0.0f)
	{
		alpha = s->accumulator * (1.0f / SPRING_SOLVER_STEP_TIME);
		s->position = s->position * alpha + s->pposition * (1.0f - alpha);
		s->velocity = s->velocity * alpha + s->pvelocity * (1.0f - alpha);
	}
	if(spring_atrest(s))
	{
		if(s->tension > 0.0f)
		{
			s->start = s->stop;
			s->position = s->stop;
		}
		else
		{
			s->stop = s->position;
			s->start = s->stop;
		}
		s->velocity = 0.0f;
	}
	return 1;
}
