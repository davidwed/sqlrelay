// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

		void	setPreparedFlag(bool prepared);
		bool	getPreparedFlag();
		void	setQueryWasInterceptedFlag(bool querywasintercepted);
		bool	getQueryWasInterceptedFlag();
		void	setBindsWereFakedFlag(bool bindswerefaked);
		bool	getBindsWereFakedFlag();
		void	setFakeInputBindsForThisQueryFlag(
					bool fakeinputbindsforthisquery);
		bool	getFakeInputBindsForThisQueryFlag();

		sqlrserverconnection	*conn;

		stringbuffer	querywithfakeinputbinds;

	private:
		sqlrservercursorprivate	*pvt;
