#ifndef TEXT_BUTTON_H
#define TEXT_BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "engine/context.h"
#include "engine/texture.h"

/** Hogy melyik irányba jelenjen meg az elem a koordinátához képest */
typedef enum Alignment
{
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	LEFT_CENTER,
	CENTER,
	RIGHT_CENTER,
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
} Alignment;

typedef struct TextButton TextButton;

/** Egy szöveg és gomb kezelő struktúra */
struct TextButton
{
	/** A gomb x koordinátája */
	double x;
	/** A gomb y koordinátája */
	double y;
	/** A gomb szélessége */
	double w;
	/** A gomb magassága */
	double h;
	/** A gomb önmagához viszonyított elhelyezkedése */
	Alignment button_alignment;
	/** A gomb ablakhoz viszonyított elhelyezkedése */
	Alignment window_alignment;

	/** A gomb textúrája */
	Texture *background;
	/** A textúra, ami lefedi a gombot, ha fölötte van az egér */
	Texture *overlay;
	/** A gomb szövegének a színe */
	SDL_Color text_color;
	/** A gomb szövegének a fontja */
	TTF_Font *text_font;
	/** A gomb szövege */
	char *text;
	/** A függvény, ami megmondja, hogy a gomb területén van-e egy pont */
	bool (*is_in_bounds)(Context *ctx, TextButton *btn, double x, double y);
	/** Hogy a gomb fölött van-e a gomb */
	bool hover;
};

/**
 * Létrehoz egy gombot
 * @param x a gomb x koordinátája
 * @param y a gomb y koordinátája
 * @param w a gomb szélessége
 * @param h a gomb magassága
 * @param window_alignment a gomb ablakhoz viszonyított elhelyezkedése
 * @param button_alignment a gomb önmagához viszonyított elhelyezkedése
 * @returns a létrehozott gomb pointere
 */
TextButton *create_button(double x, double y, double w, double h, Alignment window_alignment, Alignment button_alignment);

/**
 * Felszabadít egy gombot
 * @param btn a felszabadítandó gomb pointere
 */
void destroy_button(TextButton *btn);

/**
 * Beállítja egy gomb textúráit
 * @param btn a gomb pointere
 * @param background a gomb háttere, lehet @c NULL
 * @param overlay a textúra, ami lefedi a gombot, ha fölötte van az egér, lehet @c NULL
 */
void set_textures(TextButton *btn, Texture *background, Texture *overlay);

/**
 * Beállítja egy gomb szöveggel kapcsolatos adatait
 * @param btn a gomb pointere
 * @param text a gomb szögege
 * @param font a szöveg fontja
 * @param text_color a szöveg színe
 */
void set_text_data(TextButton *btn, char *text, TTF_Font *font, SDL_Color text_color);

/**
 * Frissíti a gombot
 * @param ctx a kontextus
 * @param btn a gomb
 */
void update_button(Context *ctx, TextButton *btn);

/**
 * Megjeleníti a gombot
 * @param ctx a kontextus
 * @param btn a gomb
 */
void draw_button(Context *ctx, TextButton *btn);
#endif
