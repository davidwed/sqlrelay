// Copyright (c) 2005 David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool	sqlrcursor::sql_injection_detection_ingress(const char *query) {
	return false;
}

bool	sqlrcursor::sql_injection_detection_egress() {
	return false;
}

void	sqlrcursor::sql_injection_detection_database_init() {
}

void 	sqlrcursor::sql_injection_detection_log(const char *query,
					char *parsed_sql,
					char *log_buffer) {
}

void 	sqlrcursor::sql_injection_detection_parameters() {
}

bool	sqlrcursor::sql_injection_detection_ingress_bl(const char *query) {
	return false;
}

bool	sqlrcursor::sql_injection_detection_egress_bl() {
	return false;
}

bool	sqlrcursor::sql_injection_detection_ingress_wl(const char *query) {
	return false;
}

bool	sqlrcursor::sql_injection_detection_egress_wl() {
	return false;
}

bool	sqlrcursor::sql_injection_detection_ingress_ldb() {
	return false;
}

bool	sqlrcursor::sql_injection_detection_egress_ldb() {
	return false;
}

void 	sqlrcursor::sql_injection_detection_parse_sql(const char *query) {
}

void 	sqlrcursor::sql_injection_detection_parse_results() {
}

bool	sqlrcursor::sql_injection_detection_check_db(const char *sid_db) {
	return false;
}
