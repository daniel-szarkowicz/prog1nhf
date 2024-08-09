#include "engine/debug.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include "engine/text_button.h"
#include "engine/context.h"
#include "engine/texture.h"


TextButton *create_button(double x, double y, double w, double h, Alignment window_alignment, Alignment button_alignment)
{
	DEBUG_PRINT("Creating new TextButton");
	TextButton *btn = (TextButton *)malloc(sizeof(TextButton));
	btn->x = x;
	btn->y = y;
	btn->h = h;
	btn->w = w;
	btn->window_alignment = window_alignment;
	btn->button_alignment = button_alignment;
	btn->background = NULL;
	btn->overlay = NULL;
	btn->text_color = (SDL_Color){0};
	btn->text_font = NULL;
	btn->text = NULL;
	btn->is_in_bounds = NULL;
	btn->hover = false;
	return btn;
}

void set_textures(TextButton *btn, Texture *background, Texture *overlay)
{
	DEBUG_PRINT("Setting textures");
	btn->background = background;
	btn->overlay = overlay;
}

void set_text_data(TextButton *btn, char *text, TTF_Font *font, SDL_Color color)
{
	DEBUG_PRINT("Setting text data");
	btn->text = text;
	btn->text_font = font;
	btn->text_color = color;
}

void destroy_button(TextButton *btn)
{
	DEBUG_PRINT("Destroying Button");
	free(btn);
}

void update_button(Context *ctx, TextButton *btn)
{
	if (btn->is_in_bounds != NULL)
	{
		btn->hover = btn->is_in_bounds(ctx, btn, ctx->mouse.x, ctx->mouse.y);
	}
}

void draw_button(Context *ctx, TextButton *btn)
{
	SDL_FRect dstrect = {
		btn->x + (ctx->window.width * (btn->window_alignment % 3) / 2) - (btn->w * (btn->button_alignment % 3) / 2),
		btn->y + (ctx->window.height * (btn->window_alignment / 3) / 2) - (btn->h * (btn->button_alignment / 3) / 2),
		btn->w,
		btn->h
	};

	//	SDL_FRect dstrect = {btn->x + ctx->window.width/2,
	//						 btn->y + ctx->window.height/2,
	//						 btn->w,
	//						 btn->h};

	if (btn->background != NULL)
	{
		SDL_RenderCopyF(ctx->renderer, btn->background->ptr, NULL, &dstrect);
	}

	if (btn->text != NULL && btn->text_font != NULL)
	{
		SDL_Surface *surface = TTF_RenderUTF8_Blended(btn->text_font, btn->text, btn->text_color);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		int text_w, text_h;
		SDL_QueryTexture(texture, NULL, NULL, &text_w, &text_h);
		SDL_FRect text_rect = {0};
		text_rect.h = dstrect.h * 0.6;
		text_rect.w = text_w * text_rect.h / text_h;
		text_rect.x = dstrect.x + dstrect.w/2 - text_rect.w/2;
		text_rect.y = dstrect.y + dstrect.h/2 - text_rect.h/2;

		SDL_RenderCopyF(ctx->renderer, texture, NULL, &text_rect);

		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}

	if (btn->overlay != NULL && btn->hover)
	{
		SDL_RenderCopyF(ctx->renderer, btn->overlay->ptr, NULL, &dstrect);
	}
}
