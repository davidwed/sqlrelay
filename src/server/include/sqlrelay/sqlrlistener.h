// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#ifndef SQLRLISTENER_H
#define SQLRLISTENER_H

#include <sqlrelay/private/sqlrlistenerincludes.h>

class SQLRSERVER_DLLSPEC sqlrlistener : public listener {
	public:
			sqlrlistener();
			~sqlrlistener();
		bool	init(int argc, const char **argv);
		void	listen();

		const char	*getId();
		const char	*getLogDir();
		const char	*getDebugDir();

		#include <sqlrelay/private/sqlrlistener.h>
};

#endif
