#ifndef __LIS3DH_H__
#define __LIS3DH_H__

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define LIS3DH_DEFAULT_ADDRESS  (0x19)    // if SDO/SA0 is 3V, its 0x19, or 0x18 on GND 
/*=========================================================================*/

#define LIS3DH_REG_STATUS1       0x07
#define LIS3DH_REG_OUTADC1_L     0x08
#define LIS3DH_REG_OUTADC1_H     0x09
#define LIS3DH_REG_OUTADC2_L     0x0A
#define LIS3DH_REG_OUTADC2_H     0x0B
#define LIS3DH_REG_OUTADC3_L     0x0C
#define LIS3DH_REG_OUTADC3_H     0x0D
#define LIS3DH_REG_INTCOUNT      0x0E
#define LIS3DH_REG_WHOAMI        0x0F
#define LIS3DH_REG_TEMPCFG       0x1F
#define LIS3DH_REG_CTRL1         0x20
#define LIS3DH_REG_CTRL2         0x21
#define LIS3DH_REG_CTRL3         0x22
#define LIS3DH_REG_CTRL4         0x23
#define LIS3DH_REG_CTRL5         0x24
#define LIS3DH_REG_CTRL6         0x25
#define LIS3DH_REG_REFERENCE     0x26
#define LIS3DH_REG_STATUS2       0x27
#define LIS3DH_REG_OUT_X_L       0x28
#define LIS3DH_REG_OUT_X_H       0x29
#define LIS3DH_REG_OUT_Y_L       0x2A
#define LIS3DH_REG_OUT_Y_H       0x2B
#define LIS3DH_REG_OUT_Z_L       0x2C
#define LIS3DH_REG_OUT_Z_H       0x2D
#define LIS3DH_REG_FIFOCTRL      0x2E
#define LIS3DH_REG_FIFOSRC       0x2F
#define LIS3DH_REG_INT1CFG       0x30
#define LIS3DH_REG_INT1SRC       0x31
#define LIS3DH_REG_INT1THS       0x32
#define LIS3DH_REG_INT1DUR       0x33
#define LIS3DH_REG_CLICKCFG      0x38
#define LIS3DH_REG_CLICKSRC      0x39
#define LIS3DH_REG_CLICKTHS      0x3A
#define LIS3DH_REG_TIMELIMIT     0x3B
#define LIS3DH_REG_TIMELATENCY   0x3C
#define LIS3DH_REG_TIMEWINDOW    0x3D
#define LIS3DH_REG_ACTTHS        0x3E
#define LIS3DH_REG_ACTDUR        0x3F
#define CE0 10
#define SCK 14
#define MOSI 12
#define MISO 13

typedef enum
{
  LIS3DH_RANGE_16_G         = 0b11,   // +/- 16g
  LIS3DH_RANGE_8_G           = 0b10,   // +/- 8g
  LIS3DH_RANGE_4_G           = 0b01,   // +/- 4g
  LIS3DH_RANGE_2_G           = 0b00    // +/- 2g (default value)
} lis3dh_range_t;

typedef enum
{
  LIS3DH_AXIS_X         = 0x0,
  LIS3DH_AXIS_Y         = 0x1,
  LIS3DH_AXIS_Z         = 0x2,
} lis3dh_axis_t;

typedef enum
{
  LIS3DH_DATARATE_400_HZ     = 0b0111, //  400Hz 
  LIS3DH_DATARATE_200_HZ     = 0b0110, //  200Hz
  LIS3DH_DATARATE_100_HZ     = 0b0101, //  100Hz
  LIS3DH_DATARATE_50_HZ      = 0b0100, //   50Hz
  LIS3DH_DATARATE_25_HZ      = 0b0011, //   25Hz
  LIS3DH_DATARATE_10_HZ      = 0b0010, // 10 Hz
  LIS3DH_DATARATE_1_HZ       = 0b0001, // 1 Hz
  LIS3DH_DATARATE_POWERDOWN  = 0,
  LIS3DH_DATARATE_LOWPOWER_1K6HZ  = 0b1000,
  LIS3DH_DATARATE_LOWPOWER_5KHZ  =  0b1001,

} lis3dh_dataRate_t;

// lettura dati i2c
void lis3dh_init(int fd);
void checkconnection(int fd);
void setDataRate(int fd, lis3dh_dataRate_t dataRate );
lis3dh_dataRate_t getDataRate(int fd);
void setRange(int fd,lis3dh_range_t range );
lis3dh_range_t getRange(int fd);
void lis3dh_read_xyz(int fd, float buff[]);
void imposta_accelerazione(int fd,int valore);

// lettura dati spi
void lis3dh_init_spi();
void checkconnection_spi();
void setDataRate_spi(lis3dh_dataRate_t dataRate );
lis3dh_dataRate_t getDataRate_spi();
void setRange_spi(lis3dh_range_t range );
lis3dh_range_t getRange_spi();
void lis3dh_read_xyz_spi(float buff[]);
void imposta_accelerazione_spi(int valore);
void write_register_spi(uint8_t reg, uint8_t value);
uint8_t read_register_spi(uint8_t reg);

// stampa dei valori su schermo
void scrivi_su_schermo(float buff[], int valore);
int ricerca_numero_spazi( void );
void trasforma_in_intero(float buff_scalato[],int buff_int[] );
int arrotonda(float numero);
void  valuta_positivo_negativo( float buff[],int flag[]);
float ricerca_fattore(int valore);
void scala_valori(float buff_scalato[] ,float buff[] ,float fattore_di_scala );

// intestazione
void stampa_intestazione(int numero_spazi);
void stampa_inizio_spazi( int numero_spazi);
void stampa_fine_spazi(int numero_spazi);
void stampa_spazio(float valore);
void stampa_underscore(int numero_spazi);

/*****************************************************************

 Copyright 2001   PIER LUCA MONTESSORO

 University of Udine
 ITALY

 montessoro@uniud.it
 www.montessoro.it

 This file is part of a freeware open source software package.
 It can be freely used (as it is or modified) as long as this
 copyright note is not removed.

******************************************************************/

//collegamento server
void invia_dati_server(int sk, float buff[], int valore );
int create_tcp_client_connection (char *ip_address, int port);
int close_tcp_connection (int sk);
int tcp_send (int sk, char *buffer);
int tcp_receive (int sk, char *buffer);
void error_handler (const char *message);


#endif
