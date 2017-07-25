/* 
 *
 *
 * Name: IPv6 Talker
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

#define LISTENER_PORT "7777" // SRv6 Listener is listening on this port.

int main(int argc, char *argv[])
{
   int sockfd;
   struct addrinfo hints, *ll_hints, *p;
   int ret_val, bytes_sent;

   if (argc != 3) {
      fprintf(stderr, "Usage: talker hostname message\n");
      exit(EXIT_FAILURE);
   }

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET6;
   hints.ai_socktype = SOCK_DGRAM;

   ret_val = getaddrinfo(argv[1], LISTENER_PORT, &hints, &ll_hints);
   if (ret_val != 0) {
      fprintf(stderr, "srv6_talker: getaddrinfo error - %s\n", gai_strerror(ret_val));
      exit(EXIT_FAILURE);
   }
  
   // loop through the linked-list of result structures.
   for (p = ll_hints; p != NULL; p = p->ai_next) {
      sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd == -1) {
	 perror("srv6_talker: socket.");
	 continue;
      }
      break; 
   } 

   if (p == NULL) {
      fprintf(stderr, "talker: failed to create socket\n");
      exit(EXIT_FAILURE);
   }

   bytes_sent = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen);
   if (bytes_sent == -1) {
      perror("srv6_talker: sendto.");
      exit(EXIT_FAILURE);
   }

   freeaddrinfo(ll_hints);
   
   printf("srv6_talker sent %s - %d bytes - to %s\n", argv[2], bytes_sent, argv[1]);
   close(sockfd);
    
   return 0;
}
