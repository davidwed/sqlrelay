SQLRCLIENTWRAPPER_DLLSPEC
sqlrcon	sqlrcon_alloc_copyrefs(const char *server,
					 uint16_t port, const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					int copyreferences);

SQLRCLIENTWRAPPER_DLLSPEC
sqlrcur	sqlrcur_alloc_copyrefs(sqlrcon sqlrconref, int copyreferences);

SQLRCLIENTWRAPPER_DLLSPEC
sqlrcur	sqlrcur_getOutputBindCursor_copyrefs(sqlrcur sqlrcurref,
					const char *variable,
					int copyreferences);
