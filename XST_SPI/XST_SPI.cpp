#include "XST_SPI.h"

SPI_TypeDef *SPIx;

XST_SPI::XST_SPI(SPI_TypeDef * SPI_nomer) { //constructor
  SPIx = SPI_nomer;
}

void XST_SPI::reset(void)
{
  SPIx->SPCR = 0x10;
  SPIx->SPSR = 0x05;
  SPIx->SPDR = 0x00;
  SPIx->SPER = 0x00;
}

void XST_SPI::begin(void)
{
  GPIO_CS->ALTFUNCSET = (1 << MOSI) | (1 << SCK) | (1 << MISO);
  GPIO_CS->OUTENABLESET = (1 << CS);
  GPIO_CS->OUTENABLESET |= (1 << MOSI) | (1 << SCK);
  GPIO_CS->OUTENABLESET &= ~1 << MISO;
  GPIO_CS->DATAOUT      |= (1 << CS);

  settings(); //set default settings

  core_enable(); //core enable
}

void XST_SPI::core_enable(bool is_enable)
{
  if (is_enable)
    SPIx->SPCR |= SPE; //core enable
  else
    SPIx->SPCR &= ~SPE; //core enable
}

//JANGAN LUPA ENABLE CORE SETELAH UBAH SETTING
bool XST_SPI::settings(bool IS_MASTER, uint8_t MODE, uint8_t CLK_DIV_MODE) {
  //SPI Default Settings: Master, CPOL CPHA Mode , SCK Divisor Mode 3
  reset();
  if (IS_MASTER)
    SPIx->SPCR |= MSTR;
  else
    SPIx->SPCR &= ~MSTR;

  if (MODE > 0x3)
    //Wrong CPOL CPHA mode
    return 0;
  else
    SPIx->SPCR |= MODE << 2;

  if (CLK_DIV_MODE > 0x3)
    //Wrong SCK divisor
    return 0;
  else
    SPIx->SPCR |= CLK_DIV_MODE;

  //Setting success
  return 1;
}

void SPI_interrupt_Enable(SPI_TypeDef *SPI, uint8_t ien_irq)
{

  if (ien_irq)
  {
    NVIC_EnableIRQ(SPI_IRQn);
    SPI->SPCR |= (SPI_SPCR_SPIE_Msk);
  }
  else
  {
    SPI->SPCR &= ~(SPI_SPCR_SPIE_Msk);
    NVIC_DisableIRQ(SPI_IRQn);
  }
}

void XST_SPI::cs_active()
{
  GPIO_CS->DATAOUT &= ~1 << CS; //chip select enable
}

void XST_SPI::cs_inactive()
{
  GPIO_CS->DATAOUT |= 1 << CS; //chip select enable
}

uint8_t XST_SPI::transfer(uint8_t data)
{
  SPIx->SPDR = data;  //transfer the control byte.
  delaymicros(1); //for stability
  while (!(SPIx->SPSR & WFEMPTY)); //do nothing if write fifo is full
  delaymicros(25);
  return SPIx->SPDR;
  //CMSDK_GPIO0->DATAOUT |= (1<<CS); //chip select disable
}

void XST_SPI::end_transfer(void)
{
  __ISB();//small delay
  CMSDK_GPIO0->DATAOUT |= (1 << CS); //chip select disable
  SPIx->SPCR &= ~SPE; //disable core, reset fifo
}