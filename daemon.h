#ifndef DAEMON_H
#define DAEMON_H

#ifndef _WIN32

#include "common.h"

namespace mana
{

class daemon
{
	public :
		daemon(string daemon_name = "daemon");

		string daemon_name;
		bool daemon_started = false;
		system logfile;
		vector<string> error;

		string lockname();
		bool stop();
		bool start();
		bool restart();
		int status();
		int args_parser(int argc, char *argv[]);
		// args_handler : called before args_parser. if false, stop args_parser
		virtual bool args_handler(int argc, char *argv[]);

		virtual void daemonize() = 0; // put your loop here
};

} // namespace

#endif // _WIN32

#endif // DAEMON_H
