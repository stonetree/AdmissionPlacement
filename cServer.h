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

	void setcpu_capacity(double _capacity){cpu_capacity = _capacity;}
	double getcpu_capacity(void) const {return cpu_capacity;}

	void setmem_capacity(double _capacity){mem_capacity = _capacity;}
	double getmem_capacity(void) const {return mem_capacity;}

	void setdisk_capacity(double _capacity){disk_capacity = _capacity;}
	double getdisk_capacity(void) const {return disk_capacity;}

	void setcpu_residual(double _residual){cpu_residual = _residual;}
	double getcpu_residual(void) const {return cpu_residual;}

	void setmem_residual(double _residual){mem_residual = _residual;}
	double getmem_residual(void) const {return mem_residual;}

	void setdisk_residual(double _residual){disk_residual = _residual;}
	double getdisk_residual(void) const {return disk_residual;}

public:
	void allocateVirtMach(cVirtualMachine* _vmine);
	void releaseVirtMach(cVirtualMachine* _vmine);
	
public:
	cServer(void);
	cServer(ID _id,double _cpu_capacity,double _mem_capacity,double _disk_capacity,double _cpu_residual = 0,double _mem_residual = 0,double _disk_residual = 0);
	cServer(const cServer& _server);
	cServer& operator=(const cServer& _server);
	bool operator==(const cServer& _server);
	~cServer(void);
};

