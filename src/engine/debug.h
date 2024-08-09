/**
 * @file Bogártalanításhoz hasznos eszközök
 * hehe
 */
#ifndef ENGINE_DEBUG_H
#define ENGINE_DEBUG_H

#ifdef DEBUG
#define _XOPEN_SOURCE
#include "debugmalloc.h"
#include <stdio.h>

/**
 * Kiír egy szöveget formázható szöveget debug módban
 * Nem csinál semmit normál módban
 */
#define DEBUG_PRINT(...) \
{\
	printf("[DEBUG] ");\
	printf(__VA_ARGS__);\
	printf("\n");\
}
#else
#define DEBUG_PRINT(...) ((void)0)
#endif

#endif /* ENGINE_DEBUG_H */
