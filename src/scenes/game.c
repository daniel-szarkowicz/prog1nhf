#include "engine/debug.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

#include "scenes/game.h"
#include "engine/context.h"
#include "hex_math.h"
#include "scenes/menu.h"
#include "engine/text_button.h"
#include "engine/texture.h"

static void save_and_quit(Context *ctx)
{
	DEBUG_PRINT("Quitting game");
	GameData *data = (GameData *)ctx->scene.data;
	if (data->state == PLAYING)
	{
		map_to_file(data->map, "savefile");
	}
	else
	{
		remove("savefile");
	}
	destroy_map(data->map);
}

void game_draw(Context *ctx)
{
	GameData *data = (GameData *)ctx->scene.data;
	SDL_SetRenderDrawColor(ctx->renderer, 15, 15, 15, 255);
	SDL_RenderClear(ctx->renderer);
	update_button(ctx, data->exit_btn);
	update_button(ctx, data->flag_counter);
	draw_map(data->map, ctx);
	if (!data->exit_btn->hover && !data->flag_counter->hover)
	{
		if (ctx->mouse.drag[SDL_BUTTON_LEFT])
		{
			data->camera.x -= ctx->mouse.xrel;
			data->camera.y -= ctx->mouse.yrel;
		}
		if (ctx->mouse.clicked[SDL_BUTTON_MIDDLE])
		{
			data->camera.x = 0;
			data->camera.y = 0;
			data->camera.zoom = 1;
		}
		if (ctx->mouse.wheel.y > 0 && data->camera.zoom < 2.0)
		{
			data->camera.zoom *= data->camera.zoom_speed;
			data->camera.x += (data->camera.x + ctx->mouse.x - ctx->window.width/2)*(data->camera.zoom_speed - 1);
			data->camera.y += (data->camera.y + ctx->mouse.y - ctx->window.height/2)*(data->camera.zoom_speed - 1);
		}
		else if (ctx->mouse.wheel.y < 0 && data->camera.zoom > 0.2)
		{
			data->camera.zoom /= data->camera.zoom_speed;
			data->camera.x += (data->camera.x + ctx->mouse.x - ctx->window.width/2)/(data->camera.zoom_speed/(1 - data->camera.zoom_speed));
			data->camera.y += (data->camera.y + ctx->mouse.y - ctx->window.height/2)/(data->camera.zoom_speed/(1 - data->camera.zoom_speed));
		}
		switch(data->state)
		{
			case PLAYING:;
				int x, y;
				pixel_to_hex_centered(ctx, ctx->mouse.x, ctx->mouse.y, 50, &x, &y);
				tile *t = get_tile(data->map, x, y);
				if (t != NULL && t->enabled)
				{
					double px, py;
					double tile_size = 50;
					hex_to_pixel(x, y, tile_size, &px, &py);
					px *= data->camera.zoom;
					py *= data->camera.zoom;
					tile_size *= 1.025 * data->camera.zoom;
					SDL_FRect dstrect = {
						px - tile_size + ctx->window.width / 2 - data->camera.x,
						py - tile_size + ctx->window.height / 2 - data->camera.y,
						2*tile_size,
						2*tile_size
					};
					SDL_RenderCopyF(ctx->renderer, data->tile_overlay->ptr, NULL, &dstrect);
					if (ctx->mouse.clicked[SDL_BUTTON_LEFT])
					{
						if (!data->map->populated)
						{
							populate_map(data->map, x, y);
						}
						if (!t->flagged)
						{
							if (t->mine)
							{
								show_mines(data->map);
								DEBUG_PRINT("You lose");
								data->state = DEFEAT;
							}
							else
							{
								activate_tile(data->map, x, y);
								if (data->map->enabled_count - data->map->activated_count == data->map->mine_count)
								{
									DEBUG_PRINT("You win");
									data->state = VICTORY;
								}
							}
						}
					}
					if (ctx->mouse.clicked[SDL_BUTTON_RIGHT])
					{
						set_tile_flagged(data->map, x, y, !t->flagged);
					}
				}
				sprintf(data->flag_counter->text, "%d/%d", data->map->flagged_count, data->map->mine_count);
				break;
			case VICTORY:
				sprintf(data->flag_counter->text, "Nyertél :)");
				break;
			case DEFEAT:
				sprintf(data->flag_counter->text, "Vesztettél");
				break;
			default:
				break;
		}

	}
	draw_button(ctx, data->exit_btn);
	draw_button(ctx, data->flag_counter);
	if (ctx->mouse.clicked[SDL_BUTTON_LEFT] && data->exit_btn->hover)
	{
		game_unload(ctx);
		menu_load(ctx);
	}
	SDL_RenderPresent(ctx->renderer);
}

void game_quit(Context *ctx)
{
	DEBUG_PRINT("Quit signal recieved in game scene");
	game_unload(ctx);
	ctx->quit = true;
}

void game_load(Context *ctx)
{
	DEBUG_PRINT("Loading game scene");
	ctx->scene.draw = game_draw;
	ctx->scene.quit = game_quit;
	ctx->scene.raw_event = NULL;
	GameData *data = ctx->scene.data = (GameData *)malloc(sizeof(GameData));
	data->tile_overlay = load_texture(ctx, "res/tile_overlay.png");
	data->map = NULL;
	data->camera = (Camera){0, 0, 1, 1.1};
	SDL_Color text_color = {127, 127, 127, 255};

	data->button_background = load_texture(ctx, "res/button_background.png");
	data->button_overlay = load_texture(ctx, "res/overlay.png");
	data->font = TTF_OpenFont("res/font.ttf", 50);

	double button_height = 100;
	double button_width = data->button_background->width * button_height / data->button_background->height;

	data->exit_btn = create_button(10, 10, button_width, button_height, TOP_LEFT, TOP_LEFT);
	data->exit_btn->is_in_bounds = is_in_bounds;
	set_textures(data->exit_btn, data->button_background, data->button_overlay);
	set_text_data(data->exit_btn, "Kilépés", data->font, text_color);

	data->flag_counter = create_button(-10, 10, button_width, button_height, TOP_RIGHT, TOP_RIGHT);
	data->flag_counter->is_in_bounds = is_in_bounds;
	set_textures(data->flag_counter, data->button_background, NULL);
	set_text_data(data->flag_counter, malloc(20), data->font, text_color);

	data->state = PLAYING;
}

void game_unload(Context *ctx)
{
	DEBUG_PRINT("Unloading game scene");
	ctx->scene.draw = NULL;
	ctx->scene.quit = NULL;
	ctx->scene.raw_event = NULL;
	GameData *data = ctx->scene.data;

	destroy_button(data->exit_btn);

	free(data->flag_counter->text);
	destroy_button(data->flag_counter);

	unload_texture(data->tile_overlay);
	unload_texture(data->button_background);
	unload_texture(data->button_overlay);

	map_unload_textures(data->map);
	save_and_quit(ctx);

	TTF_CloseFont(data->font);

	free(data);
	ctx->scene.data = NULL;
}
