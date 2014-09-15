#include "stdafx.h"
#include "common.h"
#include "cServer.h"
#include "cVirtualMachine.h"
#include "cPolity.h"
//#include <ilcplex/ilocplex.h>
//ILOSTLBEGIN

bool noPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iterationPlacement)
{
	//the requests is intentionally rejected

	*_iterationPlacement = 0;
	_request->setAccepted(false);
	return true;
}

bool enoughResource(double _cpu_residual,double _mem_residual,double _disk_residual,cVirtualMachine* _vm)
{
	return (_vm->getcpuRequired() <= _cpu_residual)&&(_vm->getmemRequired() <= _mem_residual)&&(_vm->getdiskRequired() <= _disk_residual);
}


bool enoughResource(cServer* _server,cVirtualMachine* _vm)
{
	double server_cpu_residual = _server->getcpuResidual();
	double server_mem_residual = _server->getmemResidual();
	double server_disk_residual = _server->getdiskResidual();

	double vm_cpu_required = _vm->getcpuRequired();
	double vm_mem_required = _vm->getmemRequired();
	double vm_disk_required = _vm->getdiskRequired();

	return (server_cpu_residual >= vm_cpu_required)&&(server_mem_residual >= vm_mem_required)&&(server_disk_residual >= vm_disk_required);
}

bool enoughResource(cServer* _server,double _cpu_required,double _mem_required,double _disk_required)
{
	double server_cpu_residual = _server->getcpuResidual();
	double server_mem_residual = _server->getmemResidual();
	double server_disk_residual = _server->getdiskResidual();

	return (server_cpu_residual >= _cpu_required)&&(server_mem_residual >= _mem_required)&&(server_disk_residual >= _disk_required);
}

bool greedyVMPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iteration_placement)
{
	//This function places VMs with the greedy manner
	//First, we will take the servers with the descending order based on their number of  residual resources
	//Then, also take the VMs that will be deployed with the descending order based on its requirement to the resources
	//Finally, we try to find enough server to host

	*_iteration_placement = 0;
	multimap<double,cServer*> server_index_multimap;
	vector<cServer>::iterator iter_server_vec = _server_vec.begin();
	int found_vm_count = 0;
	double cpu_residual,mem_residual,disk_residual;

	//taking the order for VMs
	multimap<double,cVirtualMachine*> vm_index_multimap;
	vector<cVirtualMachine>::iterator iter_vm_vec = _request->vm_vec.begin();
	for (;iter_vm_vec != _request->vm_vec.end();iter_vm_vec++)
	{
		vm_index_multimap.insert(make_pair(iter_vm_vec->getcpuRequired() * iter_vm_vec->getmemRequired() * iter_vm_vec->getdiskRequired(),&(*iter_vm_vec)));
	}
	
	//taking the order for servers
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		server_index_multimap.insert(make_pair(iter_server_vec->getcpuResidual() * iter_server_vec->getmemResidual() * iter_server_vec->getdiskResidual(),&(*iter_server_vec)));
	}

	multimap<double,cServer*>::reverse_iterator reverse_iter_server_index_multimap = server_index_multimap.rbegin();
	multimap<double,cVirtualMachine*>::reverse_iterator reverse_iter_vm_index_multimap = vm_index_multimap.rbegin();
	for (;reverse_iter_server_index_multimap != server_index_multimap.rend();reverse_iter_server_index_multimap++)
	{
		cpu_residual = reverse_iter_server_index_multimap->second->getcpuResidual();
		mem_residual = reverse_iter_server_index_multimap->second->getmemResidual();
		disk_residual = reverse_iter_server_index_multimap->second->getdiskResidual();

		while(enoughResource(cpu_residual,mem_residual,disk_residual,reverse_iter_vm_index_multimap->second))
		{
			cpu_residual -= reverse_iter_vm_index_multimap->second->getcpuRequired();
			mem_residual -= reverse_iter_vm_index_multimap->second->getmemRequired();
			disk_residual -= reverse_iter_vm_index_multimap->second->getdiskRequired();
			
			reverse_iter_vm_index_multimap->second->setHostedServPoint(reverse_iter_server_index_multimap->second);
			reverse_iter_vm_index_multimap->second->setHostedServID(reverse_iter_server_index_multimap->second->getID());

			reverse_iter_vm_index_multimap++;
			if (reverse_iter_vm_index_multimap == vm_index_multimap.rend())
			{
				_request->setAccepted(true);
				return true;
			}
		}
	}

	_request->setAccepted(false);
	return false;
}

bool balanceVMPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iteration_placement)
{
	//This function places VMs with the greedy manner
	//First, we will take the servers with the descending order based on their number of  residual resources
	//Then, also take the VMs that will be deployed with the descending order based on its requirement to the resources
	//Finally, we try to find enough server to host

	*_iteration_placement = 0;
	multimap<double,cServer*> server_index_multimap;
	vector<cServer>::iterator iter_server_vec = _server_vec.begin();
	int found_vm_count = 0;
	double cpu_residual,mem_residual,disk_residual;

	//taking the order for VMs
	multimap<double,cVirtualMachine*> vm_index_multimap;
	vector<cVirtualMachine>::iterator iter_vm_vec = _request->vm_vec.begin();
	for (;iter_vm_vec != _request->vm_vec.end();iter_vm_vec++)
	{
		vm_index_multimap.insert(make_pair(iter_vm_vec->getcpuRequired() * iter_vm_vec->getmemRequired() * iter_vm_vec->getdiskRequired(),&(*iter_vm_vec)));
	}

	//taking the order for servers
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		server_index_multimap.insert(make_pair(iter_server_vec->getcpuResidual() * iter_server_vec->getmemResidual() * iter_server_vec->getdiskResidual(),&(*iter_server_vec)));
	}

	multimap<double,cServer*>::iterator iter_server_index_multimap = server_index_multimap.begin();
	multimap<double,cVirtualMachine*>::iterator iter_vm_index_multimap = vm_index_multimap.begin();
	for (;iter_server_index_multimap != server_index_multimap.end();iter_server_index_multimap++)
	{
		cpu_residual = iter_server_index_multimap->second->getcpuResidual();
		mem_residual = iter_server_index_multimap->second->getmemResidual();
		disk_residual = iter_server_index_multimap->second->getdiskResidual();

		if(enoughResource(cpu_residual,mem_residual,disk_residual,iter_vm_index_multimap->second))
		{
			cpu_residual -= iter_vm_index_multimap->second->getcpuRequired();
			mem_residual -= iter_vm_index_multimap->second->getmemRequired();
			disk_residual -= iter_vm_index_multimap->second->getdiskRequired();

			iter_vm_index_multimap->second->setHostedServPoint(iter_server_index_multimap->second);
			iter_vm_index_multimap->second->setHostedServID(iter_server_index_multimap->second->getID());

			iter_vm_index_multimap++;
			if (iter_vm_index_multimap == vm_index_multimap.end())
			{
				_request->setAccepted(true);
				return true;
			}
		}
	}
	return false;
}

//Use the following function to enumerate the all feasible placement solutions
bool addOne(vector<ID>& _vm_index,int _index,int _server_num)
{
	
	_vm_index[_index]++;

	if (_vm_index[_index] > _server_num)
	{
		if (_index == 0)
		{
			return false;
		}
		
		_vm_index[_index] = 1;
		return addOne(_vm_index,_index--,_server_num);
	}
	
	return true;
}

bool optimalVMPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iteration_placement)
{
	
	//This placement method will enumerate all feasible placement solution for currently arriving request
	//The reason why we use this simple method is because the objective associated with this optimization problem
	//can not be solved with cplex.

	int vm_num = _request->vm_vec.size();
	int server_num = _server_vec.size();

	map<ID,double>::iterator find_iter_server_cpu_required_map;
	map<ID,double>::iterator find_iter_server_mem_required_map;
	map<ID,double>::iterator find_iter_server_disk_required_map;

	map<ID,double>::iterator iter_server_cpu_required_map;
	map<ID,double>::iterator iter_server_mem_required_map;
	map<ID,double>::iterator iter_server_disk_required_map;

	static vector<ID> vm_index;
	
	//It's the first time this algorithm running
	//Initial the vector storing VMs for current request
	//It only happens when a new request arrives
	if (*_iteration_placement == 0)
	{
		int i = 0;
		for (i = 0;i < vm_num - 1; i++)
		{
			vm_index.push_back(1);
		}

		//set the value of latest element to be 0 to make the recursion function work normally.
		vm_index.push_back(0);
	}

	//Enumerate all possible feasible placement solution
	while(addOne(vm_index,vm_index.size() - 1,server_num))
	{		
		//find a potential placement configuration 

		//check whether it is satisfied the capacity constrain
		vector<cVirtualMachine>::iterator iter_vm = _request->vm_vec.begin();
		vector<ID>::const_iterator iter_vm_hosting_ID = vm_index.begin();

		
		map<ID,double> server_cpu_required_map;
		map<ID,double> server_mem_required_map;
		map<ID,double> server_disk_required_map;

		for (;iter_vm_hosting_ID != vm_index.end();iter_vm++,iter_vm_hosting_ID++)
		{
			find_iter_server_cpu_required_map = server_cpu_required_map.find(*iter_vm_hosting_ID);
			if (find_iter_server_cpu_required_map == server_cpu_required_map.end())
			{
				server_cpu_required_map.insert(make_pair(*iter_vm_hosting_ID,iter_vm->getcpuRequired()));
				server_mem_required_map.insert(make_pair(*iter_vm_hosting_ID,iter_vm->getmemRequired()));
				server_disk_required_map.insert(make_pair(*iter_vm_hosting_ID,iter_vm->getdiskRequired()));
			}
			else
			{
				server_cpu_required_map[*iter_vm_hosting_ID] += iter_vm->getcpuRequired();
				server_mem_required_map[*iter_vm_hosting_ID] += iter_vm->getmemRequired();
				server_disk_required_map[*iter_vm_hosting_ID] += iter_vm->getdiskRequired();
			}
		}

		iter_server_cpu_required_map = server_cpu_required_map.begin();
		iter_server_mem_required_map = server_mem_required_map.begin();
		iter_server_disk_required_map = server_disk_required_map.begin();

		_request->setAccepted(true);
		while(iter_server_cpu_required_map != server_cpu_required_map.end() && _request->getAccepted())
		{
			if (!enoughResource(&(_server_vec[iter_server_cpu_required_map->first - 1]),iter_server_cpu_required_map->second,iter_server_mem_required_map->second,iter_server_disk_required_map->second))
			{
				_request->setAccepted(false);
				server_cpu_required_map.clear();
				server_mem_required_map.clear();
				server_disk_required_map.clear();

				break;
			}

			iter_server_cpu_required_map++;
			iter_server_mem_required_map++;
			iter_server_disk_required_map++;
		}

		//Current placement deployment is not feasible under the capacity constrain
		//enumerate another potential solution
		if (_request->getAccepted())
		{
			*_iteration_placement = 1;

			vector<ID>::iterator iter_hosting_server_id = vm_index.begin();
			vector<cVirtualMachine>::iterator iter_vm = _request->vm_vec.begin();

			for (;iter_hosting_server_id != vm_index.end();iter_hosting_server_id++,iter_vm++)
			{
				iter_vm->setHostedServID(*iter_hosting_server_id);
				iter_vm->setHostedServPoint(&_server_vec[*iter_hosting_server_id - 1]);
			}


			return true;		
		}

	}
	
	*_iteration_placement = 0;
	vm_index.clear();

	return false;
}

//void optimalVMPlacement(vector<cServer>& _server_vec,cRequest* _request)
//{
//	//To be convenient, right now only one type of resource (CPU) will be taken account.
//	
//	IloInt sizeI,sizeJ;
//
//	IloInt vm_size = _request->vm_vec.size();
//	IloInt server_size = _server_vec.size();
//
//	int vm_index = 0;
//	int server_index = 0;
//
//	//Initial the running environment classes for cplex
//	IloEnv env;
//	IloModel model(env);
//	IloArray<IloIntVarArray> varx(env,vm_size);
//
//	//Initial placing variables
//	for (vm_index = 0;vm_index < vm_size; vm_index++)
//	{
//		varx[vm_index] = IloIntVarArray(env,server_size,0,1);
//	}
//
//	model.add(varx);
//
//	//Initial resource requirement for each VM
//	IloNumArray resource_requirement(env,vm_size);
//	vector<cVirtualMachine>::iterator iter_vm = _request->vm_vec.begin();
//	for (vm_index = 0;iter_vm != _request->vm_vec.end();iter_vm++,vm_index++)
//	{
//		resource_requirement[vm_index] = iter_vm->getcpuRequired();
//	}
//
//	//Initial the residual capacity for each server
//	IloNumArray server_capacity(env,server_size);
//	vector<cServer>::iterator iter_server_vec = _server_vec.begin();
//	for (server_index = 0;iter_server_vec!= _server_vec.end();iter_server_vec++,server_index++)
//	{
//		server_capacity[server_index] = iter_server_vec->getcpuResidual();
//	}
//	
//	//Constraining that each VM will be deployed on only one server
//	for (vm_index = 0; vm_index < vm_size; vm_index++)
//	{
//		IloExpr express1(env);
//		for (server_index = 0;server_index < server_size; server_index++)
//		{
//			express1 += varx[vm_index][server_index];
//		}
//		model.add(express1 <= 1);
//		express1.end();
//	}
//
//	//Constraining that the number of resource required by the requests that are allocated on server i will be no more than its capacity
//	for (server_index = 0;server_index < server_size; server_index++)
//	{
//		IloExpr express2(env);
//		for (vm_index = 0; vm_index < vm_size;vm_index++)
//		{
//			express2 += resource_requirement[vm_index];
//		}
//
//		model.add(express2 <= server_capacity[server_index]);
//		express2.end();
//	}
//	
//
//	try {
//		IloModel model(env);
//		IloNumVarArray vars(env);
//		vars.add(IloNumVar(env, 0.0, 40.0));
//		vars.add(IloNumVar(env));
//		vars.add(IloNumVar(env));
//		model.add(IloMaximize(env, vars[0] + 2 * vars[1] + 3 * vars[2]));
//		model.add( - vars[0] + vars[1] + vars[2] <= 20);
//		model.add( vars[0] - 3 * vars[1] + vars[2] <= 30);
//		IloCplex cplex(model);
//		if ( !cplex.solve() ) {
//			env.error() << "Failed to optimize LP." << endl;
//			throw(-1);
//		}
//		IloNumArray vals(env);
//		env.out() << "Solution status = " << cplex.getStatus() << endl;
//		env.out() << "Solution value = " << cplex.getObjValue() << endl;
//		cplex.getValues(vals, vars);
//		env.out() << "Values = " << vals << endl;
//	}
//	catch (IloException& e) {
//		cerr << "Concert exception caught: " << e << endl;
//	}
//	catch (...) {
//		cerr << "Unknown exception caught" << endl;
//	}
//	env.end();
//	return 0;
//}

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

	policy.first = "OPTIMAL";
	policy.second = optimalVMPlacement;
	policy_vec.push_back(policy);

	return;
}

void initialPolicies(unsigned int _conf_policy)
{
	/*********************************************
	10 -- LOAD_BALANCE;
    11 -- LOAD_BALANCE + NO_PLACEMENT;
	20 -- GREEDY;
    21 -- GREEDY + NO_PLACEMENT; 
	30 -- OPTIMAL;
	31 -- OPTIMAL + NO_PLACEMENT;
	40 -- LOAD_BALANCE + GREEDY;
	41 -- LOAD_BALANCE + GREEDY + NO_PLACEMENT;
	*********************************************/
	policy_vec.clear();

	pair<string,placementfunction> policy;

	div_t t = div(_conf_policy,10);
		
	if (t.quot == 1)
	{
		policy.first = "BALANCE";
		policy.second = balanceVMPlacement;
		policy_vec.push_back(policy);
		
		if (t.rem == 1)
		{
			policy.first = "NO_PLACEMENT";
			policy.second = noPlacement;
			policy_vec.push_back(policy);
		}
	}
	else if (t.quot == 2)
	{
		policy.first = "GREEDY";
		policy.second = greedyVMPlacement;
		policy_vec.push_back(policy);

		if (t.rem == 1)
		{
			policy.first = "NO_PLACEMENT";
			policy.second = noPlacement;
			policy_vec.push_back(policy);
		}
	}
	else if (t.quot == 3)
	{
		policy.first = "OPTIMAL";
		policy.second = optimalVMPlacement;
		policy_vec.push_back(policy);

		if (t.rem == 1)
		{
			policy.first = "NO_PLACEMENT";
			policy.second = noPlacement;
			policy_vec.push_back(policy);
		}
	}
	else if (t.quot == 4)
	{
		policy.first = "GREEDY";
		policy.second = greedyVMPlacement;
		policy_vec.push_back(policy);

		policy.first = "BALANCE";
		policy.second = balanceVMPlacement;
		policy_vec.push_back(policy);

		if (t.rem == 1)
		{
			policy.first = "NO_PLACEMENT";
			policy.second = noPlacement;
			policy_vec.push_back(policy);
		}
	}

	return;
}

