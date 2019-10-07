#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// funzioni per la comunicazione I2C e SPI
#include "Lis3dh.h"

// librerie wiringpi
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>

#define channel 0 
#define clock 500000 

int main(int argc, char *argv[])
{
	// variabili per l'acquisizione dati da accelerometro
    int fd = 0;
    int valore = 0;
	int  numero_iterazioni = 0;
	int i = 0;
    float buff[3];
    char iterazioni[30];

    // varibile per il collegamento TCP
    int sk;

    if(argc != 5 )
    {
        printf(" indicare se la comunicazione seriale risulta essere SPI oppure I2C\n"  );
        printf(" indicare il valore di accelerazione da impostare (2,4,8,16)\n" );
        printf(" indicare indirizzo IP e porta per il collegamento socket\n"    );
        return 1;
    } 
   
    if(strcmp(argv[1], "I2C")== 0)
    {
        
		 // inizializzazione della comunicazione i2c
    	 fd = wiringPiI2CSetup(LIS3DH_DEFAULT_ADDRESS);
        
         if( fd < 0 )
         {
            printf(" errore nella lettura dei parametri del dispositivo ");
         }
         
  
         // stabilire la connessione tcp con il server
         if ((sk = create_tcp_client_connection (argv[3], atoi (argv[4]))) < 0)
         {
            fprintf (stderr, "cannot open TCP connection\n");
            exit (EXIT_FAILURE);
         }

         lis3dh_init(fd);                       // inizializzazione dei registri
         valore = atoi(argv[2]);                // lettura range di accelerazione
         imposta_accelerazione(fd,valore);
         tcp_send(sk, argv[2]);                 // invio del range al server

         do
         {
             printf("Indicare numero di campioni da inviare al server (QUIT per terminare) : ");
             scanf("%s", iterazioni );
    
             if( strcmp("QUIT", iterazioni ) != 0 )
             { 
                  numero_iterazioni = atoi(iterazioni);
                  tcp_send(sk, iterazioni );

               for( i = 0; i < numero_iterazioni; i++ )
               {
                   lis3dh_read_xyz(fd, buff);          // lettura dati da accelerometro
                   scrivi_su_schermo(buff, valore);    // stampa valori per il client
                   invia_dati_server(sk,buff,valore);  // invio dati client-server
                   delay(100);
				   
                   printf("\033c");                    // pulisce lo schermo
					
                }
                  printf("termine invio dati\n");
              }
              else // chiusura del client
              {
                  numero_iterazioni = -1;
                  tcp_send(sk, iterazioni );
                  printf("chiusura connessione\n");
              } 
   
         }while(numero_iterazioni >= 0 );
    

         close_tcp_connection (sk);

         }
         else
         { 
            digitalWrite( CE0, HIGH );
            pinMode( CE0, OUTPUT );
         }
    
    if(strcmp(argv[1], "SPI")== 0)
    {
		
        //inizializzazione della comunicazione spi
        fd = wiringPiSPISetup(channel, clock); 
        
        if( fd < 0 )
        {
           printf(" errore nella lettura dei parametri del dispositivo in mod. spi; valore di clock: %d\n", clock);
        }

        // stabilire la connessione tcp con il server
        if ((sk = create_tcp_client_connection (argv[3], atoi (argv[4]))) < 0)
        {
           fprintf (stderr, "cannot open TCP connection\n");
           exit (EXIT_FAILURE);
        }

        lis3dh_init_spi();                     // inizializzazione dei registri
        valore = atoi(argv[2]);                // lettura range di accelerazione
        imposta_accelerazione_spi(valore); 
        tcp_send(sk, argv[2]);                 // invio del range al server
       
        do
         {
             printf("Indicare numero di campioni da inviare al server (QUIT per terminare) : ");
             scanf("%s", iterazioni );
    
             if( strcmp("QUIT", iterazioni ) != 0 )
             { 
                  numero_iterazioni = atoi(iterazioni);
                  tcp_send(sk, iterazioni );

               for( i = 0; i < numero_iterazioni; i++ )
               {
                   lis3dh_read_xyz_spi(buff);              // lettura dati da accelerometro
                   scrivi_su_schermo(buff, valore);        // stampa valori per il client
                   invia_dati_server(sk,buff,valore);      // invio dati client-server
                   delay(100);
                   
                    printf("\033c");                       // pulisce lo schermo
                }
                  printf("termine invio dati\n");
              }
              else // chiusura del client
              {
                  numero_iterazioni = -1;
                  tcp_send(sk, iterazioni );
                  printf("chiusura connessione\n");
              } 
   
         }while(numero_iterazioni >= 0 );
    

         close_tcp_connection (sk);
		 
       }
       else
       {
          pinMode( SCK, OUTPUT );
          pinMode( MOSI, OUTPUT );
          pinMode( MISO, OUTPUT );
       }
      
   
    return EXIT_SUCCESS;
}
