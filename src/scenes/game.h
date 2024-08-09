#ifndef SCENES_GAME_H
#define SCENES_GAME_H

#include <stdbool.h>
#include "engine/context.h"
#include "map.h"
#include "engine/texture.h"
#include "engine/text_button.h"

/** A játék állapota */
typedef enum GameState
{
	PLAYING,
	VICTORY,
	DEFEAT
} GameState;

/** A játék nézet kamerája */
typedef struct Camera
{
	/** A kamera x koordinátája */
	double x;
	/** A kamera y koordinátája */
	double y;
	/** A kamera nagyítása */
	double zoom;
	/** A kamera nagyítási sebessége */
	double zoom_speed;
} Camera;

/** A játék nézet adatai */
typedef struct GameData
{
	/** A gombok háttere */
	Texture *button_background;
	/** A textúra, ami lefedi gombokat, ha fölöttük van az egér */
	Texture *button_overlay;
	/** A gomok fontja */
	TTF_Font *font;
	/** A textúra ami lefedi az egér alatti mezőt */
	Texture *tile_overlay;
	/** A kamera */
	Camera camera;
	/** A kilépés gomb */
	TextButton *exit_btn;
	/** A jelöléseket mutató számláló */
	TextButton *flag_counter;
	/** A játék pályája */
	Map *map;
	/** Hogy milyen állapotban van a játék */
	GameState state;
} GameData;

/** A játék nézet megjelenítője */
void game_draw(Context *ctx);

/** A játék kilépés kezelője */
void game_quit(Context *ctx);

/** Elkezdi a játék nézetet */
void game_load(Context *ctx);

/** Befejezi a játék nézetet */
void game_unload(Context *ctx);

#endif /* !SCENES_GAME_H */
