#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IP "10.0.3.1"
#define PORT 12345

int main(int argc, char *argv[])
{
  struct sockaddr_in  dest, src;
  int                 fd;
  char                message[128];
  char               *group;
  int                 port;
  int                 count;
  size_t              length, recvd;
  socklen_t srclen = sizeof(src);

  if (argc != 3 ) {
    group = IP;
    port = PORT;
  } else {
    group = argv[1];
    port = atoi(argv[2]);
  }

  /* 1. Set up destination */
  dest.sin_family = AF_INET;
  dest.sin_port = htons(port);
  // inet_aton(group, &dest.sin_addr);
  dest.sin_addr.s_addr  = htonl(INADDR_ANY);

  /* create what looks like an ordinary UDP socket */
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket");
    exit(1);
  }

  if (bind(fd, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
    perror("bind");
    close(fd);
    return -1;
  }

  printf("Unicast reader on %s:%d\n", group, port);

  while(1){
    recvd = recvfrom(fd, message, sizeof(message), 0,
                        (struct sockaddr*)&src, &srclen);
    if (recvd < 0) {
      perror("recvfrom");
      break;
    }
    if (recvd >= (ssize_t)sizeof(message)){
      recvd = sizeof(message)-1;
      return -1;
    } 
    message[recvd] = '\0';

    printf("rx_udp: %zd bytes from %s:%d: %s",
           recvd, inet_ntoa(src.sin_addr), ntohs(src.sin_port), message);

    sleep(1);
  }
  return 0;
}
