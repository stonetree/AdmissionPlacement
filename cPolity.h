#pragma once
#include "stdafx.h"
#include "common.h"


class cPolity
{
public:
	map<string,int> system_state_policy;

public:
	cPolity(void);
	cPolity(const cPolity& _policy);
	cPolity& operator=(const cPolity& _policy);
	~cPolity(void);
};

