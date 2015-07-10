class SQLRSERVER_DLLSPEC sqlrresultsettranslation {
	public:
			sqlrresultsettranslation(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters);
		virtual	~sqlrresultsettranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength);
	protected:
		sqlrresultsettranslations	*sqlrrsts;
		xmldomnode			*parameters;
};
