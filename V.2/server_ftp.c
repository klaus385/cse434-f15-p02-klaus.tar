// Name of Author(s): Stephen Klaus
// Course Number and Name: CSE 434, Computer Networks
// Semester: Fall 2015
// Part: 2
// Server.c

/* A simple server in the internet domain using TCP
*/

//header files

//input - output declarations included in all C programs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h> // Implementing Error Codes

//contains definitions of a number of data types used in system calls
#include <sys/types.h>

//definitions of structures needed for sockets
#include <sys/socket.h>

//in.h contains constants and structures needed for internet domain addresses
#include <netinet/in.h>

//------------------------- Macro Defines Declerations -----------------------//

#define PORTNUM 8888 // Default Port Number
#define BUFFSIZE 256 // Default Buffer Size
#define MAXCLIENTSIZE 5 // Maximum number of Clients Allowed

//-------------------------- Prototype Declerations --------------------------//

// Old connection Handler basic String Communication

// void connectionHandler(int sock);

// New FTP Communication Function Handler

void ftpConnectionHandler(int newsockfd);

// This function is called when a system call fails. It displays a
// message about the error on stderr and then aborts the program.

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Main Function Decleration

int main(int argc, char *argv[])
{
    // variable declarations

    // sockfd and newsockfd are file descriptors,These two variables store
    // the values
    // returned by the socket system call and the accept system call.
    // portno stores the port number on which the server accepts connections.

    int sockfd, newsockfd, portno, pid, n;

    // clilen stores the size of the address of the client.
    // This is required for the accept system call.

    socklen_t clilen;

    // server reads characters from the socket connection into this buffer.

    char buffer[BUFFSIZE];

    // sockaddr_in is a structure containing an internet address

    /*
     struct sockaddr_in
     {
        short   sin_family; //must be AF_INET
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8]; // Not used, must be zero
     };
     */

    // in_addr structure, contains only one field, a unsigned long called s_addr.

    // Checking if Input Socket is actually provided

    if (argc < 2)
    {
        fprintf(stderr, "ERROR no port provided \n");
        exit(1);
    }

    // serv_addr will contain the address of the server, and cli_addr will
    // contain the address of the client which connects to the server.

     struct sockaddr_in serv_addr, cli_addr;

    // create socket
    // it take three arguments - address domain, type of socket, protocol
    // (zero allows the OS to choose thye appropriate protocols
    // based on type of socket)

     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     if (sockfd < 0)
     {
        error("ERROR opening socket");
     }

    //set server address buffer with zeros using bzero or memset
    //two arguments - pointer to buffer and sizeof buffer

    bzero((char *) &serv_addr, sizeof(serv_addr));

    // atoi() function can be used to convert port number from a string of
    // digits to an integer, if your input is in the form of a string.

    portno = PORTNUM;

    // contains a code for the address family

    serv_addr.sin_family = AF_INET;

    // contains the IP address of the host

    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //contain the port number

    serv_addr.sin_port = htons(portno);

    // bind() system call binds a socket to an address, in this case the address
    // of the current host and port number on which the server will run.
    // three arguments, the socket file descriptor, the address to which is
    // bound, and the size of the address to which it is bound.

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    // listen system call allows the process to listen on the
    // socket for connections.
    // The first argument is the socket file descriptor, and second is number
    // of connections that can be waiting while the process is handling a
    // particular connection.

     listen(sockfd,MAXCLIENTSIZE);

     clilen = sizeof(cli_addr);

    // Statement to Display Ftp Started

    printf("Welcome to CSE 434 FTP Client/Server");

    // Infinite Loop to Create a Child to handle the functions
    // Then Wait for Next Connection to the Socket

    for(;;)
    {
        // accept() system call causes the process to block until
        // a client connects to the server.

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0)
        {
            error("ERROR on accept");
            exit(0); // Dont Know if this is Needed.
        }

        pid = fork();

        if (pid < 0)
        {
            error("ERROR on fork");
            exit(0); // Dont Know if this is Needed.
        }
        if (pid == 0)
        {
            close(sockfd);

            // connectionHandler(newsockfd); // Needs to Change to ftp

            // Implementing new FTP Service

            ftpConnectionHandler(newsockfd);

            exit(0);
        }
        else
        {
            // close connections using file descriptors

            close(newsockfd);
        }

    } // End of Infinite For Loop

     if (newsockfd < 0)
     {
         error("ERROR on accept");
         exit(0);
     }

    // close connections using file descriptors

     close(sockfd);

     return 0;
}

// Function to Allow for Handling Each Connection [UPDATE: to FTP]

void ftpConnectionHandler(int newsockfd)
{
    // Varible Declerations for FTP Service

    int i, c, req;
    int fileSize;
    int numBlocks, numBlocksTemp, lastNumBlocks, lastNumBlocksTemp;
    int notReadable;

    // Varible Declerations for Y/N Cases


    // Setting Up Blocks for Reading File Line By Line

    numBlocks = 0;
    lastNumBlocks = 0;
    notReadable = 0;

    // File Varible Decleration

    FILE *fp;
    char fname[BUFFSIZE];
    char ftp_buffer[512];

    // Start Implementing FTP Sever for Client

    if ((read(newsockfd, fname, BUFFSIZE)) < 0)
    {
        error("Unable to Read File Name \n");
    }

    // Opening FIle

    fp = fopen(fname, "r");

    // Unable to Open File

    if (fp == NULL)
    {
        error("File Not Found \n");
    }

    printf("Server: Your File Name is %s\n", fname);

    printf("Beginning to Transfer File \n");

    fileSize = 0;

    c = getc(fp);

    while (c != EOF)
    {
        fileSize++;
    }

    numBlocks = fileSize / 512;
    numBlocksTemp = htons(numBlocks);
    lastNumBlocks = fileSize % 512;
    lastNumBlocksTemp = htons(lastNumBlocks);
    rewind(fp);

    // Start to Transfer the Actual File
    for (i = 0; i < numBlocks; i++)
    {
        notReadable = fread(ftp_buffer, sizeof(char), 512, fp);

        if (notReadable == 0)
        {
            error("Server: File Read Error");
        }
        if (notReadable != 512)
        {
            error("Server: File Read Error");
        }

        printf("%d...", i); // May not be needed

        if (lastNumBlocks > 0)
        {
            printf("%d\n", numBlocks);
            notReadable = fread(ftp_buffer, sizeof(char), lastNumBlocks, fp);

            if (notReadable == 0)
            {
                error("Server: File Read Error");
            }
            if (notReadable != lastNumBlocks)
            {
                error("Server: File Read Error");
            }
        }
    }

    // Closing File and Socket

    printf("File Done Transferring on the Socket");

    fclose(fp); // File Closed
    close(newsockfd); // Socket Closed

} // End of FTP Service Setup
