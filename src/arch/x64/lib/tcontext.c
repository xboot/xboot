#include "context.h"

static void demo_platform_context_test_func1(struct transfer_t from)
{
    fcontext_t* contexts = (fcontext_t*)from.priv;

    contexts[0] = from.context;
    from.context = contexts[2];

    size_t count = 10;
	while (count--)
    {
        printf("func1: %lu\r\n", count);
        from = jump_fcontext(from.context, contexts);
    }
	jump_fcontext(contexts[0], NULL);
}
static void demo_platform_context_test_func2(struct transfer_t from)
{
    fcontext_t* contexts = (fcontext_t*)from.priv;

    size_t count = 10;
	while (count--)
    {
        printf("func2: %lu\r\n", count);
        from = jump_fcontext(from.context, contexts);
    }
	jump_fcontext(contexts[0], NULL);
}
static void demo_platform_context_test()
{
    static fcontext_t contexts[3];
	static char stacks1[8192];
	static char stacks2[8192];

    contexts[1] = make_fcontext(stacks1, sizeof(stacks1), demo_platform_context_test_func1);
    contexts[2] = make_fcontext(stacks2, sizeof(stacks2), demo_platform_context_test_func2);

    jump_fcontext(contexts[1], contexts);
}

int demo_platform_context_main(int argc, char * argv[])
{
    demo_platform_context_test();
    return 0;
}
