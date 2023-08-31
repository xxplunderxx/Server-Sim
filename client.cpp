//***************************************************************************
//
//  Jacob Eul
//  z1892587
//  csci 463 - section 1
//  date 12/02/22
//  Assignment #7
//
//  This program connects to a socket held by the server and writes to it
//  It aslo displays a message returned from the server.
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************

/*
Copyright (c) 1986, 1993
The Regents of the University of California.  All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
must display the following acknowledgement:
This product includes software developed by the University of
California, Berkeley and its contributors.
4. Neither the name of the University nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
Modifications to make this build & run on Linux by John Winans, 2021
*/

#include <iostream>
#include <iomanip>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DATA "Half a league, half a league . . ."

/**
 * @brief prints out a standard error message
 *
 * This function is printed when the program fails. This allows a
 * standard usage message to be displayed whenever necessary.
 * 
 ********************************************************************************************/
static void usage()
{
	std::cerr << "Usage: client [-s server-ip] server-port\n";
	std::cerr << "    -s Specify the server’s IPv4 number in dotted-quad format (default: 127.0.0.1)";
	std::cerr << "    The server port number to which the client must connect\n";
	exit(1);
}

/*
 * This program creates a socket and initiates a connection with the socket
 * given in the command line.  One message is sent over the connection and
 * then the socket is closed, ending the connection. The form of the command
 * line is streamwrite hostname portnumber
 */

static ssize_t safe_write(int fd, const char *buf, size_t len)
{
    while(len > 0)
    {
        ssize_t wlen = write(fd, buf, len);
        if(wlen == -1)
            return -1;  // unrecoverable error from write

        len -= wlen;    // reduce the ramining # of bytes sent by the # of bytes sent
        buf += wlen;    // increment the buffer to reflect the change in bytes sent
    }
    return len;
}

static int print_response(int fd)
{
    char buf[2048];
    ssize_t buf_len = 1;

    while(buf_len > 0){

    //keeps going until rval is 0
    if((buf_len = read(fd, buf, sizeof(buf))) == -1){
        perror("reading stream message");
        return -1;
    }
    else if (buf_len > 0){
        buf_len = write(fileno(stdout), buf, buf_len);
    }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int sock;                   // File descriptor for the socket connecting to the server
    struct sockaddr_in server;  // Socket adress for the server connection
    ssize_t len = 1;

    char buf[2048];
    char port_IP[255] = "127.0.0.1";
    int opt;
    while ((opt = getopt(argc, argv, "s:")) != -1)
	{
		switch(opt)
		{
			case 's': {
                strncpy(port_IP,optarg,255);
                break;
            }
			default:
				usage();
		}
	}

     /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        exit(1);
    }
     /* Connect socket using name specified by command line. */
    server.sin_family = AF_INET;
    
    if(inet_pton(AF_INET, port_IP, &server.sin_addr) <= 0)
    {
        fprintf(stderr, "%s: invalid address/format\n", argv[1]);
        exit(2);
    }

    server.sin_port = htons(atoi(argv[optind]));                 // copy host port number to port readable by server

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        perror("connecting stream socket");
        exit(1);
    }
    while(len != 0){ /// keeps reading till returns 0
      //256
      len = read(fileno(stdin), buf, sizeof(buf)); //reads the file sizeof(buf) default 2048
	   if(safe_write(sock, buf, len) < 0)           //write(fileno(stdout), buf, len);
         perror("writing on stream socket");       // -1 error checking
    }

    shutdown(sock, SHUT_WR);
    print_response(sock);

    close(sock);

    return 0;
}