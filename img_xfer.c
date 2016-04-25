/*****************************************************************************/
/*  TCP client for transferring images from VCSBC360 camera to client        */
/*                                                                           */
/*  Rikky Duivenvoorden -- April 22, 2016                                    */
/*  UTAT UAV                                                                 */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define MEMORY 1500000 /* Memory to store one image */
#define HEADER_SIZE 1078 /* Size of BMP file header in bytes */

/*****************************************************************************/

int main(void){
  /*----- Define variables -----*/
  int connect_error;
  unsigned int dx, dy;
  
  FILE *hdr_file_ptr, *img_file_ptr;
  unsigned char bmp_header[HEADER_SIZE]; /* VCSBC360 header pointer (size of 1078 bytes) */
  
  char img_filename[] = "image";
  char img_extnsn[] = ".bmp";
  
  dx = 3008; /* image width */
  dy = 480; /* image height */
  
  /*----- Define socket struct -----*/
  struct sockaddr_in servAddr;
  
  /*----- Allocate memory for raw image data -----*/
  linebuffer = (unsigned char *)calloc(MEMORY, 1); /* clear allocated memory, one byte per pixel */
  
  /*----- Read VCSBC360 BMP header file -----*/
  /* read template header file (created using saved image header from atxClient) */
  hdr_file_ptr = fopen("VCSBC360_hdr.bmp", "rb");
	printf("%d \n", fread(bmp_header, 1, HEADER_SIZE, hdr_file_ptr));
	fclose(hdr_file_ptr);
	
  /*----- Create client TCP socket -----*/
  int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  printf("\n%d\n", sockfd);
  
  /* check if socket was created successfully */
  if (sockfd < 0) {
    printf("Create stream socket failed\n");
    return 0;
  }
  
  /*----- Establish connection to server -----*/
  /* reset servAddr struct */
  memset(&servAddr, 0, sizeof(servAddr));
  
  /* assign IP address and port number */
  servAddr.sin_family = AF_INET; /* Internet address family */
  servAddr.sin_addr.s_addr = inet_addr("192.168.0.65"); /*0xc0a80041; VCSBC360 on: 192.168.0.65 */
  servAddr.sin_port = htons(2002); /* Server port */
  
  printf("%d\n", servAddr.sin_port);
  
  /* Connect to VCSBC360 server*/
  printf("\nConnecting to server: 0x%08x\n", servAddr.sin_addr.s_addr);
  connect_error = connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr));
  
  /* check for proper connection */
  if (connect_error != 0) {
    printf("\nConnect failed - 0x%x\n", connect_error);
    return 0;
  }
  
  printf("\nRemote Ethernet port: %d\n",servAddr.sin_port);
  
  /*----- Request and Save Images -----*/
  /* */
  /*----- -----*/
  
  
  free(linebuffer);
  
  /*----- Close socket -----*/
  int status = shutdown(sockfd, 1); /* close connection by disallowing further sends*/
  /*printf("\n%d\n", status);*/
  
  if (status != 0) {
    printf("Close socket failed\n\n");
    return 0;
  }
  else{
    printf("Stream socket successfully closed\n\n");
    return 0;
  }
}
