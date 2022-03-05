


/* The ESP32 has four SPi buses, however as of right now only two of
 * them are available to use, HSPI and VSPI. Simply using the SPI API 
 * as illustrated in Arduino examples will use VSPI, leaving HSPI unused.
 */
#include <SPI.h>

// Define ALTERNATE_PINS to use non-standard GPIO pins for SPI bus

#define HSPI_MISO   12
#define HSPI_MOSI   13
#define HSPI_SCLK   14
#define HSPI_SS     15


static const int spiClk = 2000; // 1 MHz

//uninitalised pointers to SPI objects
SPIClass * hspi = NULL;

void setup() {
  //initialise two instances of the SPIClass attached to VSPI and HSPI respectively
  hspi = new SPIClass(HSPI);
  
  //clock miso mosi ss

  //alternatively route through GPIO pins
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS); //SCLK, MISO, MOSI, SS

  //set up slave select pins as outputs as the Arduino API
  //doesn't handle automatically pulling SS low
  pinMode(5, OUTPUT); //VSPI SS
  pinMode(HSPI_SS, OUTPUT); //HSPI SS
  spiWrite(106, 16);
}

// the loop function runs over and over again until power down or reset
void loop() {
  //use the SPI buses
  spiRead(64);
  delay(50);

}


void spiWrite(byte reg_addr, byte command) {
 
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(HSPI_SS, LOW);
  hspi->transfer(reg_addr);
  hspi->transfer(command);
  digitalWrite(HSPI_SS, HIGH);
  hspi->endTransaction();
}

void spiRead(byte reg_addr) {
 
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(HSPI_SS, LOW);
  hspi->transfer(reg_addr);
  digitalWrite(HSPI_SS, HIGH);
  hspi->endTransaction();
}
