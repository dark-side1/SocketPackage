server:server.cpp TcpSocket.cpp TcpServer.cpp TaskQueue.cpp ThreadPool.cpp
	g++ -std=c++11 server.cpp TcpSocket.cpp TcpServer.cpp TaskQueue.cpp ThreadPool.cpp  -o server -lpthread

client:client.cpp TcpSocket.cpp
	g++ -std=c++11 client.cpp TcpSocket.cpp -o client -lpthread

