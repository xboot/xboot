#ifndef __SEQLOCK_H__
#define __SEQLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <barrier.h>
#include <spinlock.h>

typedef struct {
	unsigned int sequence;
	spinlock_t lock;
} seqlock_t;

static inline void seqlock_init(seqlock_t * sl)
{
	sl->sequence = 0;
	spin_lock_init(&sl->lock);
}

static inline unsigned int read_seqbegin(const seqlock_t * sl)
{
	unsigned ret;

repeat:
	ret = sl->sequence;
	smp_rmb();
	if(ret & 1)
		goto repeat;
	return ret;
}

static inline unsigned int read_seqretry(const seqlock_t * sl, unsigned int start)
{
	smp_rmb();
	return (sl->sequence != start);
}

static inline void write_seqlock(seqlock_t * sl)
{
	spin_lock(&sl->lock);
	++sl->sequence;
	smp_wmb();
}

static inline void write_sequnlock(seqlock_t * sl)
{
	smp_wmb();
	sl->sequence++;
	spin_unlock(&sl->lock);
}

static inline int write_tryseqlock(seqlock_t * sl)
{
	int ret = spin_trylock(&sl->lock);

	if(ret)
	{
		++sl->sequence;
		smp_wmb();
	}
	return ret;
}

#define read_seqbegin_irqsave(lock, flags)			({ local_irq_save(flags); read_seqbegin(lock); })
#define read_seqretry_irqrestore(lock, iv, flags)	({ int ret = read_seqretry(lock, iv); local_irq_restore(flags); ret; })
#define write_seqlock_irqsave(lock, flags)			do { local_irq_save(flags); write_seqlock(lock); } while (0)
#define write_sequnlock_irqrestore(lock, flags)		do { write_sequnlock(lock); local_irq_restore(flags); } while(0)
#define write_seqlock_irq(lock)						do { local_irq_disable(); write_seqlock(lock); } while (0)
#define write_sequnlock_irq(lock)					do { write_sequnlock(lock); local_irq_enable(); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __SEQLOCK_H__ */
