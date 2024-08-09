#include "engine/debug.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#include "engine/context.h"
#include "engine/text_button.h"
#include "hex_math.h"

#include "scenes/menu.h"
#include "scenes/game.h"
#include "scenes/new_game.h"
#include "map.h"


void menu_draw(Context *ctx)
{
	MenuData *data = (MenuData *)ctx->scene.data;
	SDL_SetRenderDrawColor(ctx->renderer, 15, 15, 15, 255);
	SDL_RenderClear(ctx->renderer);

	update_button(ctx, data->newgame_btn);
	update_button(ctx, data->continue_btn);
	update_button(ctx, data->exit_btn);

	draw_button(ctx, data->newgame_btn);
	draw_button(ctx, data->continue_btn);
	draw_button(ctx, data->exit_btn);
	if (ctx->mouse.clicked[SDL_BUTTON_LEFT])
	{
		if (data->newgame_btn->hover)
		{
			menu_unload(ctx);
			new_game_load(ctx);
		}
		else if (data->continue_btn->hover)
		{
			Map *map = map_from_file("savefile");
			if (map != NULL)
			{
				menu_unload(ctx);
				game_load(ctx);
				map_load_textures(map, ctx);
				((GameData *)ctx->scene.data)->map = map;
			}
		}
		else if (data->exit_btn->hover)
		{
			menu_unload(ctx);
			ctx->quit = true;
		}
	}
	SDL_RenderPresent(ctx->renderer);
}

void menu_quit(Context *ctx)
{
	DEBUG_PRINT("Quit signal recieved in menu scene");
	menu_unload(ctx);
	ctx->quit = true;
}

void menu_load(Context *ctx)
{
	DEBUG_PRINT("Loading menu scene");
	ctx->scene.draw = menu_draw;
	ctx->scene.quit = menu_quit;
	ctx->scene.raw_event = NULL;
	MenuData *data = ctx->scene.data = (MenuData *)malloc(sizeof(MenuData));
	SDL_Color text_color = {127, 127, 127, 255};

	data->button_background = load_texture(ctx, "res/button_background.png");
	data->button_overlay = load_texture(ctx, "res/overlay.png");
	data->button_font = TTF_OpenFont("res/font.ttf", 50);

	double button_height = 100;
	double button_width = data->button_background->width * button_height / data->button_background->height;

	data->newgame_btn = create_button(0, -120, button_width, button_height, CENTER, CENTER);
	data->newgame_btn->is_in_bounds = is_in_bounds;
	set_textures(data->newgame_btn, data->button_background, data->button_overlay);
	set_text_data(data->newgame_btn, "Új játék", data->button_font, text_color);

	data->continue_btn = create_button(0, 0, button_width, button_height, CENTER, CENTER);
	data->continue_btn->is_in_bounds = is_in_bounds;
	set_textures(data->continue_btn, data->button_background, data->button_overlay);
	set_text_data(data->continue_btn, "Folytatás", data->button_font, text_color);

	data->exit_btn = create_button(0, 120, button_width, button_height, CENTER, CENTER);
	data->exit_btn->is_in_bounds = is_in_bounds;
	set_textures(data->exit_btn, data->button_background, data->button_overlay);
	set_text_data(data->exit_btn, "Kilépés", data->button_font, text_color);
}

void menu_unload(Context *ctx)
{
	DEBUG_PRINT("Uloading menu scene");
	ctx->scene.draw = NULL;
	ctx->scene.quit = NULL;
	ctx->scene.raw_event = NULL;
	MenuData *data = (MenuData *)ctx->scene.data;

	destroy_button(data->newgame_btn);
	destroy_button(data->continue_btn);
	destroy_button(data->exit_btn);

	TTF_CloseFont(data->button_font);
	unload_texture(data->button_background);
	unload_texture(data->button_overlay);

	free(data);
	ctx->scene.data = NULL;
}
