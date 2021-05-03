// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

		void		setValue(uint64_t row, uint64_t col,
							const char *value);
		void		setColumnName(uint64_t col, const char *name);
		void		clear();

	private:
		sqlrresultsettableprivate	*pvt;
