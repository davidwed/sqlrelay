// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	protected:
		void	init();
		bool	semWait(semaphoreset *semset,
					int32_t index,
					thread *thr,
					bool withundo,
					int32_t	timeout,
					bool *timedout);
		static void	alarmHandler(int32_t signum);

	private:
		sqlrserverbaseprivate	*pvt;
