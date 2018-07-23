#include "server.h"

namespace mana
{

/****    server    ****/

server::server()
{
	started = false;
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
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		errors.push_back("mana::tcp_server sockfd creation");
		return false; // error handler
	}

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		errors.push_back("mana::tcp_server setsockopt creation");
		return false; // error handler
	}

	// init struct
	bzero((char*) &server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	int res = bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	if (res < 0)
	{
		errors.push_back("mana::tcp_server bind on port " + convert::int_string(port) + " (" + convert::int_string(res) + ")");
		return false; // error handler
	}

	res = listen(sockfd, 10);
	if (res < 0)
	{
		errors.push_back("mana::tcp_server listen (" + convert::int_string(res) + ")");
		return false; // error handler
	}

	// start handler loop
	handler_loop = thread(&tcp_server::sessions_handler, this);

	started = true;
	return true;
}

void tcp_server::sessions_handler()
{
	while (true)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int sock = accept(sockfd, (struct sockaddr*) &client_addr, &client_len);
		if (sock < 0)
		{
			errors.push_back("mana::tcp_server accept failed (" + convert::int_string(sock) + ")");
			continue; // error handler
		}

		int client_port = ntohs(client_addr.sin_port);
		string client_ip = string(inet_ntoa(client_addr.sin_addr));

		// mutex + add thread in sessions
		{
			lock_guard<mutex> lock(m_sessions);
			sessions.push_back(thread(&tcp_server::session_thread, this, sock, client_ip, client_port));
		}
	}
}

void tcp_server::session_thread(int sock, string client_ip, int client_port)
{
	on_connect(sock, client_ip, client_port);

	close(sock);
	// clean, unless !started, because it'll be clean by parent's thread
	if (started) clean_session(this_thread::get_id());
}

void tcp_server::clean_session(thread::id id)
{
	lock_guard<mutex> lock(m_sessions);
	auto it = find_if(sessions.begin(), sessions.end(), [=](thread &t) { return (t.get_id() == id); });
	if (it != sessions.end())
	{
		it->detach();
		sessions.erase(it);
	}
}

bool tcp_server::stop()
{
	if (!started) return false;
	started = false;

	pthread_cancel(handler_loop.native_handle());
	handler_loop.join();

	close(sockfd);

	// clean all sessions !
	lock_guard<mutex> lock(m_sessions);
	for (auto it = sessions.begin(); it != sessions.end(); ++it)
	{
		pthread_cancel(it->native_handle());
		it->join();
	}
	return true;
}

tcp_server::~tcp_server()
{
	if (started)
	{
		stop();
	}
}

} // namespace
