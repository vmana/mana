#ifndef MANADATABASE_H
#define MANADATABASE_H

#include "common.h"

#ifdef MANA_MSSQL
#include <sybfront.h>
#include <sybdb.h>
#endif

#ifdef MANA_MYSQL
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#endif

#ifdef MANA_SQLITE
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>
#include <QVariant>
#endif

// apt-get install libmdbodbc1 mdbtools unixodbc libmysqlcppconn-dev

namespace mana
{

class database_rows
{
	public :
		database_rows();
};

class database
{
	protected :
		bool _is_open;
		
		pthread_mutex_t p_mutex;
		void lock();
		void unlock();
		
		#ifdef MANA_MSSQL
		static int err_handler(DBPROCESS*, int, int, int, char*, char*);
		static int msg_handler(DBPROCESS*, DBINT, int, int, char*, char*, char*, int);
		#endif
		
		vector<vector<string> > query_mssql(string sql_query);
		vector<vector<string> > query_mysql(string sql_query);
		vector<vector<string> > query_sqlite(string sql_query);

	public :
		enum db_type { mssql, sqlite, mysql };
		enum db_mode { normal, mutex };
		
		db_type type;
		string server = "";
		string db;
		string user;
		string pass;
		int internal_mode;
		
		static vector<string> error;
		static bool log_errors;
		
		// mssql
		#ifdef MANA_MSSQL
		DBPROCESS *dbproc;
		#endif
		
		// mysql
		#ifdef MANA_MYSQL
		sql::Driver *mysql_driver;
		sql::Connection *mysql_con;
		#endif
		
		#ifdef MANA_SQLITE
		QSqlDatabase db_lite;
		QString db_lite_name;
		#endif
		
		database();
		bool open();
		bool close();
		bool is_open();
		
		vector<vector<string> > query(string sql_query);
		string purge(string &S);
		~database();
};

} // namespace

#endif // MANADATABASE_H
