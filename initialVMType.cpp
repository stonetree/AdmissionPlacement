#include "stdafx.h"
#include "cBaseVM.h"


//define the parameter w.r.t the configurations of different types of VMs in terms of the num of resources specified.
//VM referred as MEDIUM
const double vm_medium_cpu = 3;
const double vm_medium_mem = 3.75;
const double vm_medium_disk = 4;

//VM referred as LARGE
const double vm_large_cpu = 6.5;
const double vm_large_mem = 7.5;
const double vm_large_disk = 32;

//VM referred as XLARGE
const double vm_xlarge_cpu = 13;
const double vm_xlarge_mem = 15;
const double vm_xlarge_disk = 80;

/**********************************************/

//initial base VM types
void initialbase_vm_typeType(map<VMtype,cBaseVM>& _base_vm_type_map)
{
	_base_vm_type_map.clear();

	_base_vm_type_map.insert(make_pair(MEDIUM,cBaseVM(MEDIUM,vm_medium_cpu,vm_medium_mem,vm_medium_disk)));
	_base_vm_type_map.insert(make_pair(LARGE,cBaseVM(LARGE,vm_large_cpu,vm_large_mem,vm_large_disk)));
	_base_vm_type_map.insert(make_pair(XLARGE,cBaseVM(XLARGE,vm_xlarge_cpu,vm_xlarge_mem,vm_xlarge_disk)));

	return;
}