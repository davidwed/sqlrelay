// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	protected:
		sqlrservercontroller	*cont;
		xmldomnode		*parameters;
		bool			debug;

		filedescriptor		*clientsock;

		gsscredentials	gcred;
		gssmechanism	gmech;
		gsscontext	gctx;
		tlscontext	tctx;
