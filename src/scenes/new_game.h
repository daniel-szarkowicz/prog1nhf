#ifndef SCENES_NEW_GAME_H
#define SCENES_NEW_GAME_H

#include "engine/context.h"
#include "engine/text_button.h"

/** Az új játék nézetnek az adatai */
typedef struct NewGameData
{
	/** A létrehozandó tábla sugara */
	int radius;
	/** Az ankák száma az új játékban */
	int mine_count;
	/** A gombok háttere */
	Texture *button_background;
	/** A textúre ami lefedi a gombokat, ha fölöttük van az egér */
	Texture *button_overlay;
	/** A kicsi gombok háttere */
	Texture *small_background;
	/** A textúre ami lefedi a kicsi gombokat, ha fölöttük van az egér */
	Texture *small_overlay;
	/** A gombok fontja */
	TTF_Font *button_font;
	/** A vissza gomb */
	TextButton *back_button;
	/** A kezdés gomb */
	TextButton *start_button;
	/** A pályaméret növelő gomb */
	TextButton *map_plus_button;
	/** A pályaméret csökkentő gomb */
	TextButton *map_minus_button;
	/** Az aknák számát növelő gomb */
	TextButton *mine_plus_button;
	/** Az aknák számát csökkentő gomb */
	TextButton *mine_minus_button;
	/** A pálya méretét mutató elem */
	TextButton *radius_display;
	/** Az aknák számát mutató elem */
	TextButton *mine_count_display;
	/** Az akna ikon */
	TextButton *mine_icon;
	/** A pálya ikon */
	TextButton *map_icon;
} NewGameData;

/**
 * Megjeleníti az új játék nézetet
 * @param ctx a kontextus
 */
void new_game_draw(Context *ctx);

/**
 * Az új játék nézet kilépés kezelője
 * @param ctx a kontextus
 */
void new_game_quit(Context *ctx);

/**
 * Elkezdi az új játék nézetet
 * @param ctx a kontextus
 */
void new_game_load(Context *ctx);

/**
 * Befejezi az új játék nézetet
 * @param ctx a kontextus
 */
void new_game_unload(Context *ctx);

#endif /* !SCENES_NEW_GAME_H */
