#pragma once
#include "stdafx.h"
#include "cRequest.h"
#include "cVirtualMachine.h"

class cServer
{
private:
	ID id;

	//The total capacity specified
	resource_unit cpu_capacity;
	resource_unit mem_capacity;
	resource_unit disk_capacity;

	//the number of residual w.r.t different types of resources 
	resource_unit cpu_residual;
	resource_unit mem_residual;
	resource_unit disk_residual;

public:
	list<cVirtualMachine*> hosted_vm_list;

public:
	void setID(ID _id){id = _id;}
	ID   getID(void) const {return id;}

	void setcpuCapacity(resource_unit _capacity){ cpu_capacity = _capacity; }
	resource_unit getcpuCapacity(void) const { return cpu_capacity; }

	void setmemCapacity(resource_unit _capacity){ mem_capacity = _capacity; }
	resource_unit getmemCapacity(void) const { return mem_capacity; }

	void setdiskCapacity(resource_unit _capacity){ disk_capacity = _capacity; }
	resource_unit getdiskCapacity(void) const { return disk_capacity; }

	void setcpuResidual(resource_unit _residual){ cpu_residual = _residual; }
	resource_unit getcpuResidual(void) const { return cpu_residual; }

	void setmemResidual(resource_unit _residual){ mem_residual = _residual; }
	resource_unit getmemResidual(void) const { return mem_residual; }

	void setdiskResidual(resource_unit _residual){ disk_residual = _residual; }
	resource_unit getdiskResidual(void) const { return disk_residual; }

public:
	void allocateVirtMach(cVirtualMachine* _vmine);
	void releaseVirtMach(cVirtualMachine* _vmine);
	
public:
	cServer(void);
	cServer(ID _id, resource_unit _cpu_capacity, resource_unit _mem_capacity, resource_unit _disk_capacity);
	cServer(const cServer& _server);
	cServer& operator=(const cServer& _server);
	bool operator==(const cServer& _server);
	~cServer(void);
};

