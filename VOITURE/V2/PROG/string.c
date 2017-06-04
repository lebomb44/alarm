#include "global.h"
#include "string.h"
#include "fifo.h"
#include "timer.h"
#include "uart0.h"

void string_getLine(FIFO_T* fifo, u08* line, u16 n, u16 timeout)
{
  /* n should be greater or equal to 2 */
  /* timeout should be greater than 0 */
  u16 data='\n';
  u16 index=0;

  /* Garbage CR and LF until timeout */
  while(((data==0xFFFF) || (data == '\n') || (data == '\r')) && (timeout > 0))
  {
    data = fifo_get(fifo);
    if(data == 0xFFFF) { timeout--; delay_ms(1); }
  }
  if(timeout == 0) { line[0]= '\0'; DEBUG0_PUTS_P((const u08*)PSTR("GET Line Timeout1\n")); return; }

  /* Get usable Char until timeout */
  while(((data==0xFFFF) || ((data != '\n') && (data != '\r'))) && (timeout > 0) && (index+1<n))
  {
    if(data == 0xFFFF) { timeout--; delay_ms(1); }
    else
    {
      line[index] = (u08) data;
      index++;
    }
    data = fifo_get(fifo);
  }
  if(timeout == 0) { line[0] = '\0'; DEBUG0_PUTS_P((const u08*)PSTR("GET Line Timeout2\n")); return; }
  line[index] = '\0';
}

u08 string_ncmp_P(u08* s1, const u08* s2, u16 n)
{
  while(n)
  { /* Compare string until end of one of them. Can compare with shorter string */
    if ((*s1 != pgm_read_byte(s2)) || (*s1 == 0) || (pgm_read_byte(s2) == 0)) { return 0; }
    s1++; s2++;
    n--;
  }
  return (1);
}

u08 string_len_P(const u08* s1)
{
  u08 n=0;
  while(pgm_read_byte(s1))
  {
    if(n<255) { n++; } else { return 255; }
    s1++;
  }
  return n;
}

void string_ncat(u08* src, u08* dst, u16 size)
{
  /* size should be greater or equal to 2 */
  /* Find end of destination string */
  while((*dst != '\0') && (size>0)) { dst++; size--; }
  /* Copy until end of source string or max size */
  while((*src != '\0') && (size>0)) { (*dst) = (*src); dst++; src++; size--; }
  /* Don t forget end of line */
  *dst = '\0';
}

void string_ncat_P(const u08* src, u08* dst, u16 size)
{
  /* size should be greater or equal to 2 */
  u08* ptr=0;
  /* Find end of destination string */
  while((*dst != '\0') && (size>1)) { dst++; size--; }
  ptr=(u08*) src;
  /* Copy until end of source string or max size */
  while((pgm_read_byte(ptr) != '\0') && (size>1)) { (*dst) = pgm_read_byte((const u08*) ptr); dst++; ptr++; size--; }
  /* Don t forget end of line */
  *dst = '\0';
}

u16 string2u16(u08* s, u08 n)
{
  u16 ret=0;
  while(n)
  {
    /* Check if it is a usable char */
    if(((*s) < '0') || ((*s) > '9')) { return ret; }
    /* Check if it is possible to continue */
    if(ret>6553) { return ret; }
    ret = ret * 10;
    ret = ret + ((*s) - '0');
    s++;
    n--;
  }
  return ret;
}

void u32_2string(u08 type, u32 data, u08* s)
{
  u08 i=8;
  u32 div_val=0;
  static const unsigned char hex[] = "0123456789ABCDEF";
  switch (type)
  {
    case 'b':
      i=32;
      do { i--; if(bit_is_set(data,i)) { (*s)='1'; s++; } else { (*s)='0'; s++; } } while(i);
      break;
    case 'c':
      (*s)=(u08)data;
      break;
    case 'd':
      div_val = 1000000000;
      while (div_val > 1 && div_val > data) { div_val /= DEC; }
      do { (*s)=(hex[data / div_val]); s++; data %= div_val; div_val /= DEC; }
      while (div_val);
      break;
    case 'x':
      div_val = 0x10000000;
      do { (*s)=(hex[data / div_val]); s++; data %= div_val; div_val /= HEX; }
      while (div_val);
      break;
    default:
      (*s) = (type); s++;
      break;
  }
  (*s)='\0';
}
