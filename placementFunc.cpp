#include "stdafx.h"
#include "common.h"
#include "cServer.h"
#include "cVirtualMachine.h"
#include "cPolity.h"

bool noPlacement(vector<cServer>& _server_vec,cRequest* _request)
{
	//do nothing
	return true;
}

bool enoughResource(double _cpu_residual,double _mem_residual,double _disk_residual,cVirtualMachine* _vm)
{
	return (_vm->getcpuRequired() <= _cpu_residual)&&(_vm->getmemRequired() <= _mem_residual)&&(_vm->getdiskRequired() <= _disk_residual);
}

bool greedyVMPlacement(vector<cServer>& _server_vec,cRequest* _request)
{
	//This function places VMs with the greedy manner
	//First, we will take the servers with the descending order based on their number of  residual resources
	//Then, also take the VMs that will be deployed with the descending order based on its requirement to the resources
	//Finally, we try to find enough server to host
	map<double,cServer*> server_index_map;
	vector<cServer>::iterator iter_server_vec = _server_vec.begin();
	int found_vm_count = 0;
	double cpu_residual,mem_residual,disk_residual;

	//taking the order for VMs
	map<double,cVirtualMachine*> vm_index_map;
	vector<cVirtualMachine>::iterator iter_vm_vec = _request->vm_vec.begin();
	for (;iter_vm_vec != _request->vm_vec.end();iter_vm_vec++)
	{
		vm_index_map.insert(make_pair(iter_vm_vec->getcpuRequired() * iter_vm_vec->getmemRequired() * iter_vm_vec->getdiskRequired(),&(*iter_vm_vec)));
	}
	
	//taking the order for servers
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		server_index_map.insert(make_pair(iter_server_vec->getcpuResidual() * iter_server_vec->getmemResidual() * iter_server_vec->getdiskResidual(),&(*iter_server_vec)));
	}

	map<double,cServer*>::reverse_iterator reverse_iter_server_index_map = server_index_map.rbegin();
	map<double,cVirtualMachine*>::reverse_iterator reverse_iter_vm_index_map = vm_index_map.rbegin();
	for (;reverse_iter_server_index_map != server_index_map.rend();reverse_iter_server_index_map++)
	{
		cpu_residual = reverse_iter_server_index_map->second->getcpuResidual();
		mem_residual = reverse_iter_server_index_map->second->getmemResidual();
		disk_residual = reverse_iter_server_index_map->second->getdiskResidual();

		while(enoughResource(cpu_residual,mem_residual,disk_residual,reverse_iter_vm_index_map->second))
		{
			cpu_residual -= reverse_iter_vm_index_map->second->getcpuRequired();
			mem_residual -= reverse_iter_vm_index_map->second->getmemRequired();
			disk_residual -= reverse_iter_vm_index_map->second->getdiskRequired();
			
			reverse_iter_vm_index_map->second->setHostedServPoint(reverse_iter_server_index_map->second);
			reverse_iter_vm_index_map->second->setHostedServID(reverse_iter_server_index_map->second->getID());

			reverse_iter_vm_index_map++;
			if (reverse_iter_vm_index_map == vm_index_map.rend())
			{
				return true;
			}
		}
	}

	
	return false;
}

bool balanceVMPlacement(vector<cServer>& _server_vec,cRequest* _request)
{
	//This function places VMs with the greedy manner
	//First, we will take the servers with the descending order based on their number of  residual resources
	//Then, also take the VMs that will be deployed with the descending order based on its requirement to the resources
	//Finally, we try to find enough server to host
	map<double,cServer*> server_index_map;
	vector<cServer>::iterator iter_server_vec = _server_vec.begin();
	int found_vm_count = 0;
	double cpu_residual,mem_residual,disk_residual;

	//taking the order for VMs
	map<double,cVirtualMachine*> vm_index_map;
	vector<cVirtualMachine>::iterator iter_vm_vec = _request->vm_vec.begin();
	for (;iter_vm_vec != _request->vm_vec.end();iter_vm_vec++)
	{
		vm_index_map.insert(make_pair(iter_vm_vec->getcpuRequired() * iter_vm_vec->getmemRequired() * iter_vm_vec->getdiskRequired(),&(*iter_vm_vec)));
	}

	//taking the order for servers
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		server_index_map.insert(make_pair(iter_server_vec->getcpuResidual() * iter_server_vec->getmemResidual() * iter_server_vec->getdiskResidual(),&(*iter_server_vec)));
	}

	map<double,cServer*>::iterator iter_server_index_map = server_index_map.begin();
	map<double,cVirtualMachine*>::iterator iter_vm_index_map = vm_index_map.begin();
	for (;iter_server_index_map != server_index_map.end();iter_server_index_map++)
	{
		cpu_residual = iter_server_index_map->second->getcpuResidual();
		mem_residual = iter_server_index_map->second->getmemResidual();
		disk_residual = iter_server_index_map->second->getdiskResidual();

		while(enoughResource(cpu_residual,mem_residual,disk_residual,iter_vm_index_map->second))
		{
			cpu_residual -= iter_vm_index_map->second->getcpuRequired();
			mem_residual -= iter_vm_index_map->second->getmemRequired();
			disk_residual -= iter_vm_index_map->second->getdiskRequired();

			iter_vm_index_map->second->setHostedServPoint(iter_server_index_map->second);
			iter_vm_index_map->second->setHostedServID(iter_server_index_map->second->getID());

			iter_vm_index_map++;
			if (iter_vm_index_map == vm_index_map.end())
			{
				return true;
			}
		}
	}


	return false;
}

void initialPolicies()
{
	policy_vec.clear();

	pair<string,placementfunction> policy;
	policy.first = "NO_PLACEMENT";
	policy.second = noPlacement;
	policy_vec.push_back(policy);

	policy.first = "GREEDY";
	policy.second = greedyVMPlacement;
	policy_vec.push_back(policy);

	policy.first = "BALANCE";
	policy.second = balanceVMPlacement;
	policy_vec.push_back(policy);

	return;
}