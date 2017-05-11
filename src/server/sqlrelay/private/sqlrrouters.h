// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void		unload();
		void		loadRouter(xmldomnode *route);

		friend class routerconnection;
		friend class routercursor;

		void	setCurrentConnectionId(const char *connid);

		sqlrroutersprivate	*pvt;
