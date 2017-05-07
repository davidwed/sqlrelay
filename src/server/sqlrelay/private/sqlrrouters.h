// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void		unload();
		void		loadRouter(xmldomnode *route,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount);

		friend class routerconnection;
		friend class routercursor;
		void		setCurrentConnectionId(const char *conid);

		sqlrroutersprivate	*pvt;
