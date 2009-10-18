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
#define BUF_SIZE 1024

void fatalerror(char *message) {
  char * programname = "hproxy";
  fprintf(stderr, "%s: %s\n", programname, message);
  perror(programname);
  fflush(stdout);
  exit(1);
}

void Cat (int in_fd, int out_fd)
{
  unsigned char *const buf = malloc (BUF_SIZE);
  int bytes_rcvd, bytes_sent = 0, i, j = 2;

  while(1) 
  {
    bytes_rcvd = recv (in_fd, buf, BUF_SIZE, 0);

    for (i = 0; i < bytes_rcvd; i += bytes_sent)
    {
      bytes_sent = send (out_fd, buf + i, bytes_rcvd - i, 0);

      if (bytes_sent < 0)
        break;
    }

    if(strstr(buf, "\n\r\n") != NULL)
      break;
  } 

  free (buf);
}

char* getServer(int in_fd)
{
  unsigned char *const header = malloc (150);
  recv (in_fd, header, 150, MSG_PEEK);
  char* dom = strstr(header,"//") + 2;
  char * server =  strndup(dom, strlen(dom) - strlen(strchr(dom,'/') ));

  return server;
}

char* dirIP(char* serv){
  char * IP;
  struct hostent *he;

  if ((he=gethostbyname(serv))==NULL) {
    printf("error de gethostbyname()\n");
    exit(-1);
  }

  return inet_ntoa(*((struct in_addr *)he->h_addr));
}

int connectToServer(char* server)
{
  int out;
  struct sockaddr_in serveraddr;

  /* Creando estructura de servidor */
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr(dirIP(server));
  serveraddr.sin_port = htons(80);

  /* Open a socket. */
  out = socket(AF_INET, SOCK_STREAM, 0);
  if (out < 0)
    fatalerror("can't open socket");

  /* Connect to the server. */
  if (connect(out , (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    fatalerror("can't connect to server");

  return out;
}

void showPage (int in_fd)
{
  char * server = getServer(in_fd);
  int out_fd = connectToServer(server);

  Cat(in_fd, out_fd);
  Cat(out_fd, in_fd);
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
