#include "Process.h"
#include <iostream>
Process &default_process()
{
	static const Process::Status default_status = {Process::none, Process::BlockedL};
	static Process p("Default process name", "", default_status, Process::Init, "");
	return p;
}

Process::Process()
	: name(default_process().name),
	  PID(default_process().PID),
	  status(default_process().status),
	  priority(default_process().priority),
	  parent_process(default_process().parent_process)
{
}

Process::Process(string name, string PID, Status status, PriorityType priority, string parent_process)
	: name(name),
	  PID(PID),
	  status(status),
	  priority(priority),
	  parent_process(parent_process)
{
}
Process &Process::add_children_process(string name, string PID, Status status, PriorityType priority)
{
	Process child(name, PID, status, priority, this->PID); //创建子进程
	if (child.status.process_type != Process::ready)
	{ //设置进程状态为ready
		child.set_process_status(ready);
	}
	children.insert(make_pair(child.PID, child)); //设置进程IDmap

	return (*children.find(PID)).second;
}

void Process::set_parent_process(string PID)
{
	parent_process = PID;
}

void Process::get_resource(string RID, int resource_num)
{
	auto resource = other_resource.find(RID);
	if (resource != other_resource.end())
	{ //已在资源列表即增加其数量
		(*resource).second = (*resource).second + resource_num;
	}
	else
	{ //未在资源列表即插入
		other_resource.insert(make_pair(RID, resource_num));
	}
}

int Process::relase_resource(string RID)
{
	auto resource = other_resource.find(RID);
	if (resource != other_resource.end())
	{
		//释放对应资源
		int resource_num = resource->second;
		other_resource.erase(RID);
		return resource_num;
	}
	else
	{
		//没有找到对应资源
		return 0;
	}
}

void Process::change_status(ProcessOperate operater)
{
	switch (operater)
	{
	case create:
		if (status.process_type == none)
		{
			set_process_status(ready);
		}
		else
		{
			throw "Status error!";
		}
		break;
	case destroy:
		if (status.process_type == ready || status.process_type == running || status.process_type == blocked)
		{
			set_process_status(none);
		}
		else
		{
			throw "Status error!";
		}
		break;
	case request:
		if (status.process_type == running)
		{
			set_process_status(blocked);
			set_list_status(BlockedL);
		}
		else
		{
			throw "Status error!";
		}
		break;
	case release:
		if (status.process_type == blocked)
		{
			set_process_status(ready);
			set_list_status(ReadyL);
		}
		else
		{
			throw "Status error!";
		}
		break;
	case timeout:
		if (status.process_type == running)
		{
			set_process_status(ready);
		}
		else
		{
			throw "Status error!";
		}
		break;
	case dispatch:
		if (status.process_type == running)
		{
			set_process_status(ready);
		}
		else if (status.process_type == ready)
		{
			set_process_status(running);
		}
		else
		{
			throw "Status error!";
		}
		break;
	default:
		break;
	}
}

map<string, Process> &Process::get_children()
{
	return children;
}

Process::PriorityType Process::get_priority()
{
	return priority;
}

string Process::get_PID()
{
	return PID;
}

string Process::get_name()
{
	return name;
}

void Process::set_process_status(ProcessType process_type)
{
	status.process_type = process_type;
}

void Process::set_list_status(ListType list_type)
{
	status.list_type = list_type;
}

void Process::relase_all_resource(ResourceController resource_controller, string *set_ready_process)
{
	//递归释放所有子进程资源
	string *temp;
	auto children_map_it = children.begin();
	while (children_map_it != children.end())
	{
		Process &child = children_map_it->second;
		child.relase_all_resource(resource_controller, temp);
		++children_map_it;
	}
	//释放进程资源
	auto resource_map_it = other_resource.cbegin();
	//string set_ready_process[4];
	for (int i = 0; resource_map_it != other_resource.cend(); ++i, ++resource_map_it)
	{
		Resource &r = resource_controller.get_resource(resource_map_it->first);
		set_ready_process[i] = r.release(resource_map_it->second);
		cout<< "Relase "<<r.get_name()<<" , ";
	}
}
