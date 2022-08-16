// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

		bool	getTrackInsertionOrder();
		void	setValue(const char *key, const char *value);
		void	setValues(const char **key, const char **value);
		void	setValues(const char **key, const char **value,
							uint64_t count);
		void	setValues(const char * const *key,
						const char * const *value);
		void	setValues(const char * const *key,
						const char * const *value,
						uint64_t count);
		bool	remove(const char *key);
		void	clear();

	private:
		sqlrrowdictionaryprivate	*pvt;
