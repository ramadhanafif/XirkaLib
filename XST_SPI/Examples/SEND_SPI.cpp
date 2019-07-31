#include "XST_SPI.h"

int main() {
  XST_SPI spi;
  spi.begin();
  //dummy start byte
  spi.transfer(0);
  char c;
  for (int i = 0; i < 3; i++) {
    for (const char * p = "DATA SPI\n" ; c = *p; p++) {
      delaymicros(1);
      spi.transfer(c);
    }
  }
}
