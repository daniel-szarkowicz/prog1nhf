#include "engine/debug.h"

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "engine/context.h"

static Uint32 frame_timer(Uint32 interval, void *param)
{
	SDL_Event ev;
	ev.type = DRAW_EVENT;
	SDL_PushEvent(&ev);
	return interval;
}

Context *ctx_init(int win_x, int win_y, int win_w, int win_h, const char *title)
{
	DEBUG_PRINT("Creating Context");
	DEBUG_PRINT("Initializing SDL");
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		DEBUG_PRINT("Failed to initialize SDL: %s", SDL_GetError());
		SDL_Quit();
		return NULL;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	DEBUG_PRINT("Creating Window");
	SDL_Window *window = SDL_CreateWindow(title, win_x, win_y, win_w, win_h, 0);
	if (window == NULL)
	{
		DEBUG_PRINT("Failed to create window: %s", SDL_GetError());
		SDL_Quit();
		return NULL;
	}

	DEBUG_PRINT("Creating Renderer");
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		DEBUG_PRINT("Failed to create hardware accelerated renderer: %s", SDL_GetError());
		DEBUG_PRINT("Using software fallback instead");
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL)
		{
			DEBUG_PRINT("Failed to create renderer: %s", SDL_GetError());
			SDL_DestroyWindow(window);
			SDL_Quit();
			return NULL;
		}
	}

	Context *ctx = (Context *)malloc(sizeof(Context));
	if (ctx == NULL)
	{
		DEBUG_PRINT("Failed to create context, out of memory");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return NULL;
	}
	SDL_TimerID id = SDL_AddTimer(1000/60, frame_timer, ctx);
	ctx->frame_timer_id = id;
	ctx->mouse = (Mouse){0};
	ctx->mouse.drag_threshold = 5;
	ctx->window = (Window){win_x, win_y, win_w, win_h, window};
	ctx->renderer = renderer;
	ctx->scene = (Scene){0};
	ctx->quit = false;
	SDL_RenderClear(renderer);
	DEBUG_PRINT("Finished creating Context");
	return ctx;
}

void ctx_destroy(Context *ctx)
{
	DEBUG_PRINT("Destroying Context");
	if (ctx != NULL)
	{
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window.ptr);
		SDL_RemoveTimer(ctx->frame_timer_id);
		SDL_Quit();
		free(ctx);
	}
}

void ctx_eventloop(Context *ctx)
{
	DEBUG_PRINT("Starting event loop");
	SDL_Event event;
	while (!ctx->quit)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
			case SDL_QUIT:
				ctx->quit = true;
				if (ctx->scene.quit)
					ctx->scene.quit(ctx);
			case SDL_MOUSEMOTION:
				ctx->mouse.x = event.motion.x;
				ctx->mouse.y = event.motion.y;
				ctx->mouse.xrel = ctx->mouse.x - ctx->mouse.px;
				ctx->mouse.yrel = ctx->mouse.y - ctx->mouse.py;
				for (int i = 0; i < 6; ++i)
				{
					if (ctx->mouse.pressed[i] && ctx->mouse.xrel * ctx->mouse.xrel + ctx->mouse.yrel * ctx->mouse.yrel > ctx->mouse.drag_threshold * ctx->mouse.drag_threshold)
						ctx->mouse.drag[i] = true;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				ctx->mouse.pressed[event.button.button] = true;
				ctx->mouse.just_pressed[event.button.button] = true;
				break;
			case SDL_MOUSEBUTTONUP:
				ctx->mouse.pressed[event.button.button] = false;
				if (!ctx->mouse.drag[event.button.button])
					ctx->mouse.clicked[event.button.button] = true;
				ctx->mouse.drag[event.button.button] = false;
				break;
			case SDL_MOUSEWHEEL:
				ctx->mouse.wheel.x += event.wheel.x;
				ctx->mouse.wheel.y += event.wheel.y;
				ctx->mouse.wheel.direction = event.wheel.direction;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_SIZE_CHANGED:
					case SDL_WINDOWEVENT_RESIZED:
						ctx->window.width = event.window.data1;
						ctx->window.height = event.window.data2;
						break;
					case SDL_WINDOWEVENT_MOVED:
						ctx->mouse.px -= event.window.data1 - ctx->window.x;
						ctx->mouse.py -= event.window.data2 - ctx->window.y;
						ctx->mouse.x -= event.window.data1 - ctx->window.x;
						ctx->mouse.y -= event.window.data2 - ctx->window.y;
						ctx->window.x = event.window.data1;
						ctx->window.y = event.window.data2;
						break;
					default:
						break;
				}
			case DRAW_EVENT:
				if (ctx->scene.draw)
				{
					ctx_predraw(ctx);
					ctx->scene.draw(ctx);
					ctx_postdraw(ctx);
				}
				break;
			default:
				if (ctx->scene.raw_event)
					ctx->scene.raw_event(ctx, event);
				break;
		}
	}
	DEBUG_PRINT("Stopping event loop");
}

void ctx_predraw(Context *ctx)
{
	ctx->mouse.xrel = ctx->mouse.x - ctx->mouse.px;
	ctx->mouse.yrel = ctx->mouse.y - ctx->mouse.py;
}

void ctx_postdraw(Context *ctx)
{
	ctx->mouse.px = ctx->mouse.x;
	ctx->mouse.py = ctx->mouse.y;
	ctx->mouse.wheel.x = 0;
	ctx->mouse.wheel.y = 0;
	for (int i = 0; i < 6; ++i)
	{
		ctx->mouse.just_pressed[i] = false;
		ctx->mouse.clicked[i] = false;
	}
}
