#ifndef SCENES_MENU_H
#define SCENES_MENU_H

#include <stdbool.h>
#include "engine/context.h"
#include "engine/text_button.h"

/** A menü nézetnek az adatai */
typedef struct MenuData
{
	/** A gombok háttere */
	Texture *button_background;
	/** A textúra ami lefedi a gombokat, ha fölöttük van az egér */
	Texture *button_overlay;
	/** A gombok fontja */
	TTF_Font *button_font;
	/** Az új játék gomb */
	TextButton *newgame_btn;
	/** A folytatás gomb */
	TextButton *continue_btn;
	/** A kilépés gomb */
	TextButton *exit_btn;
} MenuData;

/** A menü nézet megjelenítés kezelője */
void menu_draw(Context *ctx);

/** A menü nézet kilépés kezelője */
void menu_quit(Context *ctx);

/** Elkezdi a menü nézetet */
void menu_load(Context *ctx);

/** Befejezi a menü nézetet */
void menu_unload(Context *ctx);


#endif /* !SCENES_MENU_H */
