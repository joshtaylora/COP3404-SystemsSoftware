#include <stdio.h>

int main()
{
  int timer = 0x813128; // will be stored in hex
  int hps = 3600; // saved in decimal since we use WORD
  int mps = 60;
  int saved, hours, mins, format;
  //int zasc = 0x30; // ascii for zero
  int hcmp = 3599;
  // STA SAVED
  saved = timer;
  // CMP ZERO
  if (saved < 0)
  {
    saved = saved << 1;
    saved = saved >> 1; // DIV TWO -> STA SAVED 
    format = 1;
  }
  else
  {
    format = 0;
  }
  hours = saved;
  while (hours > hcmp)
  {
    printf("%d\n", hours);
    hours -= hps;
  }
  return 0;
}
