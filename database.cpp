#include "database.h"

namespace mana
{

vector<string> database::error;
bool database::log_errors = true;

database_rows::database_rows()
{
}


/********	database		********/

database::database()
{
	_is_open = false;
	this->server = "localhost";
	pthread_mutex_init(&p_mutex, NULL);
	internal_mode = db_mode::normal;
}

#ifdef MANA_MSSQL
int database::msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate, int severity, char *msgtext, char *srvname, char *procname, int line)
{
	enum {changed_database = 5701, changed_language = 5703 };
	if (msgno == changed_database || msgno == changed_language) return 0;

	if (!database::log_errors) return 0;

	if (msgno > 0)
	{
		database::error.push_back("database : " + convert::int_string(msgno) + " level "
			+ convert::int_string(severity) + " state " + convert::int_string(msgstate));

		if (strlen(srvname) > 0) database::error.push_back("server : " + string(srvname));
		if (strlen(procname) > 0) database::error.push_back("procedure " + string(procname));
		if (line > 0) database::error.push_back("line " + convert::int_string(line));
	}
	database::error.push_back(msgtext);
	return 0;
}

int database::err_handler(DBPROCESS * dbproc, int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
	if (!database::log_errors) return INT_CANCEL;

	if (dberr) database::error.push_back("database : " + convert::int_string(dberr) + " level " + convert::int_string(severity) + " : " + dberrstr);
	else database::error.push_back("DB-LIBRARY error : " + string(dberrstr));
	return INT_CANCEL;
}
#endif

void database::lock()
{
	if (internal_mode == db_mode::mutex) pthread_mutex_lock(&p_mutex);
}

void database::unlock()
{
	if (internal_mode == db_mode::mutex) pthread_mutex_unlock(&p_mutex);
}

string database::encoder(const string &str)
{
	switch (encoding)
	{
		case db_encoding::no_encode:
			return str;
		case db_encoding::utf8_to_iso88591:
			return convert::utf8_iso88591(str);
	}
	return str;
}

string database::decoder(const string &str)
{
	switch (encoding)
	{
		case db_encoding::no_encode:
			return str;
		case db_encoding::utf8_to_iso88591:
			return convert::iso88591_utf8(str);
	}
	return str;
}

bool database::open()
{
	if (_is_open) return _is_open;

	// wait to get the mutex if internal_mode == mutex
	lock();

	_is_open = false;

	error.clear();

	switch (type)
	{
		case mssql :
			#ifdef MANA_MSSQL
			if (dbinit() == FAIL)
			{
				if (database::log_errors) error.push_back("dbinit() failed");
				unlock();
				return _is_open;
			}

			dberrhandle(database::err_handler);
			dbmsghandle(database::msg_handler);

			LOGINREC *login;

			if ((login = dblogin()) == NULL)
			{
				if (database::log_errors) error.push_back("unable to allocate login structure");
				unlock();
				return _is_open;
			}

			DBSETLUSER(login, user.c_str());
			DBSETLPWD(login, pass.c_str());

			if ((dbproc = dbopen(login, server.c_str())) == NULL)
			{
				if (database::log_errors) error.push_back("unable to connect to" + server + " as " + user);
				dbloginfree(login);
				unlock();
				return _is_open;
			}

			dbloginfree(login);
			_is_open = true;
			#else
			if (database::log_errors) error.push_back("unable to connect : driver not loaded");
			#endif
			break;
		case mysql :
			#ifdef MANA_MYSQL
			try
			{
				mysql_driver = get_driver_instance();
				mysql_con = mysql_driver->connect("tcp://" + server, user, pass);
				mysql_con ->setSchema(db);
				_is_open = true;
			}
			catch (sql::SQLException &e)
			{
				if (database::log_errors) error.push_back("unable to connect to" + server + " as " + user + " : " + e.what());
			}
			#else
			if (database::log_errors) error.push_back("unable to connect : driver not loaded");
			#endif
			break;
		case sqlite :
			#ifdef MANA_SQLITE
			db_lite_name = QString(randstring(16).c_str());
			db_lite = QSqlDatabase::addDatabase("QSQLITE", db_lite_name);
			db_lite.setDatabaseName(QString(db.c_str()));
			db_lite.setHostName(QString(server.c_str()));
			db_lite.setUserName(QString(user.c_str()));
			db_lite.setPassword(QString(pass.c_str()));
			_is_open = db_lite.open();
			if (!_is_open)
			{
				if (database::log_errors) error.push_back("unable to connect to" + server + " as " + user);
				unlock();
				return _is_open;
			}
			#else
			if (database::log_errors) error.push_back("unable to connect : driver not loaded");
			#endif
			break;

	}

	if (!_is_open) unlock(); // prevents dead lock

	return _is_open;
}

bool database::close()
{
	bool ret = false;
	if (!_is_open) return ret;

	switch (type)
	{
		case mssql :
			#ifdef MANA_MSSQL
			dbclose(dbproc);
			#endif
			break;
		case mysql :
			#ifdef MANA_MYSQL
			delete mysql_con;
			#endif
			break;
		case sqlite :
			#ifdef MANA_SQLITE
			db_lite = QSqlDatabase();
			db_lite.close();
			QSqlDatabase::removeDatabase(db_lite_name);
			#endif
			break;
	}
	_is_open = false;

	unlock();

	return true;
}

bool database::is_open()
{
	return _is_open;
}

vector<vector<string> > database::query(string sql_query)
{
	vector<vector<string> > ret;
	if (!is_open()) open();

	switch (type)
	{
		case mssql :
			return query_mssql(sql_query);
			break;
		case mysql :
			return query_mysql(sql_query);
			break;
		case sqlite :
			return query_sqlite(sql_query);
		break;
	}
	return ret;
}

vector<vector<string> > database::query_mssql(string sql_query)
{
	vector<vector<string> > ret;
	// try to encode
	sql_query = encoder(sql_query);

	#ifdef MANA_MSSQL
	RETCODE erc;


	if ((erc = dbfcmd(dbproc, sql_query.c_str())) == FAIL)
	{
		if (database::log_errors) error.push_back("dbfcmd failed with " + sql_query);
		return ret;
	}

	if ((erc = dbsqlexec(dbproc)) == FAIL)
	{
		if (database::log_errors) error.push_back("dbsqlexec failed with " + sql_query);
		return ret;
	}

	while ((erc = dbresults(dbproc)) != NO_MORE_RESULTS)
	{
		struct COL
		{
			char *name;
			char *buffer;
			int type, size, status;
		} *columns, *pcol;
		int ncols;
		int row_code;

		ncols = dbnumcols(dbproc);
		columns = (COL*)calloc(ncols, sizeof(struct COL));
		for (pcol = columns; pcol - columns < ncols; pcol++)
		{
			int c = pcol - columns + 1;
			pcol->name = dbcolname(dbproc, c);
			pcol->type = dbcoltype(dbproc, c);
			pcol->size = dbcollen(dbproc, c);
			if (SYBCHAR != pcol->type) pcol->size = dbwillconvert(pcol->type, SYBCHAR);
			pcol->buffer = (char*)calloc(1, pcol->size + 1);

			if ((erc = dbbind(dbproc, c, NTBSTRINGBIND,	pcol->size+1, (BYTE*)pcol->buffer)) == FAIL)
			{
				if (database::log_errors) error.push_back("dbbind number " + convert::int_string(c) + " failed");
				return ret;
			}

			if ((erc = dbnullbind(dbproc, c, &pcol->status)) == FAIL)
			{
				if (database::log_errors) error.push_back("dbnullbind number " + convert::int_string(c) + " failed");
				return ret;
			}
		}

		while ((row_code = dbnextrow(dbproc)) != NO_MORE_ROWS)
		{
			vector<string> line;
			switch (row_code)
			{
				case REG_ROW :

					for (pcol=columns; pcol - columns < ncols; pcol++)
					{
						if (pcol->status == -1) line.push_back(string("mana::null"));
						else
						{
							// try to decode
							string decoded = decoder(string(pcol->buffer));
							line.push_back(decoded);
						}
					}
					ret.push_back(line);
					break;
				case FAIL :
					if (database::log_errors) error.push_back("dbnextrow failed");
					break;
			}
		}

		for (pcol = columns; pcol - columns < ncols; pcol++)
		{
			free(pcol->buffer);
		}
		free(columns);
	}
	#endif

	return ret;
}

vector<vector<string> > database::query_mysql(string sql_query)
{
	vector<vector<string> > ret;
	#ifdef MANA_MYSQL
	sql::ResultSet *res = NULL;
	sql::Statement *s = NULL;
	try
	{
		s = mysql_con->createStatement();
		res = s->executeQuery(sql_query);
		int col_count = -1;
		while (res->next())
		{
			if (col_count < 0) col_count = res->getMetaData()->getColumnCount();
			vector<string> line;
			for (int i = 0; i < col_count; i++)
			{
				line.push_back(res->getString(i+1));
			}
			ret.push_back(line);
		}
	}
	catch (sql::SQLException &e)
	{
		if (database::log_errors) error.push_back(string("query_mysql failed : ") + string(e.what()) + "\nquery : " + sql_query);
	}

	if (res) delete res;
	if (s) delete s;
	#endif
	return ret;
}

vector<vector<string> > database::query_sqlite(string sql_query)
{
	vector<vector<string> > ret;
	#ifdef MANA_SQLITE
	QSqlQuery S(QString(sql_query.c_str()), db_lite);
	for (int i = 0; S.next(); i++)
	{
		vector<string> line;
		for (int i2 = 0; i2 < S.record().count(); i2++)
		{
			line.push_back(S.record().value(i2).toString().toStdString());
		}
		ret.push_back(line);
	}
	return ret;
	#endif
	return ret;
}

string database::purge(string &S)
{
	string ret = S;
	switch (type)
	{
		case mssql :
			ret = str_replace("'", "''", ret);
			ret = str_replace("%", "%%", ret);
			break;
		case mysql :
			break;
		case sqlite :
			ret = str_replace("'", "''", ret);
			break;
	}
	return ret;
}

database::~database()
{
	close();
	if (internal_mode == mutex) pthread_mutex_unlock(&p_mutex);
	pthread_mutex_destroy(&p_mutex);
}


} // namespace

