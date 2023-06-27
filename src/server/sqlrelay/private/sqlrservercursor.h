// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		bool	isBeginTransactionQuery(const char *query);
		bool	blockCanBeIntercepted(const char *block);
		bool	isCommitQuery(const char *query);
		bool	isRollbackQuery(const char *query);
		bool	isAutoCommitOnQuery(const char *query);
		bool	isAutoCommitOffQuery(const char *query);
		bool	isAutoCommitQuery(const char *query, bool on);
		bool	isSetIncludingAutoCommitQuery(const char *query,
							bool *autocommit,
							bool *on);

		sqlrservercursorprivate	*pvt;
