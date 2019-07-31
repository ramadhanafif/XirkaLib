#include "XST_UART.h"
#define BAUDRATE 9600

int main(){
	XST_UART uart1(UART1);
	uart1.begin(BAUDRATE);
	
	uart1.print("Hallo");
	
	uart1.println(" dunia");
	uart1.sendc('.');
}