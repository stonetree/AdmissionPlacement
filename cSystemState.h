#pragma once
#include "cSystemState.h"
#include "common.h"

#include "cPolity.h"

class cServerCopy
{
private:
	resource_unit cpu_residual;
	resource_unit mem_residual;
	resource_unit disk_residual;
public:
	void setcpuRedisual(resource_unit _cpu){cpu_residual = _cpu;}
	resource_unit getcpuResidual(void)const { return cpu_residual; }

	void setmemResidual(resource_unit _mem){ mem_residual = _mem; }
	resource_unit getmemResidual(void)const { return mem_residual; }

	void setdiskResidual(resource_unit _disk){ disk_residual = _disk; }
	resource_unit getdiskResidual(void)const { return disk_residual; }


public:
	cServerCopy(resource_unit _cpu_residual, resource_unit _mem_residual, resource_unit _disk_residual) :cpu_residual(_cpu_residual), mem_residual(_mem_residual), disk_residual(_disk_residual){}
	cServerCopy(const cServerCopy& _server_copy):cpu_residual(_server_copy.cpu_residual),mem_residual(_server_copy.mem_residual),disk_residual(_server_copy.disk_residual){}
	cServerCopy(void);
	~cServerCopy(void);

};

class cSystemState
{
private:
	pair<requesttype,double> system_state;
public:
	map<string,int> system_policy;

	//currently, it is not used
	//vector<cServerCopy> server_copy_vec;

public:
	void setSystemStateIndicator(pair<requesttype,double> _system_state){system_state = _system_state;}
	pair<requesttype,double> getSystemStateIndicator(void) const {return system_state;}
public:
	cSystemState(void);
	cSystemState(requesttype _request_type,double _state_indicator){system_state.first = _request_type;system_state.second = _state_indicator;}
	~cSystemState(void);
};

