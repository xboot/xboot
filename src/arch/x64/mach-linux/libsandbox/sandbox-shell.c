#include <x.h>
#include <sandbox.h>

struct sandbox_shell_context_t {
	pthread_t thread;
	const char * cmd;
	char * msg;
	int sz;
};

static void sandbox_shell_exec(const char * cmd, char * msg, int sz)
{
	char * p, buf[2048];
	int l, n, t;
	FILE * fp;

	fp = popen(cmd, "r");
	if(fp)
	{
		p = msg;
		n = sz;
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			if(msg && (sz > 0))
			{
				if(n <= 1)
				{
					p = msg;
					n = sz;
				}
				l = strlen(buf);
				t = (l >= n) ? n - 1 : l;
				memcpy(p, buf, t);
				p[t] = '\0';
				p += t;
				n -= t;
			}
		}
		pclose(fp);
	}
}

static void * sandbox_shell_thread(void * arg)
{
	struct sandbox_shell_context_t * ctx = (struct sandbox_shell_context_t *)arg;

	pthread_detach(pthread_self());
	if(ctx)
	{
		sandbox_shell_exec(ctx->cmd, ctx->msg, ctx->sz);
		free(ctx);
	}
	return NULL;
}

void sandbox_shell(const char * cmd, char * msg, int sz, int async)
{
	struct sandbox_shell_context_t * ctx;

	if(cmd)
	{
		if(async)
		{
			ctx = malloc(sizeof(struct sandbox_shell_context_t));
			if(ctx)
			{
				ctx->cmd = cmd;
				ctx->msg = msg;
				ctx->sz = sz;
				if(pthread_create(&ctx->thread, NULL, sandbox_shell_thread, ctx) != 0)
					free(ctx);
			}
		}
		else
		{
			sandbox_shell_exec(cmd, msg, sz);
		}
	}
}
