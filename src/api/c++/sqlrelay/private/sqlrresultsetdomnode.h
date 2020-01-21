// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:

		sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					sqlrcursor *sqlrcur,
					bool creatattrnode);
		void	init(bool creatattrnode, sqlrcursor *sqlrcur);

		sqlrresultsetdomnodeprivate	*pvt;
