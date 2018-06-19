// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void		unload();
		void		loadNotification(domnode *notification);

		char		*substitutions(sqlrlistener *sqlrl,
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *str,
						const char *event,
						const char *info);

		sqlrnotificationsprivate	*pvt;
