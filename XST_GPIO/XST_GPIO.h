#ifndef XST_UART_H
#define XST_UART_H

#include "CMSDK_CM3.h"
#include <stdio.h> //untuk konversi int, float, ke String
#include <string.h>
/************************************************************
  Definisi tambahan untuk memudahkan
  Sudah terdefinisi di CMSDK_CM3.h
************************************************************/

#define UART0 CMSDK_UART0
#define UART1 CMSDK_UART1
#define UART2 CMSDK_UART2

#define DEC 0
#define HEX 1
#define BIN 2

#define READ_BUFF_SIZE 50

#define FOSC 20000000UL

class XST_UART {
  public:
    XST_UART(CMSDK_UART_TypeDef * UART_nomer);//Constructor. Assign UART number here.

    void begin(uint32_t baudrate);  //enable RX-TX UART,

    unsigned char sendc (char c); //send 1 byte of data
    void sendc (uint8_t c); //sendc with uint8_t input
    unsigned char getc(void); //receive 1 byte of data
    unsigned char getcNow(void); //read data from RX register without checking data availability

    void print(char* string); //send string of char
    void print(int num); //send a number as a string, not as an ascii
    void print(int32_t c, uint8_t type = DEC); //print a byte in 2 forms, HEX, DEC


    void println(char* string); //append a NL (new line) in end
    void println(int num); //print number as a string, append NL in end.
    void println(int32_t c, uint8_t type = DEC); //print a byte in 3 forms

    void EndSimulation(void); //send a EOT (end of transmission) and stop.
    bool RX_buf_full(void);
    bool TX_buf_full(void);
    char* read(void);


    void setTimeout(uint16_t time = 1000);

  private:
    CMSDK_UART_TypeDef * UARTx;
    uint16_t timeout;
};


#endif