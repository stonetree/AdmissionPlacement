#pragma once

class cConfiguration
{
private:
	double conf_workload_rate;

	double conf_low_arrival_rate;
	double conf_high_arrival_rate;
	double conf_xhigh_arrival_rate;

	/*********************************************
	10 -- LOAD_BALANCE;
    11 -- LOAD_BALANCE + NO_PLACEMENT;
	20 -- GREEDY;
    21 -- GREEDY + NO_PLACEMENT; 
	30 -- OPTIMAL;
	31 -- OPTIMAL + NO_PLACEMENT;
	40 -- LOAD_BALANCE + GREEDY;
	*********************************************/
	unsigned int conf_runing_policy;

public:
	void setWorkLoadRate(double _rate){conf_workload_rate = _rate;}
	double getWorkLoadRate(void) const {return conf_workload_rate;}

	void setLowArrivalRate(double _rate){conf_low_arrival_rate = _rate;}
	double getLowArrivalRate(void) const {return conf_low_arrival_rate;}

	void setHighArrivalRate(double _rate){conf_high_arrival_rate = _rate;}
	double getHighArrivalRate(void) const {return conf_high_arrival_rate;}

	void setXhighArrivalRate(double _rate){conf_xhigh_arrival_rate = _rate;}
	double getXhighArrivalRate(void) const {return conf_xhigh_arrival_rate;}

	void setPolicy(unsigned int _policy){conf_runing_policy = _policy;}
	unsigned int getPolicy(void) const {return conf_runing_policy;}

public:
	cConfiguration(void);
	cConfiguration(const cConfiguration& _config);
	cConfiguration& operator=(const cConfiguration& _config);
	cConfiguration(double _rate,double _low_arrival_rate,double _high_arrival_rate,double _xhigh_arrival_rate,unsigned int _policy):conf_workload_rate(_rate),conf_low_arrival_rate(_low_arrival_rate),\
		conf_high_arrival_rate(_high_arrival_rate),conf_xhigh_arrival_rate(_xhigh_arrival_rate),conf_runing_policy(_policy){}
	~cConfiguration(void);
};

