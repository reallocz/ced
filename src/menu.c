#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include "window.h"
#include "input.h"
#include "log.h"

// Maximum number of options
#define MENU_OPTCAP 8
// Length of each option string
#define MENU_OPTLEN 64
// A maximum of 4 menus allowed
#define MENU_MENUCAP 4

struct menu_option {
	int id;
	char optstring[MENU_OPTLEN];
};

struct menu {
	int id;
	int flags;
	enum menu_type type;
	int optcount;
	struct menu_option options[MENU_OPTCAP];
};

static struct {
	const char* tag;
	unsigned int flags;
	unsigned int menucount;
	unsigned int menucap;
	struct menu menus[MENU_MENUCAP];
} G;


static struct menu* _get_menu(hMenu menu)
{
	if(menu >= G.menucap) {
		printf("ERROR: INVALID MENU: %d\n", menu);
		exit(1);
	}
	return &G.menus[menu];
}

int menu_init()
{
	G.tag = "MENU";
	G.flags = 0;
	G.menucount = 0;
	G.menucap = MENU_MENUCAP;
	log_l(G.tag, "Init success");
	return 0;
}

int menu_exit()
{
	return 0;
}


hMenu menu_create(enum menu_type type)
{
	struct menu* mu = _get_menu(G.menucount);
	mu->id = G.menucount;
	mu->flags = 0;
	mu->type = type;
	mu->optcount = 0;

	G.menucount++;
	return mu->id;
}

void menu_destroy(hMenu menu)
{
	// TODO
}


int menu_addopt(hMenu menu, int id, const char* optstring)
{
	struct menu* mu = _get_menu(menu);
	if(mu->optcount >= MENU_OPTCAP) {
		printf("MAX MENU CAP REACHED!\n");
		return 1;
	}
	int oid = mu->optcount;
	mu->options[oid].id = id;
	strncpy(mu->options[oid].optstring, optstring, MENU_OPTLEN);

	mu->optcount++;
	return 0;
}

// TODO
int menu_prompt(hMenu menu, hWindow win)
{
	struct menu * mu = _get_menu(menu);
	WINDOW* w = win_nwin(win);
	wclear(w);

	waddstr(w, "MENU: Please choose an option");
	int row = 1;
	for(int i = 0; i < mu->optcount; ++i) {
		mvwaddch(w, row++, 0, (char)i + 48);
		waddstr(w, " -> ");
		waddstr(w, mu->options[i].optstring);
	}
	wmove(w, row, 0);
	int res;
	inpev ev;
	while(1) {
	       ev = inp_winpoll(win);
	       if(ev.type == INSERT) {
			res = ev.data.ch;
			break;
	       }
	       if(ev.type == QUIT) {
		       res = 0;
		       break;
	       }
	}
	// Clear the screen as soon as we get a valid response
	wclear(w);
	return res;
}

void menu_pprint(hMenu menu)
{
	struct menu* mu = _get_menu(menu);
	printf("MENU:\n");
	for(int i = 0; i < mu->optcount; ++i) {
		printf("%d - %s\n", mu->options[i].id,
				mu->options[i].optstring);
	}
}
