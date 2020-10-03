

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <tuple>
#include <list>
#include <string>

class Server
{
private:
	struct Input
	{
		char name[64];
		int temp;
		char description[64];
	};

	std::list<Input> weather; //store input
	int socket_file_descriptor, newsocket_file_descriptor, clilen, port_number;
	struct sockaddr_in serv_addr, client_address; //store server and client information
	char read_client[64];						  //buffer to read data in front client
	char send_client[64];						  //buffer to send data to client

public:
	/*
		Initializes all variables to the value 0
	*/
	Server()
	{
		port_number = socket_file_descriptor = newsocket_file_descriptor = clilen = 0;
	};
	/*
		Reads in data from file "weather20.txt", creates an Input Struct for
		each city, containings it's name, the temperature, and the description to send
		back to client. Stores each Struct created for each city, in a linked list named
		weather. 
	*/
	void GetFileInput()
	{
		FILE *fp;
		//file to open
		char fileName[64] = "weather20.txt";
		fp = fopen(fileName, "r");
		if (fp == NULL)
		{
			fprintf(stderr, "ERROR, file does not exist\n");
			exit(1);
		}

		//read in file line by line until it's done
		while (1)
		{
			Input *input = new Input();
			bzero(input->description, 64);
			bzero(input->name, 64);

			int i = 0;
			char c;
			while ((c = getc(fp)) != ',' && (c != EOF) && (i != 64))
			{
				input->name[i] = c;
				i++;
			}

			fscanf(fp, "%d ", &input->temp);

			c = getc(fp); //move to description

			//store 64 bits into buffer
			i = 0;
			while ((c = getc(fp)) != '\n' && (c != EOF) && (i != 64))
			{
				input->description[i] = c;
				i++;
			}
			if (c == EOF)
			{
				break;
			}
			weather.push_back(*input);
			delete input;
		}
		fclose(fp);
	};
	/*
		Gets port number inputted by user, if it's not between 2000-65535, it prompts
		user to try again
	*/
	void GetPort()
	{

		while ((port_number < 2000) || (port_number > 65535))
		{
			std::cout << "Enter server port number: ";
			std::cin >> port_number;
		}
	};
	/*
		Opens a socket and saves the file descriptor returned by it, initializes struct containing
		data about the internet address for the server and binds that struct to to the socket,
		after the creation and binding of the socket, calls listen() to specify max number
		of queued connections allowed.

	*/
	bool CreateSocket()
	{
		//create socket returns filedescriptor
		socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_file_descriptor < 0)
		{
			std::cout << "Error creating socket\n";
			return 0;
		}

		//store data in server internet address struct
		bzero((char *)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET; //IPV4
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(port_number);

		//bind socket to internet address
		if (bind(socket_file_descriptor, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			std::cout << "Error binding\n";
			return 0;
		}

		//listen to incoming requests
		listen(socket_file_descriptor, 5);
		clilen = sizeof(client_address); //get size of client internet address buffer
		return 1;
	};
	;
	/*
		Reads in data sent from client (city name) and stores it in buffer, takes the data from that buffer
		and searches the list containing all the given cities from user input. If the city is found
		it writes to the buffer which is read in by the client, the temperature and description for 
		that city. If the city is not found it writes a error message to the buffer. Closes the 
		accepted connection made by the client
	*/
	bool ReadWriteClient()
	{
		bzero(read_client, 64); //initialize buffer
		//read from sockett
		if (read(newsocket_file_descriptor, read_client, 64) < 0)
		{
			std::cout << "ERROR reading from socket\n ";
			return 0;
		}

		std::cout << "Weather report for " << read_client << std::endl;
		//store response in send_client buffer
		bzero(send_client, 64);

		//find city name in list
		bool found = false;
		for (const auto &p : weather)
		{
			//find requested city from list
			if (strcmp(p.name, read_client) == 0)
			{
				//write to buffer to send to client
				found = true;
				std::cout << "Tomorrow's maximum temperature is " << p.temp << " F\n";
				std::cout << "Tomorrow's sky condition is " << p.description << std::endl;
				sprintf(send_client, "%d %s", p.temp, p.description);
				break;
			}
		}

		//if not found write error to client
		if (!found)
		{
			std::cout << "No data" << std::endl;
			sprintf(send_client, "%d %s", -999, "e");
		}

		std::cout << std::endl;

		//send response to client
		if (write(newsocket_file_descriptor, send_client, strlen(send_client)) < 0)
		{
			std::cout << "ERROR writing to  socket\n ";
			return 0;
		}
		close(newsocket_file_descriptor);
		return 1;
	};
	/*
		Accepts connection requests made by client, and stores the  data in a file descriptor.
	*/
	void CreateConnection()
	{
		//accept connection requests made from client
		newsocket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&client_address,
										   (socklen_t *)&clilen);
		//send response to client
		if (newsocket_file_descriptor < 0)
		{
			std::cout << "ERROR Connecting\n ";
			exit(1);
		}
	};
};

int main()
{

	Server server;
	bool socket;
	server.GetFileInput();
	server.GetPort();
	socket = server.CreateSocket();

	//if socket created, binded, and listening
	if (socket)
	{
		while (1)
		{
			//start accepting connections
			server.CreateConnection();

			//read and write to client
			if (!server.ReadWriteClient())
			{
				std::cout << "try fixing input from client\n";
			}
		}
	}
	else
	{
		exit(1);
	}

	return 0;
}
