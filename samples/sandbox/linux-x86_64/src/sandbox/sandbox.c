#include <system.h>
#include <sandbox/sandbox.h>

struct sandbox_t * sandbox_alloc(void)
{
	struct sandbox_t * sandbox;
	
	system_init();

	sandbox = malloc(sizeof(struct sandbox_t));
	if(!sandbox)
		return NULL;
	
	sandbox->display = display_alloc();
	sandbox->input = input_alloc();
	
	if(!sandbox->display || !sandbox->input)
	{
		if(sandbox->display)
			display_free(sandbox->display);
		
		if(sandbox->input)
			input_free(sandbox->input);
		
		free(sandbox);
	}
	
	return sandbox;
}

void sandbox_free(struct sandbox_t * sandbox)
{
	if(!sandbox)
		return;
	
	if(sandbox->display)
		display_free(sandbox->display);
	
	if(sandbox->input)
		input_free(sandbox->input);
	
	free(sandbox);

	system_exit();
}
