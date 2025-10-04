#ifndef PLATAFORM_H
#define PLATAFORM_H

#include <sf.h>

b32 plataform_init(i32 w, i32 h);
void plataform_deinit(void);
b32 plataform_poll_events(void);

#endif
