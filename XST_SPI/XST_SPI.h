#ifndef XST_SPI_H
#define XST_SPI_H

#include "CMSDK_CM3.h"
#include <stdio.h>
#include "utility.h"

//PORT SPI
#define MISO 8
#define MOSI 9
#define SCK 10
#define CS 0 //GPIO0
#define GPIO_CS CMSDK_GPIO1

//SPCR bits
#define SPIE	SPI_SPCR_SPIE_Msk 		//Interrupt enable
#define SPE		SPI_SPCR_SPE_Msk	 	//Enable SPI
#define MSTR	SPI_SPCR_MSTR_Msk 	 	//Master mode select
#define CPOL	SPI_SPCR_CPOL_Msk	 	//Clock polarity
#define CPHA	SPI_SPCR_CPHA_Msk	 	//Clock Phase
#define SPR		SPI_SPCR_SPR_Msk			//Clock rate select

//SPSR bits
#define SPIF	SPI_SPSR_SPIF_Msk			//Interrupt flag
#define WCOL	SPI_SPSR_WCOL_Msk	 		//Write collision
#define WFFULL	SPI_SPSR_WFFULL_Msk 		//Write FIFO full
#define WFEMPTY	SPI_SPSR_WFEMPTY_Msk		//Write FIFO empty
#define RFFULL	SPI_SPSR_RFFULL_Msk		 	//Read FIFO full
#define RFEMPTY	SPI_SPSR_RFEMPTY_Msk		//Read FIFO empty

//etc
#define Wait(Address, Mask, Expected) do{;}while(((*(volatile uint32_t*)(Address)) & (Mask)) != (Expected))
#define WRITE 1
#define READ 0
#define	ENABLE 1
#define	DISABLE 0

//some functions declarations
class XST_SPI {
  public:
	XST_SPI(SPI_TypeDef * SPI_nomer = SPI0);
	void reset(void);
	void begin(void); //initialize the SPI library
	void core_enable(bool is_enable = true);
	bool settings(bool IS_MASTER = true, uint8_t MODE = 3, uint8_t CLK_DIV_MODE = 3);
	void transfer(uint8_t data);
	void end_transfer(void);
};

#endif