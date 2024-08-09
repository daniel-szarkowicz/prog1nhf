#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include "engine/context.h"

/** Egy textúra adatait tároló struktúra */
typedef struct Texture
{
	/** A textúra szélessége */
	int width;
	/** A textúra magassága */
	int height;
	/** A textúra pointere */
	SDL_Texture *ptr;
} Texture;

/**
 * Betölt egy textúrát
 * @param ctx a kontextus
 * @param filename a fájl neve
 * @returns az új textúra pointere
 */
Texture *load_texture(Context *ctx, const char *filename);

/**
 * Felszabadít egy textúrát
 * @param texture a felszabadítandó textúra pointere
 */
void unload_texture(Texture *texture);

#endif
