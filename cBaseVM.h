#pragma once

#include "stdafx.h"

class cBaseVM
{
private:
	VMtype type;
	double cpu_required;
	double mem_required;
	double disk_required;

public:
	void setVMType(VMtype _type){type = _type;}
	VMtype getVMType(void) const {return type;}

	void setcpuRequired(double _required){cpu_required = _required;}
	double getcpuRequired(void) const {return cpu_required;}

	void setmemRequired(double _required){mem_required = _required;}
	double getmemRequired(void) const {return mem_required;}

	void setdiskRequired(double _required){disk_required = _required;}
	double getdiskRequired(void) const{return disk_required;}

public:
	cBaseVM(void);
	cBaseVM(VMtype _type,double _cpu_required,double _mem_required,double _disk_required);
	cBaseVM(const cBaseVM& _base_vm_type);
	cBaseVM& operator=(const cBaseVM& _base_vm_type);
	~cBaseVM(void);
};

