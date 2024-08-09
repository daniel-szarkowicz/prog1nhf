#include "engine/debug.h"

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "map.h"
#include "engine/context.h"
#include "scenes/game.h"
#include "hex_math.h"

static const int neighbors[6][2] = {
	{-1, 0},
	{-1, 1},
	{0, 1},
	{1, 0},
	{1, -1},
	{0, -1},
};

static bool set_mine(Map *m, int x, int y)
{
	tile *t = get_tile(m, x, y);
	if (t != NULL && t->enabled && !t->mine)
	{
		t->mine = true;
		for (int i = 0; i < 6; ++i)
		{
			int x1 = x + neighbors[i][0];
			int y1 = y + neighbors[i][1];
			tile *t1 = get_tile(m, x1, y1);
			if (t1 != NULL && t->enabled)
			{
				++t1->mines_around;
			}
		}
		return true;
	}
	return false;
}

Map *generate_map(int radius, int mine_count)
{
	DEBUG_PRINT("Generating map");
	Map *m = (Map *)malloc(sizeof(Map));
	if (m == NULL)
	{
		return NULL;
	}

	m->radius = radius;
	m->mine_count = mine_count;
	m->populated = false;
	m->enabled_count = 0;
	m->activated_count = 0;
	m->flagged_count = 0;
	m->data = (tile **)malloc(sizeof(tile *) * (2 * m->radius + 1));
	if (m->data == NULL)
	{
		free(m);
		return NULL;
	}
	for (int i = 0; i < (2 * m->radius + 1); ++i)
	{
		m->data[i] = (tile *)calloc(sizeof(tile), (2 * m->radius + 1));
		if (m->data[i] == NULL)
		{
			for (int j = 0; j < i; ++j)
			{
				free(m->data[j]);
			}
			free(m->data);
			free(m);
			return NULL;
		}
	}

	tile *t = get_tile(m, 0, 0);
	t->enabled = true;
	++m->enabled_count;
	for (int r = 1; r <= radius; ++r)
	{
		int x = r, y = -r;
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < r; ++j)
			{
				t = get_tile(m, x, y);
				t->enabled = true;
				++m->enabled_count;
				x += neighbors[i][0];
				y += neighbors[i][1];
			}
		}
	}
	return m;
}

void destroy_map(Map *m)
{
	DEBUG_PRINT("Destroying map");
	if (m != NULL)
	{
		for (int i = 0; i < (2 * m->radius + 1); ++i)
		{
			free(m->data[i]);
		}
		free(m->data);
		free(m);
	}
}

void populate_map(Map *m, int x, int y)
{
	DEBUG_PRINT("Populating map");
	srand(time(NULL));
	int mines = 0;
	m->populated = true;
	while (mines < m->mine_count)
	{
		int x1 = rand() % (2 * m->radius + 1) - m->radius;
		int y1 = rand() % (2 * m->radius + 1) - m->radius;
		bool good = true;
		if (x == x1 && y == y1)
			good = false;
		for (int i = 0; good && i < 6; ++i)
		{
			if (x + neighbors[i][0] == x1 && y + neighbors[i][1] == y1)
				good = false;
		}
		if (good && set_mine(m, x1, y1))
		{
			++mines;
		}
	}
}

void map_load_textures(Map *m, Context *ctx)
{
	DEBUG_PRINT("Loading map textures");
	m->hidden_texture = load_texture(ctx, "res/tile_hidden.png");
	m->visible_texture = load_texture(ctx, "res/tile_visible.png");
	m->mine_texture = load_texture(ctx, "res/mine.png");
	m->flag_texture = load_texture(ctx, "res/flag.png");
	TTF_Font *font = TTF_OpenFont("res/font.ttf", 100);
	SDL_Color text_color = {127, 127, 127, 255};
	for (int i = 0; i < 6; ++i)
	{
		char str[] = {'1' + i, '\0'};
		SDL_Surface *surface = TTF_RenderText_Blended(font, str, text_color);
		m->numbers[i] = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
	}
	TTF_CloseFont(font);
}

void map_unload_textures(Map *m)
{
	DEBUG_PRINT("Unloading map textures");
	unload_texture(m->hidden_texture);
	unload_texture(m->visible_texture);
	unload_texture(m->mine_texture);
	unload_texture(m->flag_texture);
	for (int i = 0; i < 6; ++i)
	{
		SDL_DestroyTexture(m->numbers[i]);
	}
}

void draw_map(Map *m, Context *ctx)
{
	int min_x, min_y, max_x, max_y;
	pixel_to_hex_centered(ctx, 0, ctx->window.height, 50, &min_x, &max_y);
	pixel_to_hex_centered(ctx, ctx->window.width, 0, 50, &max_x, &min_y);
	--min_x;
	// --min_y;
	++max_x;
	// ++max_y;
	for (int i = min_x; i <= max_x; ++i)
	{
		for (int j = min_y; j <= max_y; ++j)
		{
			draw_tile(m, ctx, i, j);
		}
	}
}

void draw_tile(Map *m, Context *ctx, int x, int y)
{
	GameData *data = (GameData *)ctx->scene.data;
	tile *t = get_tile(m, x, y);
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
		if (t->visible)
		{
			SDL_RenderCopyF(ctx->renderer, m->visible_texture->ptr, NULL, &dstrect);
			if (t->mine)
			{
				SDL_RenderCopyF(ctx->renderer, m->mine_texture->ptr, NULL, &dstrect);
			}
			else if (t->mines_around > 0)
			{
				SDL_Texture *texture = m->numbers[t->mines_around-1];
				int text_w, text_h;
				SDL_QueryTexture(texture, NULL, NULL, &text_w, &text_h);
				SDL_FRect text_rect = {0};
				text_rect.h = dstrect.h * 0.75;
				text_rect.w = text_w * text_rect.h / text_h;
				text_rect.x = dstrect.x + dstrect.w/2 - text_rect.w/2;
				text_rect.y = dstrect.y + dstrect.h/2 - text_rect.h/2;

				SDL_RenderCopyF(ctx->renderer, texture, NULL, &text_rect);
			}
		}
		else
			SDL_RenderCopyF(ctx->renderer, m->hidden_texture->ptr, NULL, &dstrect);

		if (t->flagged)
			SDL_RenderCopyF(ctx->renderer, m->flag_texture->ptr, NULL, &dstrect);

	}
}

tile *get_tile(Map *m, int x, int y)
{
	x += m->radius;
	y += m->radius;
	if (0 <= x && x < (2 * m->radius + 1) && 0 <= y && y < (2 * m->radius + 1))
	{
		return &m->data[x][y];
	}
	return NULL;
}

void activate_tile(Map *m, int x, int y)
{
	tile *t = get_tile(m, x, y);
	if (t != NULL && t->enabled && !t->visible && !t->flagged)
	{
		t->visible = true;
		++m->activated_count;
		if (t->mines_around == 0)
		{
			for (int i = 0; i < 6; ++i)
			{
				int x1 = x + neighbors[i][0];
				int y1 = y + neighbors[i][1];
				tile *t1 = get_tile(m, x1, y1);
				if (t1 != NULL && !t1->visible)
				{
					activate_tile(m, x1, y1);
				}
			}
		}
	}
}

void show_mines(Map *m)
{
	for (int i = -m->radius; i <= m->radius; ++i)
	{
		for (int j = -m->radius; j <= m->radius; ++j)
		{
			tile *t = get_tile(m, i, j);
			if (t != NULL && t->enabled && t->mine)
			{
				t->visible = true;
			}
		}
	}
}

void set_tile_flagged(Map *m, int x, int y, bool flagged)
{
	tile *t = get_tile(m, x, y);
	if (t != NULL && !t->visible && t->flagged != flagged)
	{
		m->flagged_count += flagged ? 1 : -1;
		t->flagged = flagged;
	}
}

Map *map_from_file(const char *filename)
{
	DEBUG_PRINT("Loading map from file: %s", filename);
	FILE *f = fopen(filename, "rt");
	if (f == NULL)
	{
		DEBUG_PRINT("Failed to open file: %s", filename);
		return NULL;
	}
	int radius, mine_count, populated;
	fscanf(f, "%d %d %d", &radius, &mine_count, &populated);
	Map *m = generate_map(radius, mine_count);
	if (m == NULL)
		return NULL;
	m->mine_count = mine_count;
	m->populated = populated;
	for (int i = 0; i < (2 * radius + 1); ++i)
	{
		for (int j = 0; j < (2 * radius + 1); ++j)
		{
			int x = i - m->radius;
			int y = j - m->radius;
			int data;
			fscanf(f, "%d", &data);
			if ((data >> 2) & 1)
				set_mine(m, x, y);
			set_tile_flagged(m, x, y, (data >> 1) & 1);
			tile *t = get_tile(m, x, y);
			if ((data >> 0) & 1)
			{
				t->visible = true;
				++m->activated_count;
			}
		}
	}
	fclose(f);
	DEBUG_PRINT("Finished loading map");
	return m;
}

void map_to_file(Map *m, const char *filename)
{
	DEBUG_PRINT("Saving map to file: %s", filename);
	FILE *f = fopen(filename, "wt");
	if (f == NULL)
	{
		DEBUG_PRINT("Failed to open file for writing: %s", filename);
		return;
	}
	fprintf(f, "%d %d %d\n", m->radius, m->mine_count, m->populated);
	for (int i = 0; i < (2 * m->radius + 1); ++i)
	{
		for (int j = 0; j < (2 * m->radius + 1); ++j)
		{
			tile *t = get_tile(m, i - m->radius, j - m->radius);
			int data = 0;
			data |= t->mine << 2;
			data |= t->flagged << 1;
			data |= t->visible << 0;
			fprintf(f, "%d ", data);
		}
		fprintf(f, "\n");
	}
	fclose(f);
	DEBUG_PRINT("Finished saving map");
}
