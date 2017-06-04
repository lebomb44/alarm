#ifndef _STRING_
#define _STRING_

#include "global.h"
#include "fifo.h"

void string_getLine(FIFO_T* fifo, u08* line, u16 n, u16 timeout);
u08 string_ncmp_P(u08* s1, const u08* s2, u16 n);
u08 string_len_P(const u08* s1);
void string_ncat(u08* src, u08* dst, u16 size);
void string_ncat_P(const u08* src, u08* dst, u16 size);
u16 string2u16(u08* s, u08 n);
void u32_2string(u08 type, u32 data, u08* s);

#endif

