#include "ssh.h"
#ifndef NO_MANA_SSH
#ifndef _WIN32

namespace mana
{

#if defined LIBSSH_VERSION_MINOR && LIBSSH_VERSION_MINOR < 9
#define SSH_KNOWN_HOSTS_OK SSH_SERVER_KNOWN_OK
#define SSH_KNOWN_HOSTS_CHANGED SSH_SERVER_KNOWN_CHANGED
#define SSH_KNOWN_HOSTS_OK SSH_SERVER_KNOWN_OK
#define SSH_KNOWN_HOSTS_OTHER SSH_SERVER_FOUND_OTHER
#define SSH_KNOWN_HOSTS_NOT_FOUND SSH_SERVER_FILE_NOT_FOUND
#define SSH_KNOWN_HOSTS_UNKNOWN SSH_SERVER_NOT_KNOWN
#define SSH_KNOWN_HOSTS_ERROR SSH_SERVER_ERROR
#define ssh_session_is_known_server ssh_is_server_known
#define ssh_session_update_known_hosts ssh_write_knownhost
#endif

ssh::ssh()
{
	session = NULL;
	host = "";
	user = "";
	password = "";
	port = 22;
}

void ssh::init()
{
	ssh_threads_set_callbacks(ssh_threads_get_pthread());
	ssh_init();
}

ssh::~ssh()
{
	disconnect();
}

bool ssh::verify_knownhost()
{
	int state = ssh_session_is_known_server(session);

	switch (state)
	{
		case SSH_KNOWN_HOSTS_OK :
			break; /* ok */
		case SSH_KNOWN_HOSTS_CHANGED :
			error.push_back("server known " + host + " has changed");
			return false;
		case SSH_KNOWN_HOSTS_OTHER :
			error.push_back("host key for this server was not found but another type of key exists");
			return false;
		case SSH_KNOWN_HOSTS_NOT_FOUND :
		case SSH_KNOWN_HOSTS_UNKNOWN :

			if (ssh_session_update_known_hosts(session) < 0)
			{
				error.push_back("ssh_write_knownhost failed");
				return false;
			}
			break;
		case SSH_KNOWN_HOSTS_ERROR :
			error.push_back(string(ssh_get_error(session)));
			return false;
	}
	return true;
}

bool ssh::auth_public_key()
{
	ssh_key public_key, private_key;
	int res;
	res = ssh_pki_import_pubkey_file(public_key_file.c_str(), &public_key);
	if (res != SSH_OK)
	{
		error.push_back("import public key from " + public_key_file + " failed with code " + convert::int_string(res));
		return false;
	}

	res = ssh_userauth_try_publickey(session, NULL, public_key);
	if (res != SSH_AUTH_SUCCESS)
	{
		error.push_back("ssh_userauth_try_publickey failed with code " + convert::int_string(res));
		ssh_key_free(public_key);
		return false; // doesn't accept auth
	}

	res = ssh_pki_import_privkey_file(private_key_file.c_str(), NULL, NULL, NULL, &private_key);
	if (res != SSH_OK)
	{
		error.push_back("import private key from " + private_key_file + " failed with code " + convert::int_string(res));
		ssh_key_free(public_key);
		return false;
	}

	res = ssh_userauth_publickey(session, NULL, private_key);
	if (res != SSH_AUTH_SUCCESS)
	{
		error.push_back("ssh_userauth_publickey failed with code " + convert::int_string(res));
		ssh_key_free(public_key);
		ssh_key_free(private_key);
		return false;
	}

	ssh_key_free(public_key);
	ssh_key_free(private_key);
	return true;
}

bool ssh::connect()
{
	bool ret = false;

	if (connected()) disconnect();
	if (session) ssh_free(session); // free existing session

	session = ssh_new();
	if (!session)
	{
		error.push_back("ssh_new failed");
		return ret;
	}

	ssh_options_set(session, SSH_OPTIONS_HOST, host.c_str());
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
	if (user == "") ssh_options_set(session, SSH_OPTIONS_USER, NULL);
	else ssh_options_set(session, SSH_OPTIONS_USER, user.c_str());

	long milliseconds_timeout = timeout*1000;
	ssh_options_set(session, SSH_OPTIONS_TIMEOUT_USEC, &milliseconds_timeout);
	ssh_options_set(session, SSH_OPTIONS_KNOWNHOSTS, known_hosts_file.c_str());

	// connect
	int rc = ssh_connect(session);
	if (rc != SSH_OK)
	{
		error.push_back(string(ssh_get_error(session)));
		return ret;
	}

	protected_connected = true;

	// security check
	if (!verify_knownhost())
	{
		disconnect();
		return false;
	}

	// public key auth
	if (auth_public_key()) return true; // we don't need password

	// password auth
	rc = ssh_userauth_password(session, NULL, password.c_str());
	if (rc != SSH_AUTH_SUCCESS)
	{
		error.push_back("password authentification failed");
		disconnect();
		return ret;
	}

	return true;
}

bool ssh::disconnect()
{
	if (!connected()) return false;
	if (!session) return false;

	protected_connected = false;
	ssh_disconnect(session);
	ssh_free(session);
	session = NULL;
	return true;
}

bool ssh::connected()
{
	return protected_connected;
}

bool ssh::exec(string cmd, string *result)
{
	bool ret = true;
	if (!connected()) return false;

	ssh_channel channel = ssh_channel_new(session);
	if (!channel)
	{
		error.push_back("ssh_channel_new failed");
		return false;
	}

	int rc = ssh_channel_open_session(channel);
	if (rc != SSH_OK)
	{
		error.push_back("ssh_channel_open_session failed");
		ssh_channel_free(channel);
		return false;
	}

	char buffer[256];
	int nbytes;

	rc = ssh_channel_request_exec(channel, cmd.c_str());
	if (rc != SSH_OK)
	{
		error.push_back("ssh_channel_request_exec failed");
		ssh_channel_close(channel);
		ssh_channel_free(channel);
		return false;
	}

	// read result if not null
	if (result)
	{
		result->clear();
		nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
		while (nbytes > 0)
		{
			result->append(buffer, nbytes);
			nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
		}

		if (nbytes < 0)
		{
			error.push_back("ssh_channel_read");
			ssh_channel_close(channel);
			ssh_channel_free(channel);
			return false;
		}
	}

	// close channel
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);
	return ret;
}

void ssh::set_host(string url)
{
	vector<string> x = explode("@", url);
	if (x.size() > 1)
	{
		user = x[0];
		host = x[1];
	} else host = x[0];
	x = explode(":", host);
	if (x.size() > 1)
	{
		host = x[0];
		port = convert::string_int(x[1]);
	} else port = 22;
}

} // namespace

#endif // _WIN32
#endif // NO_MANA_SSH
