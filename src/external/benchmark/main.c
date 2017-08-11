#include <benchmark.h>
#include <pmu.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    benchmark [args ...]\r\n");
}

static int do_benchmark(int argc, char ** argv)
{
	struct profiler_t * p1, * p2;

	profiler_reset();

	profiler_snap("L1DCACHE_ACCESS", L1DCACHE_ACCESS, 0);
	profiler_snap("L1DCACHE_MISS", L1DCACHE_MISS, 1);
	profiler_snap("L2DCACHE_ACCESS", L2DCACHE_ACCESS, 2);
	profiler_snap("L2DCACHE_MISS", L2DCACHE_MISS, 3);
	benchmark("memcpy");
	profiler_snap("L1DCACHE_ACCESS", L1DCACHE_ACCESS, 0);
	profiler_snap("L1DCACHE_MISS", L1DCACHE_MISS, 1);
	profiler_snap("L2DCACHE_ACCESS", L2DCACHE_ACCESS, 2);
	profiler_snap("L2DCACHE_MISS", L2DCACHE_MISS, 3);

	profiler_dump();
	p1 = profiler_search("L1DCACHE_MISS");
	p2 = profiler_search("L1DCACHE_ACCESS");
	if(p2->end != p2->begin)
		printf("L1DCACHE miss = %f%%\r\n", (p1->end - p1->begin) * 100.0 / (p2->end - p2->begin));

	p1 = profiler_search("L2DCACHE_MISS");
	p2 = profiler_search("L2DCACHE_ACCESS");
	if(p2->end != p2->begin)
	printf("L2DCACHE miss = %f%%\r\n", (p1->end - p1->begin) * 100.0 / (p2->end - p2->begin));

	return 0;
}

static struct command_t cmd_benchmark = {
	.name	= "benchmark",
	.desc	= "benchmark test command",
	.usage	= usage,
	.exec	= do_benchmark,
};

static __init void benchmark_cmd_init(void)
{
	register_command(&cmd_benchmark);
}

static __exit void benchmark_cmd_exit(void)
{
	unregister_command(&cmd_benchmark);
}

command_initcall(benchmark_cmd_init);
command_exitcall(benchmark_cmd_exit);
