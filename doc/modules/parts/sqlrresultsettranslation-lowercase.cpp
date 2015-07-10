#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>

class SQLRSERVER_DLLSPEC lowercase : public sqlrresultsettranslation {
	public:
			lowercase(sqlrresultsettranslations *sqlrrsts,
						xmldomnode *parameters);

		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength);
	private:
		stringbuffer	newfieldbuffer;
};

lowercase::lowercase(sqlrresultsettranslations *sqlrrsts,
						xmldomnode *parameters) :
				sqlrresultsettranslation(sqlrrsts,parameters) {
}

bool lowercase::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength) {

	newfieldbuffer.clear();

	for (uint32_t i=0; i<fieldlength; i++) {
		newfieldbuffer.append(character::toLower(field[i]));
	}

	*newfield=newfieldbuffer.getString();
	*newfieldlength=newfieldbuffer.getStringLength();

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsettranslation
				*new_sqlrresultsettranslation_mask(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters) {
		return new mask(sqlrrsts,parameters);
	}
}
