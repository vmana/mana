#include "server.h"

namespace mana
{

/****    server    ****/

server::server()
{
}

void server::set_port(unsigned int port)
{
	if (port == 0) return;
	this->port = port;
}

bool server::is_started()
{
	return started;
}

server::~server()
{
}

/****    tcp server    ****/

tcp_server::tcp_server()
{
}

bool tcp_server::start()
{
	sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd < 0) return false; // error handler

	// init struct
	bzero((char*) &server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	int res = bind(sockd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	if (res < 0) return false; // error handler

	res = listen(sockd, 10);
	if (res < 0) return false; // error handler

	return true;
}

void tcp_server::session_handler()
{
	while (started)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int sock = accept(sockd, (struct sockaddr*) &client_addr, &client_len);
		if (sock < 0) continue; // error handler

		int client_port = ntohs(client_addr.sin_port);
		string client_ip = string(inet_ntoa(client_addr.sin_addr));

		sessions.push_back(thread(&tcp_server::on_connect, this, sock, client_ip, client_port));
	}
}

void tcp_server::on_connect(int sock, string client_ip, int client_port)
{
	// remove this thread
	//for (auto i = sessions.begin(); i != sessions.en
}

bool tcp_server::stop()
{
	return true;
}

tcp_server::~tcp_server()
{
}

} // namespace
