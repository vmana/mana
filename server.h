#ifndef MANA_SERVER_H
#define MANA_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>
#include <mutex>
#include "common.h"

namespace mana
{

	class server
	{
		protected:
			atomic<bool> started;
			int port = 1024;
			int sockfd;
			virtual void sessions_handler() = 0;
			list<thread> sessions;
			mutex m_sessions;
			thread handler_loop; // thread for session_handler

		public:

			vector<string> errors;

			server();
			void set_port(unsigned int port);
			virtual bool start() = 0;
			virtual void on_connect(int sock, string client_ip, int client_port) = 0;
			virtual bool stop() = 0;
			virtual bool is_started();
			~server();
	};

	class tcp_server : public server
	{
		protected:
			struct sockaddr_in server_addr;
			// thread loop started after start()
			// will create a thread on_connect(...) when a client connects
			void sessions_handler();
			// session_thread will call on_connect + will handle clean_session
			void session_thread(int sock, string client_ip, int client_port);
			virtual void clean_session(thread::id id); // call when a session ends

		public:

			tcp_server();
			virtual bool start();
			// on_connect must be implemented by derived class
			virtual void on_connect(int sock, string client_ip, int client_port) = 0;
			virtual bool stop();
			~tcp_server();
	};
}
#endif
