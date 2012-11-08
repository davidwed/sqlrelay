// Copyright (c) 2000-2012  David Muse
// See the file COPYING for more information.

#ifndef SQLRSTATISTICS_H
#define SQLRSTATISTICS_H

struct sqlrstatistics {
	int32_t	open_svr_connections;
	int32_t	opened_svr_connections;

	int32_t	open_cli_connections;
	int32_t	opened_cli_connections;

//	int32_t	timed_out_svr_connections;
//	int32_t	timed_out_cli_connections;
	
	int32_t	open_svr_cursors;
	int32_t	opened_svr_cursors;

	int32_t	times_new_cursor_used;
	int32_t	times_cursor_reused;

	int32_t	total_queries;
	int32_t	total_errors;

	int32_t	forked_listeners;
};

#endif
