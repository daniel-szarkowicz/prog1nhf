#include "engine/debug.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "hex_math.h"
#include "engine/context.h"
#include "scenes/game.h"
#include "engine/texture.h"

static const double sqrt3 = 1.7320508075688772;

// hatszög forrás: https://www.redblobgames.com/grids/hexagons/
void hex_to_pixel(int x, int y, double size, double *p_x, double *p_y)
{
	*p_x = (1.5 * x) * size;
	*p_y = (x / 2.0 + y) * sqrt3 * size;
}

void pixel_to_hex(int p_x, int p_y, double size, int *x, int *y)
{
	double hex_x = (2.0 / 3.0 * p_x) / size;
	double hex_y = (-1.0 / 3.0 * p_x + sqrt3 / 3.0 * p_y) / size;

	int rx = SDL_round(hex_x);
	int ry = SDL_round(hex_y);
	int rz = SDL_round(-hex_x - hex_y);

	double x_diff = SDL_fabs(rx - hex_x);
	double y_diff = SDL_fabs(ry - hex_y);
	double z_diff = SDL_fabs(rz + hex_x + hex_y);

	if (x_diff > y_diff && x_diff > z_diff)
		rx = -ry - rz;
	else if (y_diff > z_diff)
		ry = -rx - rz;

	if (x != NULL)
		*x = rx;
	if (y != NULL)
		*y = ry;
}

void pixel_to_hex_centered(Context *ctx, int p_x, int p_y, double size, int *x, int *y)
{
	Camera *camera = &((GameData *)ctx->scene.data)->camera;
	pixel_to_hex((p_x - ctx->window.width / 2 + camera->x) / camera->zoom, (p_y - ctx->window.height / 2 + camera->y) / camera->zoom, size, x, y);
}

// "hexagonal" button
bool is_in_bounds(Context *ctx, TextButton *btn, double x, double y)
{
	SDL_FRect bounds = {btn->x + (ctx->window.width * (btn->window_alignment % 3) / 2) - (btn->w * (btn->button_alignment % 3) / 2) + btn->w / 2,
		btn->y + (ctx->window.height * (btn->window_alignment / 3) / 2) - (btn->h * (btn->button_alignment / 3) / 2) + btn->h / 2,
		btn->w,
		btn->h};
//	SDL_FRect bounds = {btn->x + ctx->window.width / 2 + btn->w / 2,
//						 btn->y + ctx->window.height / 2 + btn->h / 2,
//						 btn->w,
//						 btn->h};

	if (bounds.y - bounds.h * SDL_sqrt(3) / 4 <= y && y <= bounds.y + bounds.h * SDL_sqrt(3) / 4)
	{
		double dist = bounds.w / 2 - SDL_fabs(bounds.y - y) / SDL_sqrt(3);
		return bounds.x - dist <= x && x <= bounds.x + dist;
	}
	return false;
}
