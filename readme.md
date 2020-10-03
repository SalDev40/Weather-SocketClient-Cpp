Client.cpp

	  Will connect  with  your server and send it requests for the weather report for a given city. Will create a stream socket, and connect  to the specified server. Outputs either the error or result sent by the server


Server.cpp

	Create  a  stream socket, bind  it  to  the specified  port  number, 
	do  a listen() to specify a maximum number of queued connection requests and do an
	accept() that will let it wait for connection requests. Retrieves data from and client
	and respond with either an error or the day’s maximum temperature and sky condition.



How to Run:
    - g++ server.cpp -o server

        - Terminal 1: ./server
        - enter port number -> between 2000 and 65535
        - MAKE SURE CLIENT MATCHES SAME PORT NUMBER
        - EX: server port = 5432, client must choose port localhost:5432 to connect
        
    - g++ client.cpp -o client

        - Terminal 2: ./client
        - Connect to server hostname:portnumber
            - enter host name: localhost
            - enter port number: 5432
            - enter a city name: 
                - enter one of these cities:
                        - Amarillo
                        - Austin
                        - Corpus Christi
                        - Dallas
                        - El Paso
                        - Galveston
                        - Houston
                        - San Antonio
                        - Truth or Consequences

                - output:

                        - Amarillo -> 61,Sunny
                        - Austin -> 76,Partly Cloudy
                        - Corpus Christi -> 79,AM Thunderstorm
                        - Dallas -> 65,Sunny
                        - El Paso -> 67,Sunny
                        - Galveston -> 69,Thunderstorms
                        - Houston -> 77,AM Thunderstorms
                        - San Antonio -> 79,Mostly Cloudy
                        - Truth or Consequences -> 53,Sunny/Windy

