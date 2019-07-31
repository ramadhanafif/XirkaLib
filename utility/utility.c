#include "utility.h"

void delaymilis(uint32_t milis) {
  uint32_t i, r;
	milis = (uint32_t) milis/3;
  for (i = 0; i < milis; i++) {
    r = 1330;
    while (r--);
  }
}

int second(int n)                                                                    //delay function declaration
{
  int r;

  r = 1330000;
  while (r--);
  return (n + 1);
}


void delaymicros(uint32_t n)                                                                    //delay function declaration
{
  int i, r;

  for (i = 0; i < n; i++) {
    r = 2;
    while (r--);
  }//for
}

void delay(uint32_t n){
	delaymilis(n);
}