// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

		sqlrlistnode();
		sqlrlistnode(sqlrcursor *cursor);
		sqlrlistnode(sqlrcursor *cursor, uint64_t row, uint64_t col);
		~sqlrlistnode();
		void	setCursor(sqlrcursor *cursor);
		sqlrcursor	*getCursor() const;
		void	setRepresentsARow(bool representsarow);
		void	setRow(uint64_t row);
		void	setColumn(uint32_t col);
		void	setValue(const char *value);
		const char * &getValue();
		void	setNext(listnode<const char *> *next);
		void	setPrevious(listnode<const char *> *next);

	private:
		sqlrlistnodeprivate	*pvt;
