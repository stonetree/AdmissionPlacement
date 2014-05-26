#pragma once

#include "stdafx.h"


class cService
{
private:
	servicetype service_type;

	double unit_reward;
	double unit_penalty;
public:
	map<VMtype,int>  vm_request_map;

public:
	void setServType(servicetype _type){service_type = _type;}
	servicetype getServType(void) const {return service_type;}


	void setUniReward(double _uniReward) {unit_reward = _uniReward;}
	double getUnitReward(void) const {return unit_reward;}

	void setUnitPenalty(double _unit_penalty){unit_penalty = _unit_penalty;}
	double getUnitPenalty(void) const {return unit_penalty;}

public:
	cService(void);
	cService(servicetype _type);
	cService(servicetype _type,vector<pair<VMtype,int>>& _vm_request_vec,double _unit_reward,double _unit_penalty);
	cService(const cService& _service);
	cService& operator=(const cService& _service);
	~cService(void);
};

