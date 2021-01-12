#pragma once



typedef enum{
    E_ABORT,
    E_SILENT_ABORT,
    E_CONTINUE,
    E_LET_USER_DECIDE

}e_level;


void
f_handle_err_and(e_level dec, const char *msg);

void
abort_with(const char *msg);
