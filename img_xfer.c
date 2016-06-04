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

/*----- Function prototypes -----*/
void delay(int millisec_delay);

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
  
  /*----- Request image -----*/
  
  /* create image request message according to VCRT */
  senddata[0] = (unsigned int) 0; /* x */
  senddata[1] = (unsigned int) 0; /* y */
  senddata[2] = (unsigned int) dx; /* image width px */
  senddata[3] = (unsigned int) dy; /* image height px */
  senddata[4] = (unsigned int) 1; /* x increment */
  senddata[5] = (unsigned int) 1; /* y increment */
  
  /* wait 5 seconds before requesting images */
  delay(2000); /* 2 seconds for debugging */
  
  /* loop to repeatedly request images over TCP/IP, and save them */
  int i;
  for (i = 0; i<10; i++ ) {
    /* send image request, 24 bytes of pixel start, image size and skip pixels */
    send_result = send(sockfd, (char *) senddata, 24, 0);
    /*printf("Image request sent to camera\n");*/
    
    if (send_result != 24) {
      printf("SEND ERROR: wrong length. 24 bytes required, sent %d bytes to camera.\n", send_result);
    }
    
    /* receive image buffer, MSG_WAITALL to wait for full image */
    /*printf("Receiving data\n");*/
    receive_result = recv(sockfd, (char *) linebuffer, 28+dx*dy, MSG_WAITALL);
    if (receive_result != 28+dx*dy) {
      printf("RECEIVE ERROR: wrong length. %d bytes expected, %d bytes received.\n", 28+dx*dy, receive_result);
    }
    /* printf("%d -- %d  \n", i, receive_result); */
    
    cpy_img_buffer = linebuffer+28; /* image starts 28 characters into the message */
    
    /*----- generate image filename -----*/
    
    /* from YYYY-MM-DD-HH-MM-SS.ss */
    gettimeofday(&tv, NULL);
    char time_buffer[26];
    millisec = (long) (tv.tv_usec/1000.0 + 0.5); // Round to nearest millisec
    
    tm_info = localtime(&tv.tv_sec);
    
    strftime(time_buffer, 26, "%Y-%m-%d_%H-%M-%S-", tm_info);
    
    char milli_str[4];
    sprintf(milli_str, "%03d", millisec);
    
    char img_path[] = "/home/odroid/VCSBC360_camera/images/";
    
    char img_filename[28];
/*    *img_filename = strcat( strcat( strcat( img_path, time_buffer ), milli_str ), img_extnsn );*/
    strcpy(img_filename,time_buffer);
    strcpy(img_filename+20,milli_str);
    strcpy(img_filename+23,img_extnsn);
    
    char img_miraclename[strlen(img_filename)+strlen(img_path)-1];
    strcpy(img_miraclename,img_path);
    strcpy(img_miraclename+strlen(img_path),img_filename);
    
    
    printf("%s\n",img_miraclename);
    /*sprintf(img_filename_1, "%s\n", strcat( milli_str, img_extnsn ) );*/
    /*sprintf(img_filename_2, "%s\n", strcat( time_buffer, img_filename_1 ) );*/
    
    /*printf("%s \n", (char *) img_filename);*/
    
    
    /* sequential: 0001, etc.*/
    /*	    img_filename = strcat( strcat(img_filename, itoa(i+1)), ".bmp");*/
    /*img_filename = "image1.bmp";*/
    
    
    //img_filename = strcat();

    /*----- save image -----*/

    img_file_ptr = fopen( img_miraclename, "wb" );
    
    /* write BMP header */
    numbytes_written = fwrite(bmp_header, 1 , HEADER_SIZE , img_file_ptr );
    
    if (numbytes_written != HEADER_SIZE) {
      printf("ERROR: file failed to write header\n");
    }
    
    /* write image */
    numbytes_written = fwrite(cpy_img_buffer , 1 , dx*dy , img_file_ptr );
    
    if (numbytes_written != dx*dy) {
      printf("ERROR: file failed to write image\n");
    }
    
    fclose(img_file_ptr);
    
  }

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

/* Delay function in milliseconds */
void delay(int millisec_delay) {
  if (millisec_delay > 0) {
    long pause;
    clock_t now,then;
    
    pause = millisec_delay*(CLOCKS_PER_SEC/1000);
    now = clock();
	then = clock();
    
    while( (now-then) < pause ) {
      now = clock();
    }
  }
}
