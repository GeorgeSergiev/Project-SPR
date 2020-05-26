#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT 10000
#define SA struct sockaddr
#define SERVER_ADDRESS "127.0.0.1"


void receiveFile(int sockfd, const char* filename) {
    int file_size, receivedFile, remain_data;
    ssize_t len;
    char buffer[BUFSIZ];    

    strcpy(buffer,filename);

    send(sockfd, buffer, sizeof(buffer), 0);
    
    recv(sockfd, buffer, BUFSIZ, 0);
    if(strcmp("File is non existant",buffer) == 0)
    {
        printf("File is non existant");
        exit(0);
    }
    
    file_size = atoi(buffer);
    receivedFile = open(filename, O_RDWR | O_CREAT, 0777);

    if (receivedFile < 0)
    {
        printf("Failed to open file foo\n");

        exit(0);
    }

    remain_data = file_size;

    while ((remain_data > 0) && ((len = recv(sockfd, buffer, BUFSIZ, 0)) > 0)) {
        printf("%s",buffer);
        write(receivedFile, buffer, len);
        remain_data -= len;
        printf("Receive %ld bytes and we hope :- %d bytes\n", len, remain_data);
    }

    close(receivedFile);
}

int main() {
    int sockfd;
    struct sockaddr_in serveraddr, cli;
    char filename[BUFSIZ];
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { 
        printf("Socket creation failed...\n"); 
        exit(0); 
    } 
    else {
        printf("Socket successfully created...\n"); 
    }
    
    memset(&serveraddr, 0, sizeof(serveraddr));

    // assign IP, PORT 
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serveraddr.sin_port = htons(PORT); 

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&serveraddr, sizeof(serveraddr)) != 0) { 
        printf("Connection with the server failed...\n");
        printf("%s\n", strerror(errno));
        exit(0); 
    } 
    else {
        printf("Connected to the server...\n"); 
    }
    fgets(filename, sizeof(filename), stdin);
    strtok(filename,"\n");
    printf("%s", filename);
    receiveFile(sockfd,filename);

    // close the socket
    close(sockfd);
    
    return 0;
}
