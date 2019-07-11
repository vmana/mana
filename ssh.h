#ifndef SSH_H
#define SSH_H

#include "common.h"

#ifndef NO_MANA_SSH
#ifndef _WIN32
#include <libssh/libssh.h>
#include <libssh/callbacks.h>

namespace mana
{

class ssh
{
	protected :
		ssh_session session;
		bool protected_connected = false;
		bool verify_knownhost();
		bool auth_public_key();
	public :
		string host;
		string user;
		string password;
		int port;
		long timeout = 6000; // milliseconds
		string known_hosts_file = "~/.ssh/known_hosts";
		string public_key_file = "~/.ssh/id_rsa.pub";
		string private_key_file = "~/.ssh/id_rsa";
		vector<string> error;

		ssh();
		static void init();
		bool connect();
		bool disconnect();
		bool connected();
		bool exec(string cmd, string *result = NULL);
		void set_host(string url); // [user@]host[:port]
		~ssh();
};

} // namespace

#endif // _WIN32
#endif // NO_MANA_SSH
#endif // SSH_H
