class SQLRSERVER_DLLSPEC sqlrfilter {
	public:
			sqlrfilter(sqlrfilters *sqlrfs,
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrfilter();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	protected:
		sqlrfilters		*sqlrfs;
		xmldomnode		*parameters;
		bool			debug;
};
