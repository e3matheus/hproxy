/*
 * funciones.c
 *
 *  Created on: Oct 11, 2009
 *      Author: e3
 */

#include "funciones.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define maxArg 6
#define BUF_SIZE 500

void fatalerror(char *message) {
  char * programname = "hproxy";
  fprintf(stderr, "%s: %s\n", programname, message);
  perror(programname);
  fflush(stdout);
  exit(1);
}

void showPage (int in_fd)
{
  unsigned char *const buf = malloc (BUF_SIZE);
  unsigned char *const bufRet = malloc (BUF_SIZE);
  int bytes_r_client, bytes_s_google = 0, i;
  int bytes_s_client, bytes_r_google = 0;
  char* server = "www.google.com";
  int out_fd;

  printf("Recibo la pagina.");
  //Recibo la pagina de paquete

  bytes_r_client = recv (in_fd, buf, BUF_SIZE, 0);

  struct hostent *he;

  if ((he=gethostbyname(server))==NULL) {
    printf("error de gethostbyname()\n");
    exit(-1);
  }

  struct sockaddr_in serveraddr;
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_ntoa((struct in_addr *)he->h_addr);
  serveraddr.sin_port = htons(80);

  /* Open a socket. */
  out_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (out_fd < 0)
    fatalerror("can't open socket");

  /* Connect to the server. */
  if (connect(out_fd , (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    fatalerror("can't connect to server");

  printf("Envio la pagina a google.");
//  bytes_s_google = send (out_fd, buf, BUF_SIZE, 0);

  /* Se le manda todo el archivo a google.com */
 /* for (i = 0; i < bytes_r_client; i += bytes_s_google)
  {
    bytes_s_google = send (out_fd, buf + i, bytes_r_client - i, 0);

    if (bytes_s_google < 0)
      break;
  }
*/
/*
  printf("Recibo la pagina de google.");
  bytes_r_google = recv (out_fd, bufRet, BUF_SIZE, 0);
  printf("Envio la pagina al cliente.");
  bytes_s_client = send (in_fd, bufRet, BUF_SIZE, 0);
*/
  free (buf);
  free (bufRet);
}

int buscarLista(FILE* fd, char* input)
{
  char caracter[500];
  while (feof(fd) == 0)
  {
    fgets(caracter, 500, fd);
    if (strcmp(caracter,input) == 0)
    {
      return 1;
    }
  }

  return 0;
}

void crearLog(int numArgumentos, char** argv)
{
  int cont = 1;
  while (cont < numArgumentos)
  {
    if  (strcmp(argv[cont],"-l") == 0)
    {
      if (fopen(argv[cont + 1],"a+")==NULL)
      {
        printf("El archivo de error no pudo ser creado");
      }
    }
    cont +=2;
  }
}

FILE * abreArchivoDirecciones(int numArgumentos, char** argv)
{
  int cont = 1;
  FILE * fd;
  while (cont < numArgumentos)
  {
    if  (strcmp(argv[cont],"-f") == 0)
    {
      fd = fopen((char*) argv[cont + 1],"a+");  
      if (fd==NULL)
      {
        printf("El archivo de error no pudo ser creado");
      }
      return fd;
    }
    cont +=2;
  }
}

char* chequearPuerto(int numArgumentos, char** argv)
{
  int cont = 1;
  while (cont < numArgumentos)
  {
    if  (strcmp(argv[cont],"-p") == 0)
    {
      return argv[cont + 1];
    }
    cont +=2;
  }
  return "16000";
}

void chequearMaxArgumentos(int numArgumentos)
{
  if  (numArgumentos > maxArg) 
  {
    printf("\n ERROR! --> No esta introduciendo correctamente los parametros...\n");
    printf("         #################################################################\n"
        "         # Usage:                                                        #\n"
        "         #      ./hproxy [output_file error_file]                           #\n"
        "         #################################################################\n\n");
    exit(1);
  }
}

char * concatenarPath(char* nombreDir, char *nombreArchivo)
{
  char * newDir= malloc(sizeof(char)*(strlen(nombreDir)+strlen(nombreArchivo)+2));
  strcpy(newDir, nombreDir);
  strcat(newDir,"/");
  strcat(newDir,nombreArchivo);

  return newDir;
}

char * obtenerFecha(time_t * time)
{
  int max = 20;
  char * str = malloc(sizeof(char) * max);

  strftime(str, max, "%F", gmtime(time));
  //	strftime(str, max, "%F %H:%I", gmtime(time));

  return str;
}
