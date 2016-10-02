#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

/*
  Arduino - AVR
  ---------
  D0  - PD0
  D1  - PD1
  D2  - PD2
  D3  - PD3
  D4  - PD4
  D5  - PD5
  D6  - PD6
  D7  - PD7
  ---------
  D8  - PB0
  D9  - PB1
  D10 - PB2
  D11 - PB3
  D12 - PB4
  D13 - PB5
  ---------
  A0  - PC0
  A1  - PC1
  A2  - PC2
  A3  - PC3
  A4  - PC4
  A5  - PC5
  (32pin Only)
  A6  - ADC6
  A7  - ADC7
 */


// Digital Pin
static const uint8_t SPI_CS   =  8;
static const uint8_t SPI_MOSI = 11;
static const uint8_t SPI_MISO = 12;
static const uint8_t SPI_SCK  = 13;

static const uint8_t CAN_INT  =  2;
static const uint8_t CAN_CS   =  8;
static const uint8_t CAN_MOSI = 11;
static const uint8_t CAN_MISO = 12;
static const uint8_t CAN_SCK  = 13;

static const uint8_t RX       =  0;
static const uint8_t TX       =  1;

static const uint8_t STAT_LED1=  4;
static const uint8_t STAT_LED2=  7;


static const uint8_t SERVO_OFF = 3;

// Analog Pin
static const uint8_t INPUT0 = 14; // A0
static const uint8_t INPUT1 = 15; // A1
static const uint8_t INPUT2 = 16; // A2
static const uint8_t INPUT3 = 17; // A3

static const uint8_t INPUT9 = 9; // D9
static const uint8_t INPUT10 = 10;// D10

static const uint8_t I2C_SDA = 18; // A4
static const uint8_t I2C_SCL = 19;  // A5

#endif
