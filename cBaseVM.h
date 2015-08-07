#pragma once

#include "stdafx.h"

class cBaseVM
{
private:
	VMtype type;
	resource_unit cpu_required;
	resource_unit mem_required;
	resource_unit disk_required;

public:
	void setVMType(VMtype _type){type = _type;}
	VMtype getVMType(void) const {return type;}

	void setcpuRequired(resource_unit _required){ cpu_required = _required; }
	resource_unit getcpuRequired(void) const { return cpu_required; }

	void setmemRequired(resource_unit _required){ mem_required = _required; }
	resource_unit getmemRequired(void) const { return mem_required; }

	void setdiskRequired(resource_unit _required){ disk_required = _required; }
	resource_unit getdiskRequired(void) const{ return disk_required; }

public:
	cBaseVM(void);
	cBaseVM(VMtype _type, resource_unit _cpu_required, resource_unit _mem_required, resource_unit _disk_required);
	cBaseVM(const cBaseVM& _base_vm_type);
	cBaseVM& operator=(const cBaseVM& _base_vm_type);
	~cBaseVM(void);
};

