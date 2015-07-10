class SQLRSERVER_DLLSPEC sqlrtranslation {
	public:
			sqlrtranslation(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrtranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);
	protected:
		sqlrtranslations	*sqlts;
		xmldomnode		*parameters;
		bool			debug;
};
