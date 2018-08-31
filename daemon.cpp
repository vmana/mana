#ifndef _WIN32
#include "daemon.h"

namespace mana
{

daemon::daemon(string daemon_name)
{
	this->daemon_name = daemon_name;
	logfile.path = "/var/log/" + file::basename(daemon_name) + ".log";
}

string daemon::lockname()
{
	#ifdef _WIN32
	return daemon_name;
	#else
	return "/tmp/" + daemon_name + ".lock";
	#endif
}

bool daemon::stop()
{
	bool ret = true;
	logfile.log("daemon stop");
	daemon_started = false;
	system::killprocess(daemon_name);
	return ret;
}

bool daemon::start()
{
	bool ret = true;
	if (system::daemonlock(lockname()))
	{
		// start it
		logfile.log("daemon start");
		daemon_started = true;
		daemonize();
	}
	else
	{
		logfile.log("daemon start (already started)");
	}
	return ret;
}

bool daemon::restart()
{
	bool ret = stop();
	if (ret) ret = start();
	if (ret) logfile.log("daemon restart");
	else logfile.log("daemon restart failed");
	return ret;
}

int daemon::status()
{
	//if (system::daemonlock(lockname)) return false;
	return 0;
}

int daemon::args_parser(int argc, char *argv[])
{
	if (!args_handler(argc, argv)) return 0;

	string cmd = "start";
	if (argc > 1) cmd = argv[1];

	if (cmd == "start")
	{
		int pid = fork();
		if (pid != 0) return 0;
		if (!start()) return 1;
	}

	if (cmd == "-h" || cmd == "--help" || cmd == "help")
	{
		cout << daemon_name << " {start|stop|status}" << endl;
		return 0;
	}

	if (cmd == "status")
	{
		if (!system::daemonlock(lockname())) cout << daemon_name << " daemon : started" << endl;
		else cout << daemon_name << " daemon : stopped" << endl;
		return 0;
	}

	if (cmd == "stop" && !system::daemonlock(lockname())) stop();
	return 0;
}

bool daemon::args_handler(int argc, char *argv[])
{
	return true;
}



} // namespace
#endif // _WIN32
