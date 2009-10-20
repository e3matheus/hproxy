/*
 * hproxy.c
 *
 *  Created on: Oct 11, 2009
 *      Author: e3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "funciones.h"
#define maxArg 7
#define PORT 37261
#define QUEUELENGTH 5

int main(int argc, char** argv)
{
  char input[30], host[500], URL[500];
  int chequeo, pid, clientaddrlength, sockfd, newsockfd;
  char* puerto = chequearPuerto(argc, argv), ipCliente;
  FILE* fd = abreArchivoDirecciones(argc, argv);
  FILE* logFd;
  struct sockaddr_in clientaddr, serveraddr;

  chequearMaxArgumentos(argc);
  logFd = crearLog(argc, argv);

     char* request = "GET http://www.danasoft.com/citysign.jpg HTTP/1.0\nHost: www.danasoft.com\nUser-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.14) Gecko/2009090905 Fedora/3.0.14-1.fc10 Firefox/3.0.14 GTB5\nAccept: image/png,image/*;q=0.8,*/*;q=0.5\nAccept-Language: en-us,en;q=0.5\nAccept-Encoding: gzip,deflate\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\nConnection: close\nProxy-Connection: close\nReferer: http://www.ldc.usb.ve/~vtheok/\nPragma: no-cache\n";
     sscanf(request, "GET %s HTTP/1.0\nHost: %s\n", URL, host);
   printf ("el url es %s\n", URL);
   printf ("el host es %s\n", host);

  /* Open a TCP socket. */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    fatalerror("can't open socket");

  /* Bind the address to the socket. */
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(PORT);
  if (bind(sockfd, (struct sockaddr *) &serveraddr,
        sizeof(serveraddr)) != 0)
    fatalerror("can't bind to socket");
  if (listen(sockfd, QUEUELENGTH) < 0)
    fatalerror("can't listen");

  while (1) {
    /* Wait for a connection. */
    clientaddrlength = sizeof(clientaddr);
    newsockfd = accept(sockfd, 
        (struct sockaddr *) &clientaddr,
        &clientaddrlength);
    if (newsockfd < 0)
      fatalerror("accept failure");

    /* Fork a child to handle the connection. */
    pid = fork();
    if (pid < 0)
      fatalerror("fork error");
    else if (pid == 0) {
      ipCliente = inet_ntoa(clientaddr);
      /* I'm the child. */
      close(sockfd);
      getInfo(newsockfd, &URL, &host);
      showPage(newsockfd, host);
      exit(EXIT_SUCCESS);
    }
    else
      /* I'm the parent. */
      close(newsockfd);
    /*    while(1){
          printf("Que pagina desea abrir?\n");
          scanf("%s", input);
          if (strcmp((char *)input,"q") == 0)
          { 
          printf("Salida");
          break;}
          else if (strcmp((char *)input,"l") == 0)
          printf("Imprime Log");
          }
          */  }

    return 0;
}
