/*
 * libx/spring.c
 */

#include <math.h>
#include <spring.h>

#define SPRING_MAX_DELTA_TIME	(0.083f)
#define SPRING_STEP_TIME		(0.001f)
#define SPRING_PRECISION		(0.005f)

void spring_init(struct spring_t * s, double start, double stop, double velocity, double tension, double friction)
{
	s->start = start;
	s->stop = stop;
	s->velocity = velocity;
	s->tension = tension;
	s->friction = friction;
	s->acc = 0;
}

int spring_step(struct spring_t * s, double dt)
{
	double v, p;

	if((s->tension == 0.0f) || (fabs(s->velocity) <= SPRING_PRECISION && fabs(s->stop - s->start) <= SPRING_PRECISION))
		return 0;
	if(dt > SPRING_MAX_DELTA_TIME)
		dt = SPRING_MAX_DELTA_TIME;
	s->acc += dt;
	while(s->acc >= SPRING_STEP_TIME)
	{
		s->acc -= SPRING_STEP_TIME;
		v = s->velocity + (s->tension * (s->stop - s->start) - s->friction * s->velocity) * SPRING_STEP_TIME;
		p = s->start + v * SPRING_STEP_TIME;
		if(fabs(v) <= SPRING_PRECISION && fabs(s->stop - p) <= SPRING_PRECISION)
		{
			s->start = s->stop;
			s->velocity = 0;
			break;
		}
		else
		{
			s->start = p;
			s->velocity = v;
		}
	}
	return 1;
}
