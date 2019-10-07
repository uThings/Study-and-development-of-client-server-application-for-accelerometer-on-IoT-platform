
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

// collegamento server
int create_tcp_server (char *ip_address, int port);
int close_tcp_connection (int sk);
int tcp_send (int sk, char *buffer);
int tcp_receive (int sk, char *buffer);
int server_handler (int sk, char *ip_addr, int port, int iterazioni, int valore);
void error_handler (const char *message);

// stampa dei valori su schermo
void stampa_valori(float buff[], int valore);
int ricerca_numero_spazi( void );
void trasforma_in_intero(float buff_scalato[],int buff_int[] );
int arrotonda(float numero);
void  valuta_positivo_negativo( float buff[],int flag[]);
float ricerca_fattore(int valore);
void scala_valori(float buff_scalato[] ,float buff[] ,float fattore_di_scala );

//intestazione della stampa
void stampa_intestazione(int numero_spazi);
void stampa_inizio_spazi( int numero_spazi);
void stampa_fine_spazi(int numero_spazi);
void stampa_spazio(float valore);
void stampa_underscore(int numero_spazi);

