class SQLRSERVER_DLLSPEC sqlrpwdenc {
	public:
			sqlrpwdenc(xmldomnode *parameters);
		virtual	~sqlrpwdenc();
		virtual const char	*getId();
		virtual	bool	oneWay();
		virtual	char	*encrypt(const char *value);
		virtual	char	*decrypt(const char *value);
	protected:
		xmldomnode	*parameters;
};
