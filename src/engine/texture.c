#include "engine/debug.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "engine/texture.h"

Texture *load_texture(Context *ctx, const char *filename)
{
	DEBUG_PRINT("Loading texture: %s", filename);
	Texture *texture = (Texture *)malloc(sizeof(Texture));
	texture->ptr = IMG_LoadTexture(ctx->renderer, filename);
	SDL_QueryTexture(texture->ptr, NULL, NULL, &texture->width, &texture->height);
	return texture;
}

void unload_texture(Texture *texture)
{
	DEBUG_PRINT("Destroying Texture");
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture->ptr);
		free(texture);
	}
}
