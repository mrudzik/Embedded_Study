//sprites.h

#pragma once

#include "ssd1306.h"


static const char *const d0[] = { "###", "#.#", "#.#", "#.#", "###" };
static const char *const d1[] = { ".#.", "##.", ".#.", ".#.", "###" };
static const char *const d2[] = { "###", "..#", "###", "#..", "###" };
static const char *const d3[] = { "###", "..#", "###", "..#", "###" };
static const char *const d4[] = { "#.#", "#.#", "###", "..#", "..#" };
static const char *const d5[] = { "###", "#..", "###", "..#", "###" };
static const char *const d6[] = { "###", "#..", "###", "#.#", "###" };
static const char *const d7[] = { "###", "..#", "..#", "..#", "..#" };
static const char *const d8[] = { "###", "#.#", "###", "#.#", "###" };
static const char *const d9[] = { "###", "#.#", "###", "..#", "###" };

static const sprite_t spr_digit[10] = {
    { d0, 3, 5 }, { d1, 3, 5 }, { d2, 3, 5 }, { d3, 3, 5 }, { d4, 3, 5 },
    { d5, 3, 5 }, { d6, 3, 5 }, { d7, 3, 5 }, { d8, 3, 5 }, { d9, 3, 5 },
};

static const char *const colon_rows[] = { "...", ".#.", "...", ".#.", "..." };
static const sprite_t spr_colon = { colon_rows, 3, 5 };

static const char *const minus_rows[] = { "...", "...", "###", "...", "..." };
static const sprite_t spr_minus = { minus_rows, 3, 5 };
