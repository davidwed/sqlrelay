// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#define NUM_CONNECT_STRING_VARS 6

#include <sqlrconnection.h>

#include <mysql.h>

class mysqlconnection;

class mysqlcursor : public sqlrcursor {
	friend class mysqlconnection;
	private:
				mysqlcursor(sqlrconnection *conn);
		bool		executeQuery(const char *query,
						long length,
						bool execute);
		const char	*getErrorMessage(bool *liveconnection);
		void		returnRowCounts();
		void		returnColumnCount();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);

		/* method performs SQL Injection Detection */
 		bool	sql_injection_detection_ingress(const char *query);
 		bool	sql_injection_detection_egress();
 
		/* method connects to SID database */
 		void	sql_injection_detection_database_init();
 
		/* method maintains log for SQL Injection Detection */
 		void 	sql_injection_detection_log(const char *query,
							char *parsed_sql,
							char *log_buffer);
 
 		/* method gets parameters for SQL Injection Detection */
 		void 	sql_injection_detection_parameters();
 
 		/* method determines if SQL in black list */
 		bool	sql_injection_detection_ingress_bl(const char *query);
 		bool	sql_injection_detection_egress_bl();
 
 		/* method determines if SQL in white list */
 		bool	sql_injection_detection_ingress_wl(const char *query);
 		bool	sql_injection_detection_egress_wl();
 
 		/*method that determines if a value is in a sid db */
 		bool 	sql_injection_detection_check_db(const char *sid_db);
 
 		/* method determines if SQL in learned database */
 		bool	sql_injection_detection_ingress_ldb();
 		bool	sql_injection_detection_egress_ldb();
 
 		/* method parses the sql query */
 		void 	sql_injection_detection_parse_sql(const char *query);
 		void 	sql_injection_detection_parse_results();

		MYSQL_RES	*mysqlresult;
		MYSQL_FIELD	*mysqlfield;
		MYSQL_ROW	mysqlrow;
		int		ncols;
		int		nrows;
		int		affectedrows;
		int		queryresult;

		mysqlconnection	*mysqlconn;

		// variables for SID
		int sql_inject_load_params;
		int ingress_mode;
		int egress_mode;
		int listen_mode;
		int verification_mode;
		int prevention_mode;	

		char sid_parsed_sql[BUFSIZ];
		char sid_parsed_results[BUFSIZ];
		char sid_query[BUFSIZ];

		char sid_log_message[BUFSIZ];

		MYSQL	*sid_mysql;
		MYSQL_RES *sid_res;
		MYSQL_ROW sid_row;
		MYSQL_FIELD *sid_fields;

		int sid_query_result;

		bool sql_injection_detection;
};

class mysqlconnection : public sqlrconnection {
	friend class mysqlcursor;
	public:
				mysqlconnection();
	private:
		int		getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn();
#ifdef HAVE_MYSQL_CHANGE_USER
		bool		changeUser(const char *newuser,
						const char *newpassword);
#endif
		sqlrcursor	*initCursor();
		void		deleteCursor(sqlrcursor *curs);
		void		logOut();
		bool		isTransactional();
#ifdef HAVE_MYSQL_PING
		bool		ping();
#endif
		const char	*identify();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();

		MYSQL	mysql;
		int	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;
};

#endif
