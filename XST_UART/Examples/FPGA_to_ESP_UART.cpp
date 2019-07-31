//#include "XST_UART.h"
//#include "lcd_xirka.h"
//#include <string.h>

//XST_UART Serial(UART1);

//int main(void) {
//  Serial.begin(9600);
//  int i = 0;
//  Serial.println("UART1 test read");
//  char buf[10];
////    LCDinit();

//	//Serial.sendc('G');
//  while (1) {
//		char str1[4] = "123";
//		int xx = stoi(str1);
//		Serial.print(xx);
//	}
//		
//}

#include "XST_UART.h"

#define BAUDRATE 9600
#define MAX_BYTES 100
XST_UART UART_ESP(UART0);

int main(void)
{
	UART_ESP.begin(BAUDRATE);
	
	char data[MAX_BYTES];
	strcpy(data, "{\"card_id\": 1234576,\"nim\": 1321010,\"name\": \"Test1\",\"instansi\": \"Xirka1\"}");
	
	//Kirim data ke ESP melalui UART0
	UART_ESP.print(data);
}


	
