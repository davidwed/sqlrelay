// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

		bool	fakeInputBinds();
		void	performSubstitution(stringbuffer *buffer,
							int16_t index);

		void	getColumnPointers(const char ***columnnames,
					uint16_t **columnnamesizes,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamesizes,
					uint32_t **columnsizes,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablesizes);

	private:
		bool	isBeginTransactionQuery(const char *query);
		bool	blockCanBeIntercepted(const char *block);
		bool	isCommitQuery(const char *query);
		bool	isRollbackQuery(const char *query);
		bool	isAutoCommitOnQuery(const char *query);
		bool	isAutoCommitOffQuery(const char *query);
		bool	isAutoCommitQuery(const char *query, bool on);
		bool	isSetIncludingAutoCommitQuery(const char *query,
								bool *on);
		void	allocateColumnPointers(uint32_t colcount);
		void	deallocateColumnPointers();

		sqlrservercursorprivate	*pvt;
