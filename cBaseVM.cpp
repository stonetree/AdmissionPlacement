#include "StdAfx.h"
#include "cBaseVM.h"


cBaseVM::cBaseVM(void)
{
}


cBaseVM::~cBaseVM(void)
{
}

cBaseVM::cBaseVM(VMtype _type, resource_unit _cpu_required, resource_unit _mem_required, resource_unit _disk_required)
{
	type = _type;
	cpu_required = _cpu_required;
	mem_required = _mem_required;
	disk_required = _disk_required;
}

cBaseVM::cBaseVM(const cBaseVM& _base_vm_type)
{
	this->operator=(_base_vm_type);
}

cBaseVM& cBaseVM::operator=(const cBaseVM& _base_vm_type)
{
	if (this != &_base_vm_type)
	{
		type = _base_vm_type.type;
		cpu_required = _base_vm_type.cpu_required;
		mem_required = _base_vm_type.mem_required;
		disk_required = _base_vm_type.disk_required;
	}

	return *this;
}