#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct Wheel Wheel;
typedef struct Mouse Mouse;
typedef struct Window Window;
typedef struct Scene Scene;
typedef struct Context Context;

/** Saját esemény(ek) */
enum CustomEvent
{
	/** A megjelenítés esemény */
	DRAW_EVENT = SDL_USEREVENT
};

/** Az egér görgőjének az adatait tároló struktúra */
struct Wheel
{
	/** A görgetés vízszintes sebessége */
	int x;
	/** A görgetés függőleges sebessége */
	int y;
	/** @see SDL_MouseWheelDirection */
	SDL_MouseWheelDirection direction;
};

/** Az egér adatait tároló struktúra */
struct Mouse
{
	/** Az egér x koordinátája */
	int x;
	/** Az egér y koordinátája */
	int y;
	/** Az egér előző x koordinátája */
	int px;
	/** Az egér előző y koordinátája */
	int py;
	/** Az egér x sebessége */
	int xrel;
	/** Az egér y sebessége */
	int yrel;
	/** Hogy az egér adott gombja le van-e nyomva */
	bool pressed[6];
	/** Hogy ebben a pillanatban nyomódott-e le az adott gomb */
	bool just_pressed[6];
	/** Hogy húzták-e az adott gombot */
	bool drag[6];
	/** A minimális sebesség, amivel mozgatni kell az egeret, hogy azt húzásnak érzékelje */
	int drag_threshold;
	/** Hogy kattintottak-e (nem húzták és elengedték) az adott gombbal */
	bool clicked[6];
	/** Az egér görgőjének az adatai */
	Wheel wheel;
};

/** Az ablak adatait tároló struktúra */
struct Window
{
	/** Az ablak x pozíciója */
	int x;
	/** Az ablak y pozíciója */
	int y;
	/** Az ablak szélessége */
	int width;
	/** Az ablak magassága */
	int height;
	/** Az ablak SDL2-es pointere */
	SDL_Window *ptr;
};

/**
 * A nézet adatait tároló struktúra
 * @example Főmenü, Játék
 */
struct Scene
{
	/** A nézet megjelenítés esemény kezelője */
	void (*draw)(Context *ctx);
	/**
	 * A nézet kilépés esemény kezelője
	 * Ez az eseménykezelő meg tudja akadályozni a leállást, ha a @c ctx->quit -et @c false -ra állítja
	 */
	void (*quit)(Context *ctx);
	/** A nézet általános eseménykezelője */
	void (*raw_event)(Context *ctx, SDL_Event event);
	/** Az esemény egyéb adataira mutató pointer */
	void *data;
};

/** Ez tárol minden fontos adatot a játékkal kapcsolatban */
struct Context
{
	/** Az egér adatait tároló struktúra */
	Mouse mouse;
	/** Az ablak adatait tároló struktúra */
	Window window;
	/** A renderer pointere */
	SDL_Renderer *renderer;
	/** A nézet adatait tároló struktúra */
	Scene scene;
	/** A képernyő frissítő időzítő */
	SDL_TimerID frame_timer_id;
	/** A kontextus kilépés jelzője */
	bool quit;
};

/**
 * Létrehoz egy új kontextust @param win_x az ablak x koordinátája
 * @param win_y az ablak y koordinátája
 * @param win_w az ablak szélessége
 * @param win_h az ablak magassága
 * @returns NULL pointer ha hiba volt, a létrejött kontextus mutatója
 */
Context *ctx_init(int win_x, int win_y, int win_w, int win_h, const char *title);

/**
 * Felszabadít egy kontextust
 * @param ctx a felszabadítandó kontextus pointere
 */
void ctx_destroy(Context *ctx);

/**
 * Elkezdi az eseményciklust
 * @param ctx a kontextus pointere
 */
void ctx_eventloop(Context *ctx);

/** Előkészíti a nézet megjelenítést */
void ctx_predraw(Context *ctx);

/** Lezárja a nézet megjelenítését */
void ctx_postdraw(Context *ctx);

#endif /* !ENGINE_CONTEXT_H */
