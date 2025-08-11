#include "util.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>

/* from extern in elements.h */
// Display *dpy;

int valid_layout_or_variant(char *sym)
{
	size_t i;
	/* invalid symbols from xkb rules config */
	static const char *invalid[] = { "evdev", "inet", "pc", "base" };

	for (i = 0; i < LEN(invalid); i++) {
		if (!strncmp(sym, invalid[i], strlen(invalid[i]))) {
			return 0;
		}
	}

	return 1;
}

char *get_layout(char *syms, int grp_num)
{
	char *tok, *layout;
	int grp;

	layout = NULL;
	tok = strtok(syms, "+:");
	for (grp = 0; tok && grp <= grp_num; tok = strtok(NULL, "+:")) {
		if (!valid_layout_or_variant(tok)) {
			continue;
		} else if (strlen(tok) == 1 && isdigit(tok[0])) {
			/* ignore :2, :3, :4 (additional layout groups) */
			continue;
		}
		layout = tok;
		grp++;
	}


	return layout;
}

char *keymap(void)
{
        char *buf = safe_malloc(sizeof(char) * 512);
        strncpy(buf, "NULL", 512);

        XkbDescRec *desc;
        XkbStateRec state;
        char *symbols;
        strncpy(buf, "NULL", 512);

        //if (!(dpy = XOpenDisplay(NULL)))
        //        goto end;

        if (!(desc = XkbAllocKeyboard()))
                goto end;

        if (XkbGetNames(dpy, XkbSymbolsNameMask, desc))
                goto end;

        if (XkbGetState(dpy, XkbUseCoreKbd, &state))
                goto end;

	if (!(symbols = XGetAtomName(dpy, desc->names->symbols)))
                goto end;

        strncpy(buf, get_layout(symbols, state.group),
                        512);

end:
	XFree(symbols);
        XkbFreeKeyboard(desc, XkbSymbolsNameMask, 1);
        // XCloseDisplay(dpy);

        return buf;
}
