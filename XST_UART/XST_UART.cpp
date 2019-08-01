#include "XST_UART.h"

//PRIVATE Arguments
CMSDK_UART_TypeDef * UARTx;
uint16_t timeout;

//PUBLIC Funtction
XST_UART::XST_UART(CMSDK_UART_TypeDef * UART_nomer) { //constructor
  UARTx = UART_nomer;
  setTimeout();
}

void XST_UART::begin(uint32_t baudrate) {
  UARTx->BAUDDIV = (uint32_t) FOSC / baudrate;
  UARTx->CTRL    = 0x03;
  if (UARTx == CMSDK_UART0)
    CMSDK_GPIO1->ALTFUNCSET = 3 << 0; //enable RX and TX
  else if (UARTx == CMSDK_UART1)
    CMSDK_GPIO1->ALTFUNCSET = 3 << 2; //enable RX and TX
  else if (UARTx == CMSDK_UART2)
    CMSDK_GPIO1->ALTFUNCSET = 3 << 4; //enable RX and TX
};
unsigned char XST_UART::sendc ( char c)
{
  while ((UARTx->STATE & 1)); // Wait if Transmit Holding register is full
  UARTx->DATA = c;
  return (c);
};

void XST_UART::sendc (uint8_t c)
{
  sendc((char) c);
};

unsigned char XST_UART::getc(void) {
  while ((UARTx->STATE & 2) == 0); // Wait if Receive Holding register is empty
  return (UARTx->DATA);
};

unsigned char XST_UART::getcNow(void) {
  return (UARTx->DATA);
};

void XST_UART::print(char* string) {
	for (uint16_t i = 0; i< strlen(string); i++){
		sendc(string[i]);
	}
};

void XST_UART::print(int32_t num, uint8_t type) { //print a byte in 3 forms
	char result[32];
	if (type == HEX) {
		sprintf(result, "%X", num);
		print(result);
	}
  else if (type == DEC){
		sprintf(result, "%d", num);
		print(result);
  }
};

void XST_UART::println(char* string) {
  print(string);
  sendc('\n');
};


void XST_UART::println(int32_t c, uint8_t type) { //print a byte in 3 forms
	print(c, type);
	sendc('\n');
};

void XST_UART::EndSimulation(void) {
  sendc((char) 0x4); // End of transmission
  while (1);
};

bool XST_UART::RX_buf_full(void) {
  return (UARTx->STATE & 2) != 0;
};

bool XST_UART::TX_buf_full(void) {
  return (UARTx->STATE & 1) != 0;
}

void XST_UART::setTimeout(uint16_t time) {
  timeout = time;
}


//Belum berhasil
char* XST_UART::read(){ 
	char buf[READ_BUFF_SIZE];
	while (1){
	
	}
}

//char* XST_UART::readString(){
//}

//char* XST_UART::readStringUntil(){
//}
//long int XST_UART::parseInt(void){
//  getc()


//}