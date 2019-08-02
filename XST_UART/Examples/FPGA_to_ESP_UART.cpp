#include "XST_UART.h"
#include "lcd_xirka.h"
#include <string.h>

XST_UART Serial(UART0);
int main(void)
{
	Serial.begin(9600);
	init();
	char buf[3];
	Serial.print("{\"card_id\":\"1101016\",\"nim\":\"1121213\",\"name\":\"TestESP\",\"instansi\":\"Xirka\"}");
	
	//Baca 3 digit response dari server
	for (int i=0;i<3;i++){
		buf[i]=Serial.getC();
	}
	
	//201-> Card Added, 200-> OK
	if(strcmp(buf,"201")|strcmp(buf,"200")){
		LCD_WriteString("Card Added!");
	}
	//400-> Error
	else{
		LCD_WriteString("Adding card failed!");
	}
}