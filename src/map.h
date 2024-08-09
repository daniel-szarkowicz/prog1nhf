#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "engine/context.h"
#include "engine/texture.h"

/** Egy mező adatait tároló struktúra */
typedef struct tile
{
	/** A szomszédos aknák száma */
	int mines_around;
	/** Hogy a mező akna-e */
	bool mine;
	/** Hogy a mező meg van-e jelölve */
	bool flagged;
	/** Hogy a mező fel van-e fedve */
	bool visible;
	/** Hogy a mező engedélyezve van-e */
	bool enabled;
} tile;

/** A tábla adatait tároló struktúra */
typedef struct Map
{
	/** A tábla suraga */
	int radius;
	/** Az aknák száma a táblán */
	int mine_count;
	/** Az engedélyezett mezők száma a táblán */
	int enabled_count;
	/** A megjelölt mezők száma */
	int flagged_count;
	/** A megjelenített mezők száma a táblán */
	int activated_count;
	/** Hogy el lettek-e már helyezve a mezők a táblán */
	bool populated;
	/** A mezőket tároló kétdimenziós tömb */
	tile **data;
	/** A nem felfedett mező textúrája */
	Texture *hidden_texture;
	/** A felfedett mező textúrája */
	Texture *visible_texture;
	/** Az akna textúrája */
	Texture *mine_texture;
	/** A zászló textúrája */
	Texture *flag_texture;
	/** A számok textúrái */
	SDL_Texture *numbers[6];
} Map;

/**
 * Létrehoz egy táblát
 * @param radius a tábla suraga
 * @param mine_count az aknák száma
 * @returns a letrehozott tábla pointere, vagy NULL ha hiba volt
 */
Map *generate_map(int radius, int mine_count);

/**
 * Felszabadít egy táblát
 * @param m a felszabadítandó tábla pointere
 */
void destroy_map(Map *m);

/**
 * Elhelyezi az aknákat a táblán
 * @param m a tábla pointere
 * @param x annak a mezőnek az x koordinátája, ami köré ne helyezzen mezőket
 * @param y annak a mezőnek az y koordinátája, ami köré ne helyezzen mezőket
 */
void populate_map(Map *m, int x, int y);

/**
 * Betölti a tábla textúráit
 * @param m a tábla pointere
 * @param ctx a kontextus
 */
void map_load_textures(Map *m, Context *ctx);

/**
 * Felszabadítja egy tábla textúráit
 * @param m a tábla aminek a textúráit fel kell szabadítani
 */
void map_unload_textures(Map *m);

/**
 * Megjeleníti a táblát
 * @param m a tábla pointere
 * @param ctx a kontextus pointere
 */
void draw_map(Map *m, Context *ctx);

/**
 * Megjelenít egy mezőt
 * @param m a tábla pointere
 * @param ctx a kontextus pointere
 * @param x a mező x koordinátája
 * @param y a mező y koordinátája
 */
void draw_tile(Map *m, Context *ctx, int x, int y);

/**
 * Visszaadja a tábla x, y koordinátán elhelyezkedő mezőt
 * @param m a tábla pointere
 * @param x a mező x koordinátája
 * @param y a mező y koordinátája
 * @returns a mező pointere, ami az adott koordinátákon található, vagy NULL ha a táblán kívül van
 */
tile *get_tile(Map *m, int x, int y);

/**
 * Felfed egy mezőt
 * Ha a mezőt 0 akna veszi körül akkor rekurzióval felfedi a szomszédjait is
 * @param m a tábla pointere
 * @param x a felfedentő mező x koordinátája
 * @param y a felfedentő mező y koordinátája
 */
void activate_tile(Map *m, int x, int y);

/**
 * Felfedi az összes nem megjelölt aknát a táblán
 * @param m a tábla pointere
 */
void show_mines(Map *m);

/**
 * Mejelöl egy mezőt
 * @param m a tábla pointere
 * @param x a mező x koordinátája
 * @param y a mező y koordinátája
 * @param flagged hogy be vagy ki jelölje a mezőt
 */
void set_tile_flagged(Map *m, int x, int y, bool flagged);

/**
 * Beolvas egy táblát az adott fájénévről
 * @param filename a fájl neve
 * @returns a létrejött tábla pointere, vagy NULL ha hiba volt
 */
Map *map_from_file(const char *filename);

/**
 * Elment egy táblát egy fájlba
 * @param m a tábla pointere
 * @param filename a fájl neve
 */
void map_to_file(Map *m, const char *filename);

#endif /* !MAP_H */
