#ifndef HELPERS_H
#define HELPERS_H

#include "engine/context.h"
#include "engine/text_button.h"

/**
 * Átvált egy hatszöges koordinátát pixel koordinárába
 * @param x a hatszöges x koordináta
 * @param y a hatszöges y koordináta
 * @param size a hatszögek oldalhossza
 * @param p_x az x pixelkoordináta memóriacíme
 * @param p_y az y pixelkoordináta memóriacíme
 */
void hex_to_pixel(int x, int y, double size, double *p_x, double *p_y);

/**
 * Átvált egy pixelkoordinátát hatszöges koordinárába
 * @param x az x pixelkoordináta
 * @param y az y pixelkoordináta
 * @param size a hatszögek oldalhossza
 * @param p_x a hatszöges x koordináta memóriacíme
 * @param p_y a hatszöges y koordináta memóriacíme
 */
void pixel_to_hex(int x, int y, double size, int *p_x, int *p_y);

void pixel_to_hex_centered(Context *ctx, int x, int y, double size, int *p_x, int *p_y);

/**
 * Megvizsgálja, hogy egy koordináta egy gombon van-e
 * @param ctx a kontextus
 * @param btn a gomb
 * @param x az x koordináta
 * @param y az y koordináta
 * @returns hogy rajta van-e a koordináta a gombon
 */
bool is_in_bounds(Context *ctx, TextButton *btn, double x, double y);
#endif /* !HELPERS_H */
