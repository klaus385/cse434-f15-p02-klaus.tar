// Name of Author(s): Stephen Klaus
// Course Number and Name: CSE 434, Computer Networks
// Semester: Fall 2015
// Part: 2
// Client.c

//header files

//input - output declarations included in all C programs
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//contains definitions of a number of data types used in system calls
#include <sys/types.h>

//definitions of structures needed for sockets
#include <sys/socket.h>

//in.h contains constants and structures needed for internet domain addresses
#include <netinet/in.h>

//netdb.h defines the structure hostent,
#include <netdb.h>

//------------------------- Macro Defines Declerations -----------------------//

#define PORTNUM 8888 // Default Port Number
#define BUFFSIZE 256 // Default Buffer Size
#define MAXCLIENTSIZE 5 // Maximum number of Clients Allowed

//-------------------------- Prototype Declerations --------------------------//

int readIn(int sd, char *ptr, int size);
int writeIn(int sd, char *ptr, int size);

//This function is called when a system call fails. It displays a message about the error on stderr and then aborts the program.
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    // variable declarations

    // sockfd is a file descriptor
    // portno stores the port number on which the server accepts connections.
    
    int sockfd, portno, n, i, len, msg, getfile;
    int notWritable, start_xfer, numBlocks, lastNumBlocks;
    
    notWritable = 0;
    numBlocks = 0;
    lastNumBlocks = 0;

    // serv_addr will contain the address of the server
    
    struct sockaddr_in serv_addr, my_addr;
    
    // File Decleration
    
    FILE *fp;

    //variable server is a pointer to a structure of type hostent
    /*
     struct  hostent
     {
        char    *h_name;        // official name of host
        char    **h_aliases;    // alias list
        int     h_addrtype;     // host address type
        int     h_length;       // length of address
        char    **h_addr_list;  // list of addresses from name server
        #define h_addr  h_addr_list[0]  // address, for backward compatiblity
     };
     */
    
    struct hostent *server;

    char buffer[BUFFSIZE];

    // atoi() function can be used to convert port number from a
    // string of digits to an integer, if your input is in the form of a string.
    
    portno = PORTNUM;

    // create socket
    // it take three arguments - address domain, type of socket, protocol
    // (zero allows the OS to choose thye appropriate protocols
    // based on type of socket)
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    
    
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    
    
    
    // funcion gethostbyname() takes name as an argument and returns
    // a pointer to a hostent containing information about that host
    // name is taken as an argument from the user
    
    server = gethostbyname(argv[1]);
    
    len = strlen(argv[1]);
    
    // printf("Creating Server \n"); // Might not be Correct Statement

    // If hostent structure is NULL (after the above assignment),
    // the system could not locate a host with this name
    
    
    
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    
    
    // set server address buffer with zeros using bzero or memset
    // two arguments - pointer to buffer and sizeof buffer
    
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //following code sets the fields in serv_addr
    //contains a code for the address family
    
    serv_addr.sin_family = AF_INET;

    // copies length bytes from s1 to s2
    
    
    
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    

    // contain the port number
    
    serv_addr.sin_port = htons(portno);

    // connect function is called by the client to establish a connection
    // to the server. It takes three arguments, the socket file descriptor,
    // the address of the host to which it wants to connect
    // (including the port number), and the size of this address
    
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    
    getfile = htons(100);
    
    printf("Client: Sending Command to Server\n");
    
    if((writeIn(sockfd,(char *)&getfile,sizeof(getfile))) < 0)
    {
        error("Client: Write Error");
    }
    
    // Getting Ok from server
    
    msg = 0;
    
    if((readIn(sockfd,(char *)&msg,sizeof(msg)))< 0)
    {
        error("Client Read Error");
    }
    msg = ntohs(msg);
    
    if (msg == 150)
    {
        error("Client Refused the Command");
    }
    else
    {
        error("Client: Command Accepted");
    }
    
    // printf("Please enter the message: "); - Old way to transmit Data
    
    // Implemetnint New Client FTP
    
    /* send file name to server */
    printf("Client: Sending filename\n");
    
    if ((writeIn(sockfd,argv[1],len))< 0)
    {
        error("Client: Write Error");
    }
    /* see if server replied that file name is OK */
    int msg_2 = 0;
    if ((readIn(sockfd,(char *)&msg_2,sizeof(msg_2)))< 0)
    {
        error("Client Read Error");
    }
    msg_2 = ntohs(msg_2);
    if (msg_2 == 200)
    {
        error("Client: Bad File Name ");
    }
    else
        printf("client: server replied %d, filename OK\n",msg_2);
    
    printf("Client: Sending Data to Server \n ");
    
    fp = fopen(argv[1], "w");
    
    if (fp == NULL)
    {
        error("File not Found");
    }
    
    //if ((readIn(sockfd, (char *)&numBlocks, sizeof(numBlocks))))
    //{
        //error("Read Error");
    //}
    
    //numBlocks = ntohs(numBlocks); // May Cause an Issue
    
    //printf("Client: Server Responded: %d blocks in file\n",numBlocks);
    
    //if((readIn(sockfd,(char *)&lastNumBlocks,sizeof(lastNumBlocks))) < 0)
    //{
      //  error("Client Read Error");
    //}
    
    //lastNumBlocks = ntohs(lasDtNumBlocks);
    
    //printf("client: server responded: %d bytes last blk\n",lastNumBlocks);
    
    printf("Start to Recieve File Contents \n");
    
    for (i = 0; i < numBlocks; i++)
    {
        //if((readIn(sockfd,buffer,BUFFSIZE)) < 0)
        //{
          //  error("Client: Blocked Error Read");
        //}
        
        notWritable = fwrite(buffer, sizeof(char), BUFFSIZE, fp);
        
        if (notWritable == 0)
        {
            error("File Write Error \n");
        }
        
        /*
        
        if (notWritable != BUFFSIZE)
        {
            
        }
         
        */
        
        //printf(" d...", i);
    }
    
    if (lastNumBlocks > 0)
    {
        printf("%d\n", numBlocks);
        
        /*
        if((readIn(sockfd,buffer,lastNumBlocks)) < 0)
        {
            error("Client: Unable to Read Last Part of File");
        }
        */
        notWritable = fwrite(buffer, sizeof(char), BUFFSIZE, fp);
        
        if (notWritable == 0)
        {
            error("File Write Error at EOF!");
        }
        
        /*
         
         if (notWritable != BUFFSIZE)
         {
         
         }
         
         */
        
    }
    
    // Old Read and Write

    /*
    
    // After a connection a client has successfully connected to the server
    // initialize the buffer using the bzero() function
    
    bzero(buffer,BUFFSIZE);

    // set the buffer to the message entered on console at client end for a maximum of 255 characters
    
    fgets(buffer,(BUFFSIZE-1),stdin);

    // write from the buffer into the socket
    
    n = write(sockfd,buffer,strlen(buffer));

    // check of the write function was successful or not
    
    if (n < 0)
    {
         error("ERROR writing to socket");
    }
     
    // both the server can read and write
    // after a connection has been established.
    // everything written by the client will be read by the server,
    // and everything written by the server will be read by the client.
    
    bzero(buffer,BUFFSIZE);
    n = read(sockfd,buffer,255);
    
    if (n < 0)
    {
         error("ERROR reading from socket");
    }
    printf("%s\n",buffer);
     
    */

    // close connections using file descriptors
    fclose(fp);
    close(sockfd);
    printf("Client: FILE TRANSFER COMPLETED \n");
    
}


// Read In File

int readIn(int sd, char *ptr, int size)
{
    int notReadable, tempSize, totalSize;
    
    tempSize = size;
    
    while (tempSize > 0)
    {
        notReadable = read(sd, ptr, tempSize);
        
        if (notReadable < 0)
        {
            return notReadable;
        }
        if (notReadable == 0)
        {
            break;
        }
        
        tempSize = tempSize - notReadable;
        ptr = ptr + notReadable;
    }
    
    totalSize = size - notReadable;
    
    return totalSize;
    
} // End of Read In Function

// Write in File

int writeIn(int sd, char *ptr, int size)
{
    int notWritable, tempSize, totalSize;
    
    while (tempSize > 0)
    {
        notWritable = write(sd, ptr, tempSize);
        
        if (notWritable <=0)
        {
            return (notWritable);
        }
        
        tempSize = tempSize - notWritable;
        ptr = ptr + notWritable;
    }
    
    totalSize = size - notWritable;
    
    return totalSize;
    
} // End of Write In Fuction
