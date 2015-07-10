class SQLRSERVER_DLLSPEC sqlrauth {
	public:
			sqlrauth(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		virtual	~sqlrauth();
		virtual	bool	authenticate(const char *user,
						const char *password);
	protected:
		xmldomnode		*parameters;
		sqlrpwdencs		*sqlrpe;
};
