#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <fstream>

#define PORT 8080
using namespace std;

int main(){

    //Initiating socket values

    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket(domain, type, protocol);
    struct sockaddr_in address;
    int address_length = sizeof(address);
    if (server_fd < 0)
    {
        perror("cannot create socket");
        return 0;
    }

    //Assinging socket properties values

    address.sin_family = AF_INET;
    address.sin_addr.s_addr= htonl(INADDR_ANY); //htonl converts a long integer (eg address) to a network representation)
    address.sin_port = htons(PORT); //htons convert a short integer (port) to a network representation
    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    //Binding a socket to address
    bind(server_fd, (struct sockaddr *)&address, address_length);

    // Waiting for an incoming connection

    int new_socket;

    if (listen(server_fd, 3) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    while(true){
        printf("\n++++++ Waiting for new connection ++++++ \n\n");
        //accepting the connection from client and assigning it to new socket 
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address))<0) //here address gets filed in with address of the client that is doing the connect;
        {
            perror("In accept");            
            exit(EXIT_FAILURE);        
        }

        //Sending and recieving message

        //reading the client requested information through new socket
        char buffer[1024] = {0};
        cout << buffer;
        long valread = read( new_socket , buffer, 1024); 
        cout << buffer;
        string reqInfo = buffer;
        if(valread < 0)
        { 
            cout << ("No bytes are there to read");
        }

        string httpMethod = reqInfo.substr(0, reqInfo.find(" ")); // http method is stored before the first space
        int pos = reqInfo.find(" ");
        string route = reqInfo.substr(reqInfo.find(" ") + 1, reqInfo.find(" ", pos + 1) );

        if (httpMethod == "GET"){


            //initiating variables to construct the response header
            string myText;
            string status = "200";
            string contentType = "text/html";
            int contentLength;
            ostringstream responseHeader;

            // reading the html file and storing into a variable
            ifstream MyReadFile("index.html");
            if (!MyReadFile)
            {
                cerr << "Unable to open file";
                exit(1); // call system to stop
            }
            string content((istreambuf_iterator<char>(MyReadFile)),(istreambuf_iterator<char>())); //reads the file and store into variable
            MyReadFile.close();
            contentLength = content.length();

            //writing response header and file content to the new socket and sending back to client
            responseHeader << "HTTP/1.1 " << status << " OK\nContent-Type: " << contentType << "\nContent-Length: " << to_string(contentLength) << "\n\n"<<content;
            write(new_socket , responseHeader.str().c_str() , responseHeader.str().length());
            printf("-------file sent---------");
            close(new_socket);
        }
        
    }
    
    return 0;
    
};

