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
#define maxArg 7
#define BUF_SIZE 1024

void fatalerror(char *message) {
  char * programname = "hproxy";
  fprintf(stderr, "%s: %s\n", programname, message);
  perror(programname);
  fflush(stdout);
  exit(1);
}

void getInfo(int in_fd, char* URL, char* host)
{
  char * header = malloc (200);
  recv (in_fd, header, 200, MSG_PEEK);
  sscanf(header, "GET %s HTTP/1.0\nHost: %s\n", URL, host);
}

char * getTipo(int in_fd)
{
  char tipo[500];
  int codigo;
  char * header = malloc (200);
  recv (in_fd, header, 200, MSG_PEEK);
  sscanf(header, "HTTP/1.0 %d %s\n", &codigo, tipo);
  if (codigo == 404)
    return "NOT FOUND";
}

void CatClient (int in_fd, int out_fd)
{
  int bytes_rcvd = 1, bytes_sent = 0, i, j, cont = 0;
  unsigned char * buf = (char *) malloc (BUF_SIZE); 

  bzero(buf, sizeof(buf));
  bytes_rcvd = read (in_fd, buf, BUF_SIZE);
  write (out_fd, buf, bytes_rcvd);
  //  puts (buf);

  free (buf);
}

void CatServ (int in_fd, int out_fd)
{
  int bytes_rcvd = 1, bytes_sent = 0, i, j, cont = 0;
  unsigned char * buf = (char *) malloc (BUF_SIZE); 
  char * tipo; 
  tipo  = getTipo(in_fd);

  while (bytes_rcvd > 0)
  {
    bzero(buf, sizeof(buf));
    bytes_rcvd = read (in_fd, buf, BUF_SIZE);
    write (out_fd, buf, bytes_rcvd);
  }

  free (buf);
}

char* dirIP(char* serv){
  char * IP;
  struct hostent *he;

  if ((he=gethostbyname(serv))==NULL) {
    printf("error de gethostbyname()\n");
    exit(-1);
  }
  return (char *) inet_ntoa(*((struct in_addr *)he->h_addr));
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

void simpleRes(int in_fd){
  char * header = "HTTP/1.0 403\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: 354\n\n<html><head></head><body><h2>La pagina esta en la lista de paginas prohibidas, si necesita informacion de esa pagina, contacte al administrador del sistema</h2></body></html>";
  send (in_fd, header, strlen(header), 0);
}

void complexRes(int in_fd){
  char * header = "HTTP/1.0 403\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: 354\n\n<html><head></head><body><h2>La pagina esta contenida dentro de una pagina prohibida, si necesita informacion de esa pagina, contacte al administrador del sistema</h2></body></html>";
  send (in_fd, header, strlen(header), 0);
}

void showPage (int in_fd, char * host, char * IP, char* fecha, char* URL)
{
  int out_fd = connectToServer(host);

  //Chequeo de Paginas Prohibidas.
  if (strcmp(host, "www.google.com") == 0)    
    simpleRes(in_fd);
  else if  (strcmp(host, "www.yahoo.com") == 0)   
    complexRes(in_fd);
  else
  {
    CatClient(in_fd, out_fd);
    char * tipo = CatServ(out_fd, in_fd);
  };

  printf("%s %s %s %s", host, IP, fecha, URL);

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

FILE* crearLog(int numArgumentos, char** argv)
{
  int cont = 1;
  FILE * fd;
  while (cont < numArgumentos)
  {
    if  (strcmp(argv[cont],"-l") == 0)
    {
      if ((fd = fopen(argv[cont + 1],"a+")) != NULL)
      {
        return fd;
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
