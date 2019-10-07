#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "serverlib.h"

/* maximum number of simultaneous connections on the server side */
#define QUEUELEN       8

#define NUMERO_MAX     80


int create_tcp_server (char *ip_address, int port)
{
   int sk, csk;
   struct sockaddr_in srv, cln;
   unsigned int dimcln;
   int keep_server_alive, keep_connection_alive;
   char buffer[10];
   char ch[3];
   int iterazioni,valore;

   /* create a socket descriptor */
   if ((sk = socket (AF_INET, SOCK_STREAM, 0)) < 0)
   {
      error_handler ("socket() [create_tcp_server()]");
      return -1;
   }

   /* fill the (used) fields of the socket address with
      the server information (local socket address) */
   bzero ((char *) &srv, sizeof (srv));
   srv.sin_family = AF_INET;
   srv.sin_addr.s_addr = inet_addr (ip_address);
   srv.sin_port = htons (port);

   /* add the local socket address to the socket descriptor */
   if (bind (sk, (struct sockaddr *) &srv, sizeof(srv)) < 0)
   {
      error_handler ("bind() [create_tcp_server()]");
      return -1;
   }

   /* make the socket a passive socket (enable the socket 
      accepting connections) */
   if (listen (sk, QUEUELEN) < 0)
   {
      error_handler ("listen() [create_tcp_server()]");
      return -1;
   }
   printf("in attesa di stabilire una connessione con un client\n");
   
   dimcln = sizeof (cln);

   /* get the next connection request from the queue */
   if ((csk = accept (sk, (struct sockaddr *) &cln, &dimcln)) < 0)
   {
       error_handler ("accept() [create_tcp_server()]");
       return -1;
   }

   printf ("connected to %s:%d\n", inet_ntoa (cln.sin_addr), (int) cln.sin_port);

   tcp_receive(csk,buffer);
   valore = atoi(buffer); // valore utile per la graficazione

   do                              /* server loop */
   {

     keep_connection_alive = 1;

      while(keep_connection_alive)
      {
          tcp_receive(csk,buffer);
          printf("%s\n", buffer);
          if(strncmp(buffer,"QUIT",4 ) == 0 ) // implica che il client vuole chiudere la connessione
          {
              printf("chiusura connessione client\n");
              keep_connection_alive = 0;
          }
          else
          {
             iterazioni = atoi(buffer);
             printf("il client sta per inviare %d campioni\n", iterazioni );
             // ricevi e stampa i dati
             keep_connection_alive = server_handler (csk, inet_ntoa (cln.sin_addr), (int) cln.sin_port,iterazioni,  valore); // la funzione che legge da socket
          }
      }

      printf("OPEN:  il server rimane aperto ad una nuova connessione \n");
      printf("CLOSE: chiusura server \n");
      printf("Digitare l'azione:");
      scanf("%s", buffer );
  
      if( strcmp( buffer, "OPEN") == 0 )
      {
          /* get the next connection request from the queue */
          printf("in attesa di una nuova connessione:\n");
          if ((csk = accept (sk, (struct sockaddr *) &cln, &dimcln)) < 0)
         {
            error_handler ("accept() [create_tcp_server()]");
             return -1;
         }
         printf ("connected to %s:%d\n", inet_ntoa (cln.sin_addr), (int) cln.sin_port);

         tcp_receive(csk,buffer);
         valore = atoi(buffer);
         keep_server_alive = 1;
       }
       else
       {
         keep_server_alive = 0;
     }
    
   } while (keep_server_alive);

   printf("server chiuso con successo \n");
   close_tcp_connection (csk);
   
   return 1;
}

void error_handler (const char *message)
{
   printf ("fatal error: %s\n", message);
   exit (EXIT_FAILURE);
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

int server_handler (int csk, char *ip_addr, int port,int iterazioni, int valore)
{
   char buffer [40],axis_x[10],axis_y[10],axis_z[10];
   float axis[3];
   char str[3] = "XYZ";
   int i,x,y,z ;

   int flag = 0;
  
   for(flag = 0; flag < iterazioni; flag++)
   {

       tcp_receive (csk, buffer);
       i = 0,x=0, y=0,z=0;
       while( buffer[i] != ' ')
       {
           axis_x[x] = buffer[i];
           i++;
           x++;
       }
       
       i++;
        
       while( buffer[i] != ' ')
       {
           axis_y[y] = buffer[i];
           i++;
           y++;
       }

       i++;

       while( buffer[i] != ' ')
       {
           axis_z[z] = buffer[i];
           i++;
           z++;
       }
      
      axis[0] = atof(axis_x);
      axis[1] = atof(axis_y);
      axis[2] = atof(axis_z);
       
      stampa_valori(axis, valore);
      printf("\033c");        // pulisce lo schermo
  
     }

     return 1;
}

/*********************************STAMPA-DATI*********************************/

void stampa_valori(float buff[] ,int valore )
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



