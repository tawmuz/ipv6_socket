/* 
 *
 *
 * Name: IPv6 Listener
 * Author: Thomas Modayil Jacob 
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LISTENER_PORT "7777"
#define MAXBUFLEN 256

// return sockaddr, IPv4 or IPv6.
void *get_sock_addr(struct sockaddr *sa) 
{
   if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in *)sa)->sin_addr);
   }

   return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
   int sockfd;
   struct addrinfo hints, *ll_hints, *p;
   int ret_val;
   struct sockaddr_storage client_address;
   socklen_t addrlen;
   int bytes_recv;
   char recv_buf[MAXBUFLEN];
   char s[INET6_ADDRSTRLEN];
   
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET6;
   hints.ai_socktype = SOCK_DGRAM;
   hints.ai_flags = AI_PASSIVE;  
   
   ret_val = getaddrinfo(NULL, LISTENER_PORT, &hints, &ll_hints);
   if (ret_val != 0) {
      fprintf(stderr, "srv6_listener: getaddrinfo error - %s\n", gai_strerror(ret_val));
      exit(EXIT_FAILURE);
   }
   
   // loop through the linked-list of result structures.
   for (p = ll_hints; p != NULL; p = p->ai_next) {
      sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd == -1) {
	 perror("srv6_listener: socket.");
	 continue;
      }
      else {
	 if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("srv6_listener: bind.");
	    continue;
	 }
      }
      break;
   }

   freeaddrinfo(ll_hints);
      
   if (p == NULL) {
      fprintf(stderr, "srv6_listener: failed to create and/or bind to socket.\n");
      exit(EXIT_FAILURE);
   }

   printf("srv6_listener: waiting to hear a hello from the other side...not you adele.\n");
   addrlen = sizeof(client_address);

   bytes_recv = recvfrom(sockfd, recv_buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&client_address, &addrlen);
   if (bytes_recv == -1) {
      perror("srv6_listener: recvfrom.");
      exit(EXIT_FAILURE);
   }

   printf("srv6_listener: received packet from %s\n",
	  inet_ntop(client_address.ss_family, get_sock_addr((struct sockaddr *)&client_address), s, sizeof(s)));

   printf("srv6_listener: packet is %d bytes long\n", bytes_recv);
   recv_buf[bytes_recv] = '\0';
   printf("srv6_listener: packet contains \"%s\"\n", recv_buf);

   close(sockfd);
   
   return 0;
}
