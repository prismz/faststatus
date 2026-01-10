#include <string.h>

char *separator(void *s_voidptr)
{
        char *s = (char *)s_voidptr;

        return strdup(s);
}
