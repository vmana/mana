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
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int)) < 0)
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
	while (started)
	{
		// poll sockfd
		struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 5000; // 5 ms
		fd_set set; FD_ZERO(&set); FD_SET(sockfd, &set);

		int res_poll = select(sockfd + 1, &set, NULL, NULL, &tv);
		if (res_poll == 0) continue; // not data to read on sockfd
		if (res_poll == -1)
		{
			errors.push_back("mana::tcp_server res_poll failed");
			break; // error handler
		}

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

	handler_loop.join();

	close(sockfd);

	// clean all sessions !
	lock_guard<mutex> lock(m_sessions);
	for (auto it = sessions.begin(); it != sessions.end(); ++it)
	{
#ifndef _WIN32
		pthread_cancel(it->native_handle());
#else
		TerminateThread(it->native_handle(), 0);
#endif
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

/****    udp server    ****/

udp_server::udp_server()
{
}

bool udp_server::start()
{
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		errors.push_back("mana::udp_server sockfd creation");
		return false; // error handler
	}

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int)) < 0)
	{
		errors.push_back("mana::udp_server setsockopt creation");
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
		errors.push_back("mana::udp_server bind on port " + convert::int_string(port) + " (" + convert::int_string(res) + ")");
		return false; // error handler
	}

	res = listen(sockfd, 10);
	if (res < 0)
	{
		errors.push_back("mana::udp_server listen (" + convert::int_string(res) + ")");
		return false; // error handler
	}

	// start handler loop
	handler_loop = thread(&udp_server::sessions_handler, this);

	started = true;
	return true;
}

void udp_server::sessions_handler()
{
	const int max_size = 4096;

	while (started)
	{
		// poll sockfd
		struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 5000; // 5 ms
		fd_set set; FD_ZERO(&set); FD_SET(sockfd, &set);

		int res_poll = select(sockfd + 1, &set, NULL, NULL, &tv);
		if (res_poll == 0) continue; // not data to read on sockfd
		if (res_poll == -1)
		{
			errors.push_back("mana::udp_server res_poll failed");
			break; // error handler
		}

		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		// buf will be deleted on error, or after on_datagram
		// on_datagram doesn't need to bother with deleting
		char *buf = new char[max_size];
		int nread = recvfrom(sockfd, buf, max_size, 0, (struct sockaddr*) &client_addr, &client_len);
		if (nread < 0)
		{
			errors.push_back("mana::udp_server recvfrom failed");
			delete [] buf;
			continue; // error handler
		}

		int client_port = ntohs(client_addr.sin_port);
		string client_ip = string(inet_ntoa(client_addr.sin_addr));

		// mutex + add thread in sessions
		{
			lock_guard<mutex> lock(m_sessions);
			sessions.push_back(thread(&udp_server::session_thread, this, buf, nread, client_ip, client_port));
		}
	}
}

void udp_server::session_thread(char* buf, int size, string client_ip, int client_port)
{
	on_datagram(buf, size, client_ip, client_port);

	delete [] buf;
	// clean, unless !started, because it'll be clean by parent's thread
	if (started) clean_session(this_thread::get_id());
}

void udp_server::clean_session(thread::id id)
{
	lock_guard<mutex> lock(m_sessions);
	auto it = find_if(sessions.begin(), sessions.end(), [=](thread &t) { return (t.get_id() == id); });
	if (it != sessions.end())
	{
		it->detach();
		sessions.erase(it);
	}
}

bool udp_server::stop()
{
	if (!started) return false;
	started = false;

	handler_loop.join();

	close(sockfd);

	// clean all sessions !
	lock_guard<mutex> lock(m_sessions);
	for (auto it = sessions.begin(); it != sessions.end(); ++it)
	{
#ifndef _WIN32
		pthread_cancel(it->native_handle());
#else
		TerminateThread(it->native_handle(), 0);
#endif
		it->join();
	}
	return true;
}

udp_server::~udp_server()
{
	if (started)
	{
		stop();
	}
}

} // namespace
