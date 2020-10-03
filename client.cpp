

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

class Client
{
private:
	int sockfd, port_number;
	struct sockaddr_in serv_addr; //address of server to connect to
	struct hostent *server;		  //pointer to host computer on internet
	char read_server[64];		  //buffer to read data from server
	char send_server[64];		  //buffer to write to server
	char hostname[32];			  //store localhost

public:
	/*
		Initializes all variables to the value 0
	*/
	Client();
	/*
		Gets port number inputted by user, if it's not between 2000-65535, it prompts
		user to try again (must match same port number as server), Also takes in the input
		of the hostname (localhost)
	*/
	void GetHostnamePort();
	/*
		Create socket and store return value in file descriptor initialize buffer containing
		information regarding the internet address of the server, and connect to that server 
		using the socket and the buffer
	*/
	void CreateSocketConnect();
	/*
		Get the name of the city as user input to send to the server
			*/
	bool GetSendingData();
	/*
		Write the buffer containing the name of the city to the server, and read in the response
		send by the server. Log either the result or the error message sent by the server.
		Close the connection made using the file descriptor retreived on socket creation.
		
	*/
	void ReadWriteServer();
};

int main()
{

	Client cli;
	cli.GetHostnamePort();
	cli.CreateSocketConnect();
	cli.GetSendingData();
	cli.ReadWriteServer();

	return 0;
}

Client::Client()
{
	port_number = sockfd = 0;
}
void Client::GetHostnamePort()
{
	std::cout << "Enter the server host name: ";
	std::cin >> hostname;

	//get localhost
	server = gethostbyname(hostname);
	if (server == NULL)
	{
		std::cout << "ERROR, no such host\n";
		exit(1);
	}

	//input port number
	while ((port_number < 2000) || (port_number > 65535))
	{
		std::cout << "Enter the server port number: ";
		std::cin >> port_number;
	}
}

void Client::CreateSocketConnect()
{
	//open socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "Error creating socket\n";
		exit(1);
	};

	//initialize and set server internet address buffer
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_number);

	//connect to server
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cout << "ERROR Connecting\n ";
		exit(1);
	}
}
bool Client::GetSendingData()
{
	
	//intialize and fill buffer which server will read from
	bzero(send_server, 64);
	std::cout << "Enter a city name: ";

	//fill buffer to send to server with user input
	int c;
	while ((c = getchar()) != '\n' && (c != EOF))
		;
	fgets(send_server, 64, stdin); //copy from stdin to buffer to send to server

	//if user is done, no input
	if (send_server[0] == '\n')
	{
		std::cout << "Have a Nice Day" << std::endl;
		exit(0);
	}
	ungetc('\n', stdin);
	send_server[strlen(send_server) - 1] = '\0';
}

void Client::ReadWriteServer()
{
	//write to server
	if (write(sockfd, send_server, strlen(send_server)) < 0)
	{
		std::cout << "ERROR writing to  socket\n ";
		exit(1);
	}

	//read response from server
	bzero(read_server, 64);
	if (read(sockfd, read_server, 64) < 0)
	{
		std::cout << "ERROR reading from socket\n ";
		exit(1);
	}

	//std::cout << read_server << std::endl;

	//pull data from buffer
	int temp;
	char descr[64];
	char descr1[64];
	bzero(descr, 64);
	bzero(descr1, 64);
	sscanf(read_server, "%d %s %s", &temp, descr, descr1);

	//display if server sent error or success
	if (temp == -999 && strcmp(descr, "e") == 0)
	{
		std::cout << "No data" << std::endl;
	}
	else
	{
		std::cout << "Tomorrow's maximum temperature is " << temp << " F\n";
		std::cout << "Tomorrow's sky condition is " << descr << " " << descr1 << std::endl;
	}

	std::cout << std::endl;
	close(sockfd);
}