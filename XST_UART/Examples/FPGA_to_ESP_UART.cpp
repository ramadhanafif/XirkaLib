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


	
