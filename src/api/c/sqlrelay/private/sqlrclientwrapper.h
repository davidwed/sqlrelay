SQLRCLIENT_DLLSPEC
sqlrcon	sqlrcon_alloc_copyrefs(const char *server,
					 uint16_t port, const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					int copyreferences);

SQLRCLIENT_DLLSPEC
int sqlrcon_isYes(const char *string);

SQLRCLIENT_DLLSPEC
int sqlrcon_isNo(const char *string);

SQLRCLIENT_DLLSPEC
sqlrcur	sqlrcur_alloc_copyrefs(sqlrcon sqlrconref, int copyreferences);

SQLRCLIENT_DLLSPEC
sqlrcur	sqlrcur_getOutputBindCursor_copyrefs(sqlrcur sqlrcurref,
					const char *variable,
					int copyreferences);
