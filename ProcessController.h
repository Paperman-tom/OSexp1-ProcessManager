#pragma once
#include <vector>
#include <string>
#include "Process.h"
#include "Dispatcher.h"
using namespace std;

class ProcessController
{
public:
	ProcessController();

	void insert_process(string PID, Process p);
	void destroy_process(string PID);
	Process &get_process(string PID);
	void scheduler(Dispatcher &dispatcher);
	void timeout(Dispatcher &dispatcher);

	map<string, Process> process_map;
};