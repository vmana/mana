#ifndef MANA_SERVER_H
#define MANA_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>
#include "common.h"

namespace mana
{

	class server
	{
		protected:
			bool started = false;
			int port = 1024;
			int sockd;
			virtual void session_handler() = 0;
			list<thread> sessions;

		public:

			server();
			void set_port(unsigned int port);
			virtual bool start() = 0;
			virtual void on_connect(int sock, string client_ip, int client_port) = 0;
			virtual bool stop() = 0;
			virtual bool is_started();
			~server();
	};

	class tcp_server : server
	{
		protected:
			struct sockaddr_in server_addr;
			// thread loop started after start()
			// will create a thread on_connect(...) when a client connects
			void session_handler();

		public:

			tcp_server();
			virtual bool start();
			virtual void on_connect(int sock, string client_ip, int client_port);
			virtual bool stop();
			~tcp_server();
	};
}
#endif
