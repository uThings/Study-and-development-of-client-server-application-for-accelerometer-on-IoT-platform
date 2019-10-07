#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include "Lis3dh.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>

#define channel 0 
#define clock 500000 
#define NUMERO_MAX 80

/****************************************I2C***************************************/

void lis3dh_init(int fd)
{
	
   wiringPiI2CWriteReg8(fd,LIS3DH_REG_CTRL1, 0x07);
   checkconnection(fd);
   setDataRate(fd, LIS3DH_DATARATE_400_HZ);
   wiringPiI2CWriteReg8(fd,LIS3DH_REG_CTRL4, 0x88);
   wiringPiI2CWriteReg8(fd,LIS3DH_REG_CTRL3, 0x10);
   wiringPiI2CWriteReg8(fd,LIS3DH_REG_TEMPCFG, 0x80);

}

void checkconnection(int fd)
{
    uint8_t deviceid = wiringPiI2CReadReg8(fd, LIS3DH_REG_WHOAMI);

   if(deviceid != 0x33)
   {
      printf(" nessuna connessione con il dispositivo\n " );
      exit(1);
   }
   
}

void setDataRate(int fd, lis3dh_dataRate_t dataRate )
{
   uint8_t ctl1 = wiringPiI2CReadReg8(fd, LIS3DH_REG_CTRL1);
   ctl1 &= ~(0xF0);
   ctl1 |= ( dataRate << 4 );
   wiringPiI2CWriteReg8(fd,LIS3DH_REG_CTRL1, ctl1);
}

lis3dh_dataRate_t getDataRate(int fd)
{
    return (lis3dh_dataRate_t)((wiringPiI2CReadReg8(fd, LIS3DH_REG_CTRL1) >> 4 )& 0x0F);
}

void setRange(int fd, lis3dh_range_t range )
{
    uint8_t r = wiringPiI2CReadReg8(fd,LIS3DH_REG_CTRL4);
    r &= ~(0x30);
    r |= range << 4;
    wiringPiI2CWriteReg8(fd,LIS3DH_REG_CTRL4,r);
}

lis3dh_range_t getRange(int fd)
{
    //return (lis3dh_range_t)((wiringPiI2CReadReg8(fd,LIS3DH_REG_CTRL4) >> 4) & 0x03);
    return (uint8_t)((wiringPiI2CReadReg8(fd,LIS3DH_REG_CTRL4) >> 4) & 0x03);
}

void lis3dh_read_xyz(int fd, float buff[])
{
    uint8_t x0, y0, z0, x1, y1, z1;
    int x,y,z;

    // 0xff per invertire gli assi 
    x0 = 0xff - wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_X_L); // valore basso
    y0 = 0xff - wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Y_L);
    z0 = 0xff - wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Z_L);
    x1 = 0xff - wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_X_H); // valore alto
    y1 = 0xff - wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Y_H);
    z1 = 0xff - wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Z_H);
    
    x = (short int)((((uint16_t) x1 ) << 8) | x0);
    y = (short int)((((uint16_t) y1 ) << 8) | y0);
    z = (short int)((((uint16_t) z1 ) << 8) | z0);

   
    uint8_t range = getRange(fd);
	
    float divider = 1;
    if ( range == LIS3DH_RANGE_16_G)
        divider = 1365;
    if ( range == LIS3DH_RANGE_8_G)
        divider = 4096;
    if ( range == LIS3DH_RANGE_4_G)
        divider = 8190;
    if ( range == LIS3DH_RANGE_2_G)
        divider = 16380;
   
    buff[0] = ((float)x) / divider;
    buff[1] = ((float)y) / divider;
    buff[2] = ((float)z) / divider;

}

void imposta_accelerazione(int fd,int valore)
{
   if( valore == 2 )
         setRange(fd,LIS3DH_RANGE_2_G);

   if( valore == 4 )
         setRange(fd,LIS3DH_RANGE_4_G);

   if( valore == 8 )
         setRange(fd,LIS3DH_RANGE_8_G);

   if( valore == 16 )
         setRange(fd,LIS3DH_RANGE_16_G);
}

/****************************************SPI****************************************/

void lis3dh_init_spi()
{
   write_register_spi(LIS3DH_REG_CTRL1, 0x07);
   checkconnection_spi();
   setDataRate_spi(LIS3DH_DATARATE_400_HZ);
   write_register_spi(LIS3DH_REG_CTRL4, 0x88);
   write_register_spi(LIS3DH_REG_CTRL3, 0x10);
   write_register_spi(LIS3DH_REG_TEMPCFG, 0x80);

}

void checkconnection_spi()
{
    uint8_t deviceid = read_register_spi(LIS3DH_REG_WHOAMI);

   if(deviceid != 0x33)
   {
      printf(" nessuna connessione con il dispositivo\n " );
      exit(1);
   }
   
}

void setDataRate_spi(lis3dh_dataRate_t dataRate )
{
   uint8_t ctl1 = read_register_spi(LIS3DH_REG_CTRL1);
   ctl1 &= ~(0xF0);
   ctl1 |= ( dataRate << 4 );
   write_register_spi(LIS3DH_REG_CTRL1, ctl1);
}

void write_register_spi(uint8_t reg, uint8_t value)
{
    uint8_t tx_buff[2];

    tx_buff[0] = reg & ~0x80;
    tx_buff[1] = value;
    digitalWrite(CE0, LOW);

    if(wiringPiSPIDataRW(channel, tx_buff , 2) == -1 )
    {
       printf("errore nella trasmissione dati");
       exit(1);
    }

    digitalWrite(CE0, HIGH);
}

uint8_t read_register_spi(uint8_t reg)
{
    uint8_t tx_buff[2];

    tx_buff[0] = reg | 0x80;
    tx_buff[1] = 0x00;

    digitalWrite(CE0, LOW);
    
    // nella lettura i dati vengono sovrascritti, quindi dallo stesso vettore ottengo i dati in ricezione rx
    if(wiringPiSPIDataRW(channel, tx_buff , 2) == -1 )
    {
       printf("errore nella trasmissione dati");
       exit(1);
    }

    digitalWrite(CE0, HIGH);

    return tx_buff[1];
}


lis3dh_dataRate_t getDataRate_spi()
{
    return (lis3dh_dataRate_t)((read_register_spi(LIS3DH_REG_CTRL1) >> 4 )& 0x0F);
}

void setRange_spi(lis3dh_range_t range )
{
    uint8_t r = read_register_spi(LIS3DH_REG_CTRL4);
    r &= ~(0x30);
    r |= range << 4;
    write_register_spi(LIS3DH_REG_CTRL4,r);
}

lis3dh_range_t getRange_spi()
{
    //return (lis3dh_range_t)((wiringPiI2CReadReg8(fd,LIS3DH_REG_CTRL4) >> 4) & 0x03);
    return (uint8_t)((read_register_spi(LIS3DH_REG_CTRL4) >> 4) & 0x03);
}

void lis3dh_read_xyz_spi(float buff[])
{
    char x0, y0, z0, x1, y1, z1;
    int  x,y,z;

    // 0xff per invertire gli assi 
    x0 = 0xff - read_register_spi(LIS3DH_REG_OUT_X_L); // valore basso
    y0 = 0xff - read_register_spi(LIS3DH_REG_OUT_Y_L);
    z0 = 0xff - read_register_spi(LIS3DH_REG_OUT_Z_L);
    x1 = 0xff - read_register_spi(LIS3DH_REG_OUT_X_H); // valore alto
    y1 = 0xff - read_register_spi(LIS3DH_REG_OUT_Y_H);
    z1 = 0xff - read_register_spi(LIS3DH_REG_OUT_Z_H);
    
    x = (short int)((((uint16_t) x1 ) << 8) | x0);
    y = (short int)((((uint16_t) y1 ) << 8) | y0);
    z = (short int)((((uint16_t) z1 ) << 8) | z0);


    uint8_t range = getRange_spi();
	
    float divider = 1;
    if ( range == LIS3DH_RANGE_16_G)
        divider = 1365;
    if ( range == LIS3DH_RANGE_8_G)
        divider = 4096;
    if ( range == LIS3DH_RANGE_4_G)
        divider = 8190;
    if ( range == LIS3DH_RANGE_2_G)
        divider = 16380;
   
    buff[0] = ((float)x) / divider;
    buff[1] = ((float)y) / divider;
    buff[2] = ((float)z) / divider;


}

void imposta_accelerazione_spi(int valore)
{
     if( valore == 2 )
         setRange_spi(LIS3DH_RANGE_2_G);

     if( valore == 4 )
         setRange_spi(LIS3DH_RANGE_4_G);

     if( valore == 8 )
           setRange_spi(LIS3DH_RANGE_8_G);

     if( valore == 16 )
           setRange_spi(LIS3DH_RANGE_16_G);
} 

/**********************scrittura dati su schermo**********************/

void scrivi_su_schermo(float buff[] , int valore)
{
      int numero_spazi;
      int flag[3]; // tiene in memoria il segno dei valori sui tre assi
                   // 0 per positivo, 1 per negativo
      int diff;
	  int buff_int[3];
      int i,j,k= 0;
      float buff_scalato[3]; // conterrà i valori di buff moltiplicati per il fattore di scala
      float fattore_di_scala;
	  char vect[3] = "XYZ";

      numero_spazi = ricerca_numero_spazi();
      valuta_positivo_negativo(buff, flag); 
      fattore_di_scala = ricerca_fattore( valore );
      scala_valori( buff_scalato, buff, fattore_di_scala );
      trasforma_in_intero(buff_scalato, buff_int ); // funzione che moltiplica per 9,8 il valore scalato e trasforma in intero il valore float 
      stampa_intestazione(numero_spazi);

      // stampa dei valori

      for(i = 0; i < 3; i++)
      {
         if( flag[i] == 1 )
         {
             diff = numero_spazi + buff_int[i];
             for( j = 0; j < diff; j++)
                  printf(" ");

             j=0;
            for( k = 0; k < -buff_int[i]; k++)
                printf("▪");
            
            k=0;
            printf("|  %c:%.02f ", vect[i], buff[i]*9.8);
            stampa_spazio(buff[i]*9.8);
            printf("|\n");
          }

         if( flag[i] == 0 )
         {
             for( j = 0; j < numero_spazi; j++)
                  printf(" ");
             j=0;
             printf("|  %c: %.02f ",vect[i],buff[i]*9.8);
             stampa_spazio(buff[i]*9.8);
             printf("|");
             for( k = 0; k < buff_int[i]; k++)
                printf("▪");
         
             k=0;
             printf("\n");
         }

     }
     i = 0;
}

int ricerca_numero_spazi(void)
{
      return NUMERO_MAX; // risulta essere impostato a 80 e serve a fissare il valore massimo
}

void trasforma_in_intero(float buff_scalato[],int buff_int[] )
{
    int i,j = 0 ;
 
    for( i = 0; i < 3; i++ )
    {
          buff_scalato[i] = buff_scalato[i]* 9.8;
                
    }
    i = 0;

   for( j = 0; j < 3; j++ )
   {
        
        buff_int[j] = arrotonda(buff_scalato[j]);
   }
   j = 0;
}

int arrotonda(float numero)
{
    int numero_intero = (int) numero;
    float differenza = (float)(numero - (float)numero_intero);

    if( fabs(differenza) >= 0.5 )
    { 
       if( numero >= 0)
         return ((int) numero + 1);
       else
         return ((int) numero - 1);
    }
    else
    {
        return ((int)numero);
    }
}

void  valuta_positivo_negativo( float buff[] ,int flag[])
{
    int i = 0;
 
    for( i = 0; i < 3; i++)
    {
        if(buff[i] < 0)
        {
           flag[i] = 1;
        }
       
        if(buff[i] >= 0) 
        {
          flag[i] = 0;
        }
   }
   i = 0;
} 

float ricerca_fattore(int valore)
{
   if( valore == 2 )
       return (((float)NUMERO_MAX)/20);

   if( valore == 4 )
        return (((float)NUMERO_MAX)/40);
  
   if( valore == 8 )
       return (((float)NUMERO_MAX)/80);

   if( valore == 16 )
       return (((float)NUMERO_MAX)/160);
}

void scala_valori(float buff_scalato[] ,float buff[] ,float fattore_di_scala )
{
    int i;
   
    i = 0;

    for( i = 0; i < 3; i++ )
   {
      buff_scalato[i] = buff[i]*fattore_di_scala;
   }
}

void stampa_intestazione(int numero_spazi)
{
    stampa_inizio_spazi( numero_spazi);
    printf("negative");
    stampa_fine_spazi( numero_spazi);
    printf("|    AXIS    |");  // 3 spazi prima e dopo di AXIS
    stampa_inizio_spazi( numero_spazi);
    printf("positive");
    printf("\n");
    stampa_underscore(numero_spazi);

}

void stampa_inizio_spazi( int numero_spazi)
{
     int spaziatura = numero_spazi/2 - 4;
     int i = 0;

     for (i = 0; i < spaziatura;i ++)
           printf(" ");

     i = 0;
} 
   
void stampa_fine_spazi(int numero_spazi)
{
    int spaziatura = ( numero_spazi - (numero_spazi/2) - 4 );
    int i;
   
    for( i = 0; i < spaziatura; i++)
       printf(" ");

   i = 0;
}

void stampa_spazio(float valore)
{
      if(abs(valore) < 100 && abs(valore) >= 10 )
        printf(" ");

      if(abs(valore) < 10 )
        printf("  "); 
} 

void stampa_underscore(int numero_spazi)
{
   int i;

   for(i = 0; i < (((numero_spazi)*2)+14);i++)
      printf("=");
  
   i = 0;
   printf("\n");
}

/****************************************TCP-CLIENT***************************************/

void invia_dati_server(int sk, float buff[], int valore)
{
   char str[40]; // contiene 10 valori per riga
   int i;
   
   i = 0;

    // trasforma in char il valore del float 
    sprintf(str,"%0.3f %0.3f %0.3f ", buff[0],buff[1],buff[2] );
   

    // invia i valori con tcp send
    // tcp_send prende un messaggio e lo stampa sul socket
	
     	tcp_send( sk, str ); //scrive i valori sul socket
                                //che saranno letti dal server
    
} 

int create_tcp_client_connection (char *ip_address, int port)
{
   int sk;
   struct sockaddr_in srv;

   /* create a socket descriptor */
   if ((sk = socket (AF_INET, SOCK_STREAM, 0)) < 0)
   {
      error_handler ("socket() [create_tcp_client_connection()]");
      return -1;
   }

   /* fill the (used) fields of the socket address with
      the server information (remote socket address) */
   bzero ((char *) &srv, sizeof (srv));
   srv.sin_family = AF_INET;
   srv.sin_addr.s_addr = inet_addr (ip_address);
   srv.sin_port = htons (port);

   /* open a connection to the server */
   if (connect (sk, (struct sockaddr *) &srv, sizeof(srv)) < 0)
   {
      error_handler ("connect() [create_tcp_client_connection()]");
      return -1;
   }

   return sk;
}

int close_tcp_connection (int sk)
{
   if (close (sk) != 0)
   {
      error_handler ("close() [close_tcp_connection()]");
      return 0;
   }

   return 1;
}

int tcp_send (int sk, char *buffer)
{
   if (write (sk, buffer, strlen(buffer)) != strlen(buffer))
   {
      error_handler ("write() [tcp_send()]");
      return 0;
   }

   return 1;
}

int tcp_receive (int sk, char *buffer)
{
   int dim, flags;

   if ((dim = read (sk, buffer, BUFSIZ)) < 0)
   {
      flags = fcntl (sk, F_GETFL, 0);
      if ((flags & O_NONBLOCK) == O_NONBLOCK)
      {   /* non-blocking mode */
         if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return 0;
         else
         {
            error_handler ("read() [tcp_receive()]");
            return -1;
         }
      }
      else
      {
         error_handler ("read() [tcp_receive()]");
         return -1;
      }
   }

   buffer[dim] = '\0';
   return dim;
}

void error_handler (const char *message)
{
   printf ("fatal error: %s\n", message);
   exit (EXIT_FAILURE);
}
   












 
