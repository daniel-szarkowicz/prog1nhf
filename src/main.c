#include "engine/debug.h"

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>

#include "engine/context.h"
#include "scenes/menu.h"

int main(void)
{
	Context *ctx = ctx_init(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, "Aknásh");
	if (ctx == NULL)
	{
		return 1;
	}
	TTF_Init();
	
	menu_load(ctx);
	
	ctx_eventloop(ctx);

	TTF_Quit();
	ctx_destroy(ctx);

	return 0;
}


