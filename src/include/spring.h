#ifndef __SPRING_H__
#define __SPRING_H__

#ifdef __cplusplus
extern "C" {
#endif

struct spring_t {
	double tension;
	double friction;

	double start;
	double stop;
	double position;
	double velocity;
	double pposition;
	double pvelocity;
	double tposition;
	double tvelocity;
	double accumulator;
};

void spring_init(struct spring_t * s, double start, double stop, double velocity, double tension, double friction);
int spring_step(struct spring_t * s, double dt);

static inline double spring_position(struct spring_t * s)
{
	return s->position;
}

static inline double spring_velocity(struct spring_t * s)
{
	return s->velocity;
}

#ifdef __cplusplus
}
#endif

#endif /* __SPRING_H__ */
