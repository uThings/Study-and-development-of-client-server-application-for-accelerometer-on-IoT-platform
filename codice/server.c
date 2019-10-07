#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "serverlib.h"

int main (int argc, char *argv[])
{
   if (argc != 3)
   {
      fprintf (stderr, "required arguments: server_ip_address tcp_port\n");
      exit (EXIT_FAILURE);
   }

   if (create_tcp_server (argv[1], atoi (argv[2])) < 0)
   {
      fprintf (stderr, "error creating TCP server\n");
      exit (EXIT_FAILURE);
   }

   return EXIT_SUCCESS;
}

