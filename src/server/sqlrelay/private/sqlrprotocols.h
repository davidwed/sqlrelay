// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadProtocol(uint16_t index, xmldomnode *listener);

		sqlrservercontroller	*cont;
		const char		*libexecdir;
		bool			debug;

		dictionary< uint16_t , sqlrprotocolplugin * >	protos;
