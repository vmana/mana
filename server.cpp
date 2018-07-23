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

	// start handler loop
	handler_loop = thread(&tcp_server::session_handler, this);

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

		// mutex + add thread in sessions
		{
			lock_guard<mutex> lock(m_sessions);
			sessions.push_back(thread(&tcp_server::on_connect, this, sock, client_ip, client_port));
		}
	}

	started = false;
	close(sockd);

	// clean all sessions !
	lock_guard<mutex> lock(m_sessions);
	for (auto it = sessions.begin(); it != sessions.end(); ++it)
	{
		pthread_cancel(it->native_handle());
		it->join();
	}
	sessions.clear();
}

void tcp_server::on_connect(int sock, string client_ip, int client_port)
{

	close(sock);
	// clean, unless !started, because it'll be clean by parent's thread
	if (!started) clean_session(this_thread::get_id());
}

void tcp_server::clean_session(thread::id id)
{
	lock_guard<mutex> lock(m_sessions);
	auto it = find_if(sessions.begin(), sessions.end(), [=](thread &t) { return (t.get_id() == id); });
	if (it != sessions.end())
	{
		sessions.erase(it);
	}
}

bool tcp_server::stop()
{
	started = false;
	pthread_cancel(handler_loop.native_handle());
	handler_loop.join();
	return true;
}

tcp_server::~tcp_server()
{
}

} // namespace
