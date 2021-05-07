#include "subst.h"
#include "envvars.h"

char *do_substitutions(const char *str)
{
	char *res;
	res = subst_envvars(str);
	return res;
}
