/*
    GPIO    NodeMCU   Name  |   Uno
   ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13
*/

#include <SPI.h>
uint8_t _ss_pin = 15;

void spi_write(byte reg_addr, byte command) {
  _ss_pin = 15;
  digitalWrite(_ss_pin, LOW);
  //set to write
  SPI.transfer(reg_addr);
  SPI.transfer(command);
  digitalWrite(_ss_pin, HIGH);
}

void spi_read(byte reg_addr) {
  digitalWrite(_ss_pin, LOW);
  //set to write
  SPI.transfer(128 && reg_addr);
  digitalWrite(_ss_pin, HIGH);
}

void spi_init(){
  pinMode(_ss_pin, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWrite(_ss_pin, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  //disable I2C
  spi_write(106, 16);
}
