#include <stdio.h>
#include <stdlib.h>
#include "err_handlers.h"


void
f_handle_err_and(e_level dec, const char *msg)
{
    printf("%s\n", msg);
    abort();
}


void
abort_with(const char *msg)
{
    printf("%s\n", msg);
    abort();
}
