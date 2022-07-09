// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

		void	prepend(const char *value);
		void	prepend(listnode<const char *> *node);
		void	append(const char *value);
		void	append(listnode<const char *> *node);
		void	insertBefore(listnode<const char *> *node,
							const char *value);
		void	insertBefore(listnode<const char *> *node,
					listnode<const char *> *newnode);
		void	insertAfter(listnode<const char *> *node,
							const char *value);
		void	insertAfter(listnode<const char *> *node,
					listnode<const char *> *newnode);
		void	moveBefore(listnode<const char *> *node,
					listnode<const char *> *nodetomove);
		void	moveAfter(listnode<const char *> *node,
					listnode<const char *> *nodetomove);
		void	detach(listnode<const char *> *node);
		bool	remove(const char *value);
		bool	removeAll(const char *value);
		bool	remove(listnode<const char *> *node);
		void	insertionSort();
		void	heapSort();
		void	clear();
	
	private:
		sqlrrowlistprivate	*pvt;
