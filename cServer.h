#pragma once
#include "stdafx.h"
#include "cRequest.h"
#include "cVirtualMachine.h"

class cServer
{
private:
	ID id;

	//The total capacity specified
	double cpu_capacity;
	double mem_capacity;
	double disk_capacity;

	//the number of residual w.r.t different types of resources 
	double cpu_residual;
	double mem_residual;
	double disk_residual;

public:
	list<cVirtualMachine*> hosted_vm_list;

public:
	void setID(ID _id){id = _id;}
	ID   getID(void) const {return id;}

	void setcpuCapacity(double _capacity){cpu_capacity = _capacity;}
	double getcpuCapacity(void) const {return cpu_capacity;}

	void setmemCapacity(double _capacity){mem_capacity = _capacity;}
	double getmemCapacity(void) const {return mem_capacity;}

	void setdiskCapacity(double _capacity){disk_capacity = _capacity;}
	double getdiskCapacity(void) const {return disk_capacity;}

	void setcpuResidual(double _residual){cpu_residual = _residual;}
	double getcpuResidual(void) const {return cpu_residual;}

	void setmemResidual(double _residual){mem_residual = _residual;}
	double getmemResidual(void) const {return mem_residual;}

	void setdiskResidual(double _residual){disk_residual = _residual;}
	double getdiskResidual(void) const {return disk_residual;}

public:
	void allocateVirtMach(cVirtualMachine* _vmine);
	void releaseVirtMach(cVirtualMachine* _vmine);
	
public:
	cServer(void);
	cServer(ID _id,double _cpu_capacity,double _mem_capacity,double _disk_capacity);
	cServer(const cServer& _server);
	cServer& operator=(const cServer& _server);
	bool operator==(const cServer& _server);
	~cServer(void);
};

