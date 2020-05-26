#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 10000
#define SA struct sockaddr

void sendFile(int sockfd);

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in serveraddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    }
    else {
        printf("Socket successfully created...\n");
    }

    memset(&serveraddr, 0, sizeof(serveraddr));

    // Assign IP, PORT;
    serveraddr.sin_family = AF_INET;
    //inet_pton(AF_INET, SERVER_ADDRESS, &(serveraddr.sin_addr));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //serveraddr.sin_addr.s_addr = inet_addr("192.168.0.101");
    serveraddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&serveraddr, sizeof(serveraddr))) != 0) {
        fprintf(stderr, "Error on bind --> %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }
    else {
        printf("Socket successfully binded...\n");
    }
    
    // Now server is ready to listen and verification 
    if((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else {
        printf("Server listening...\n");
    }

    len = sizeof(cli);

    // Accept the data packet from client and verification
    for(;;) {
        connfd = accept(sockfd, (SA*)&cli, &len);

        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        }
        else {
            printf("server acccept the client...\n"); 
        }

        sendFile(connfd); 

        close(connfd);
    }
    

    close(sockfd); 

    return 0;
}

void sendFile(int sockfd) {
    int fd, remain_data, sent_bytes;
    ssize_t len;
    off_t offset;
    struct stat file_stat;
    char file_size[BUFSIZ];
    char buffer[BUFSIZ];

    recv(sockfd, buffer, BUFSIZ, 0);
    
    
    printf("%s \n", buffer);
    fd = open(buffer, O_RDONLY);

    if (fd == -1)
    {
        strcpy(buffer,"File is non existant");

        send(sockfd, buffer, sizeof(buffer), 0);

        return;
    }

    /* Get file stats */
    if (fstat(fd, &file_stat) < 0)
    {
        fprintf(stderr, "Error fstat");

        exit(0);
    }

    printf("File Size: %ld bytes\n", file_stat.st_size);

    sprintf(file_size, "%ld", file_stat.st_size);

    len = send(sockfd, file_size, sizeof(file_size), 0);

    if (len < 0)
    {
        printf("Error on sending greetings\n");

        exit(0);
    }

    printf("Server sent %ld bytes for the size\n", len);

    offset = 0;
    remain_data = file_stat.st_size;

    while ((sent_bytes = sendfile(sockfd, fd, &offset, BUFSIZ)) > 0 && (remain_data > 0))
    {
        printf("1. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
        remain_data -= sent_bytes;
        printf("2. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
    }
}