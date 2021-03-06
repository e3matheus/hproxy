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
  char host[500], URL[500]; 
  char * fechaC;
  int chequeo, pid, clientaddrlength, sockfd, newsockfd;
  char* puerto = chequearPuerto(argc, argv), tipo;
  listForbidden = (char *) abreArchivoDirecciones(argc, argv);
  FILE* logFd;
  struct sockaddr_in clientaddr, serveraddr;
  time_t lt;

  chequearMaxArgumentos(argc);
  logFd = crearLog(argc, argv);

  /* Open a TCP socket. */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    fatalerror("can't open socket");

  /* Bind the address to the socket. */
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(atoi(puerto));
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
      /* I'm the child. */
      close(sockfd);
      char * ipCliente = (char *) inet_ntoa(clientaddr.sin_addr);
      lt = time(NULL);
      fechaC = ctime(&lt); 
      getInfo(newsockfd, &URL, &host);
      showPage(logFd, newsockfd, &host, ipCliente, fechaC, URL);
     
      exit(EXIT_SUCCESS);
    }
    else
      /* I'm the parent. */
      close(newsockfd);
  }

  return 0;
}
