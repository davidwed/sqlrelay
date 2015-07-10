#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>

using namespace rudiments;

class SQLRSERVER_DLLSPEC reversecase : public sqlrpwdenc {
	public:
			reversecase(xmldomnode *parameters);
		char    *encrypt(const char *value);
		char    *decrypt(const char *value);
	private:
		char    *reverse(const char *value);
};

reversecase::reversecase(xmldomnode *parameters) : sqlrpwdenc(parameters) {
}

char *reversecase::encrypt(const char *value) {
	return reverse(value);
}

char *reversecase::decrypt(const char *value) {
	return reverse(value);
}

char *reversecase::reverse(const char *value) {
	char    *retval=charstring::duplicate(value);
	for (char *c=retval; *c; c++) {
		if (character::isUpperCase(*c)) {
			*c=character::toLowerCase(*c);
		} else if (character::isLowerCase(*c)) {
			*c=character::toUpperCase(*c);
		}
	}
	return retval;
}

extern "C" {
	sqlrpwdenc SQLRSERVER_DLLSPEC *new_reversecase(xmldomnode *parameters) {
		return new reversecase(parameters);
	}
}
