// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void		unload();
		void		loadRouter(domnode *route);

		friend class routerconnection;
		friend class routercursor;

		void	setCurrentConnectionId(const char *connid);

		sqlrroutersprivate	*pvt;
