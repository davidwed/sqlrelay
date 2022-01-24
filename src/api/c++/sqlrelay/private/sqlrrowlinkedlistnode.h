// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

		sqlrrowlinkedlistnode();
		sqlrrowlinkedlistnode(sqlrcursor *cursor);
		sqlrrowlinkedlistnode(sqlrcursor *cursor, uint64_t row);
		~sqlrrowlinkedlistnode();
		void	setCursor(sqlrcursor *cursor);
		sqlrcursor	*getCursor() const;
		void	setRow(uint64_t row);
		void	setColumn(uint32_t col);
		void	setValue(const char *value);
		const char * &getValue();
		void	setNext(listnode<const char *> *next);
		void	setPrevious(listnode<const char *> *next);
		void	print() const;

	private:
		sqlrrowlinkedlistnodeprivate	*pvt;
