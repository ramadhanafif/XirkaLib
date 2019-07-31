#include "XST_GPIO.h"
#include "utility.h"
#include "lcd_xirka.h"

#define BLINKTEST
#define READTEST

XST_GPIO x(GPIO0);
XST_GPIO y(GPIO1);

void blinkled(int pin, XST_GPIO gpio){
	while(1){
		gpio.digitalWrite(pin,HIGH);
		delay(100);
		gpio.digitalWrite(pin,LOW);
		delay(100);
	}
}


int main(){
#ifdef BLINKTEST
	y.pinMode(3,OUTPUT);
	blinkled(3,y);
#endif

#ifdef READTEST	
	int pin = 0;
	LCD_init();
	x.pinMode(pin,INPUT);
	while(1){
		if(x.digitalRead(pin)){
			LCD_WriteString("IN");
		}
	}
#endif
}