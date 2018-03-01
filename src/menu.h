#pragma once
#include "window.h"
/** menu.h
 * A menu is a prompt buffer used to select a specific option
 */

typedef unsigned int hMenu;

enum menu_type {
	QUICK,
	INTERACTIVE
};

int menu_init();
int menu_exit();

hMenu menu_create(enum menu_type type);
void menu_destroy(hMenu menu);

/** Add an option to the menu */
int menu_addopt(hMenu menu, int optid, const char* optstring);

/** Displays the options and returns the selected option */
int menu_prompt(hMenu mu, hWindow win);

void menu_pprint(hMenu menu);
