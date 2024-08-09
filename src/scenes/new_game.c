#include "engine/debug.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include "scenes/new_game.h"
#include "scenes/menu.h"
#include "scenes/game.h"
#include "engine/context.h"
#include "map.h"
#include "engine/text_button.h"
#include "hex_math.h"

static void change_data(NewGameData *data, int radius_change, int mine_count_change)
{
	data->radius += radius_change;
	if (data->radius < 3)
		data->radius = 3;
	if (data->radius > 100)
		data->radius = 100;
	data->mine_count += mine_count_change;
	if ((3*data->radius*(data->radius+1)+1) / 2 < data->mine_count)
		data->mine_count = (3*data->radius*(data->radius+1)+1) / 2;
	else if ((3*data->radius*(data->radius+1)+1) / 8 > data->mine_count)
		data->mine_count = (3*data->radius*(data->radius+1)+1) / 8;
	sprintf(data->radius_display->text, "%d", data->radius);
	sprintf(data->mine_count_display->text, "%d", data->mine_count);
}

void new_game_draw(Context *ctx)
{
	SDL_RenderClear(ctx->renderer);
	NewGameData *data = (NewGameData *)ctx->scene.data;
	update_button(ctx, data->back_button);
	update_button(ctx, data->start_button);
	update_button(ctx, data->map_plus_button);
	update_button(ctx, data->map_minus_button);
	update_button(ctx, data->mine_plus_button);
	update_button(ctx, data->mine_minus_button);

	draw_button(ctx, data->back_button);
	draw_button(ctx, data->start_button);
	draw_button(ctx, data->map_plus_button);
	draw_button(ctx, data->map_minus_button);
	draw_button(ctx, data->mine_plus_button);
	draw_button(ctx, data->mine_minus_button);

	draw_button(ctx, data->radius_display);
	draw_button(ctx, data->mine_count_display);
	draw_button(ctx, data->mine_icon);
	draw_button(ctx, data->map_icon);

	if (ctx->mouse.clicked[SDL_BUTTON_LEFT])
	{
		if (data->back_button->hover)
		{
			new_game_unload(ctx);
			menu_load(ctx);
		}
		else if (data->start_button->hover)
		{
			Map *map = generate_map(data->radius, data->mine_count);
			new_game_unload(ctx);
			if (map == NULL)
			{
				ctx->quit = true;
				return;
			}
			map_load_textures(map, ctx);
			game_load(ctx);
			((GameData *)ctx->scene.data)->map = map;

		}
		else if (data->map_plus_button->hover)
			change_data(data, 1, 0);
		else if (data->map_minus_button->hover)
			change_data(data, -1, 0);
		else if (data->mine_plus_button->hover)
			change_data(data, 0, 1);
		else if (data->mine_minus_button->hover)
			change_data(data, 0, -1);
	}
	else if (ctx->mouse.clicked[SDL_BUTTON_RIGHT])
	{
		if (data->map_plus_button->hover)
			change_data(data, 10, 0);
		else if (data->map_minus_button->hover)
			change_data(data, -10, 0);
		else if (data->mine_plus_button->hover)
			change_data(data, 0, 10);
		else if (data->mine_minus_button->hover)
			change_data(data, 0, -10);
	}
	else if (ctx->mouse.clicked[SDL_BUTTON_MIDDLE])
	{
		if (data->mine_plus_button->hover)
			change_data(data, 0, 100);
		else if (data->mine_minus_button->hover)
			change_data(data, 0, -100);
	}
	SDL_RenderPresent(ctx->renderer);
}

void new_game_quit(Context *ctx)
{
	DEBUG_PRINT("Quit signal recieved in new_game scene");
	new_game_unload(ctx);
	ctx->quit = true;
}

void new_game_load(Context *ctx)
{
	DEBUG_PRINT("Loading new_game scene");
	ctx->scene.draw = new_game_draw;
	ctx->scene.quit = new_game_quit;
	ctx->scene.raw_event = NULL;
	NewGameData *data = ctx->scene.data = (NewGameData *)malloc(sizeof(NewGameData));
	data->radius = 10;
	data->mine_count = 50;
	SDL_Color text_color = {127, 127, 127, 255};

	data->button_background = load_texture(ctx, "res/button_background.png");
	data->button_overlay = load_texture(ctx, "res/overlay.png");
	data->small_background = load_texture(ctx, "res/tile_visible.png");
	data->small_overlay = load_texture(ctx, "res/tile_overlay.png");
	data->button_font = TTF_OpenFont("res/font.ttf", 50);

	double button_height = 100;
	double button_width = data->button_background->width * button_height / data->button_background->height;
	double small_height = 100;
	double small_width = data->small_background->width * small_height / data->small_background->height;

	data->back_button = create_button(10, -10, button_width, button_height, BOTTOM_LEFT, BOTTOM_LEFT);
	data->back_button->is_in_bounds = is_in_bounds;
	set_textures(data->back_button, data->button_background, data->button_overlay);
	set_text_data(data->back_button, "Vissza", data->button_font, text_color);

	data->start_button = create_button(-10, -10, button_width, button_height, BOTTOM_RIGHT, BOTTOM_RIGHT);
	data->start_button->is_in_bounds = is_in_bounds;
	set_textures(data->start_button, data->button_background, data->button_overlay);
	set_text_data(data->start_button, "Kezdés", data->button_font, text_color);


	data->map_plus_button = create_button(150, -50, small_width, small_height, CENTER, LEFT_CENTER);
	data->map_plus_button->is_in_bounds = is_in_bounds;
	set_textures(data->map_plus_button, data->small_background, data->small_overlay);
	set_text_data(data->map_plus_button, "+", data->button_font, text_color);

	data->map_minus_button = create_button(-150, -50, small_width, small_height, CENTER, RIGHT_CENTER);
	data->map_minus_button->is_in_bounds = is_in_bounds;
	set_textures(data->map_minus_button, data->small_background, data->small_overlay);
	set_text_data(data->map_minus_button, "-", data->button_font, text_color);


	data->mine_plus_button = create_button(150, 50, small_width, small_height, CENTER, LEFT_CENTER);
	data->mine_plus_button->is_in_bounds = is_in_bounds;
	set_textures(data->mine_plus_button, data->small_background, data->small_overlay);
	set_text_data(data->mine_plus_button, "+", data->button_font, text_color);

	data->mine_minus_button = create_button(-150, 50, small_width, small_height, CENTER, RIGHT_CENTER);
	data->mine_minus_button->is_in_bounds = is_in_bounds;
	set_textures(data->mine_minus_button, data->small_background, data->small_overlay);
	set_text_data(data->mine_minus_button, "-", data->button_font, text_color);


	data->radius_display = create_button(100, -50, button_width, button_height, CENTER, RIGHT_CENTER);
	set_text_data(data->radius_display, malloc(10), data->button_font, text_color);
	sprintf(data->radius_display->text, "%d", data->radius);

	data->mine_count_display = create_button(100, 50, button_width, button_height, CENTER, RIGHT_CENTER);
	set_text_data(data->mine_count_display, malloc(10), data->button_font, text_color);
	sprintf(data->mine_count_display->text, "%d", data->mine_count);

	data->mine_icon = create_button(100, 50, small_width, small_height, CENTER, RIGHT_CENTER);
	set_textures(data->mine_icon, load_texture(ctx, "res/mine.png"), NULL);

	data->map_icon = create_button(100, -50, small_width, small_height, CENTER, RIGHT_CENTER);
	set_textures(data->map_icon, load_texture(ctx, "res/map_icon.png"), NULL);
}

void new_game_unload(Context *ctx)
{
	DEBUG_PRINT("Unloading new_game scene");
	ctx->scene.draw = NULL;
	ctx->scene.quit = NULL;
	ctx->scene.raw_event = NULL;
	NewGameData *data = (NewGameData *)ctx->scene.data;

	destroy_button(data->back_button);
	destroy_button(data->start_button);
	destroy_button(data->map_plus_button);
	destroy_button(data->map_minus_button);
	destroy_button(data->mine_plus_button);
	destroy_button(data->mine_minus_button);

	unload_texture(data->mine_icon->background);
	destroy_button(data->mine_icon);

	unload_texture(data->map_icon->background);
	destroy_button(data->map_icon);

	free(data->radius_display->text);
	free(data->mine_count_display->text);
	destroy_button(data->radius_display);
	destroy_button(data->mine_count_display);

	unload_texture(data->button_background);
	unload_texture(data->button_overlay);

	unload_texture(data->small_background);
	unload_texture(data->small_overlay);

	TTF_CloseFont(data->button_font);
	free(data);
	ctx->scene.draw = NULL;
	ctx->scene.data = NULL;
}
