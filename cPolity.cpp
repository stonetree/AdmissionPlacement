#include "StdAfx.h"
#include "cPolity.h"


cPolity::cPolity(void)
{
}


cPolity::~cPolity(void)
{
}

cPolity::cPolity(const cPolity& _polity)
{
	this->operator=(_polity);
}

cPolity& cPolity::operator=(const cPolity& _polity)
{
	if (this != &_polity)
	{
		system_state_policy.clear();
		system_state_policy.insert(_polity.system_state_policy.begin(),_polity.system_state_policy.end());
	}

	return *this;
}