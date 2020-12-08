#include "Resource.h"

const Resource &ddefault()
{
	static Resource r("", 1, 1);
	return r;
}

bool Resource::isSingleResource()
{
	return resource_num == 1;
}

bool Resource::isFull()
{
	return free_unit_num == 0;
}

string Resource::get_name(){
	return "R"+RID;
}

int Resource::get_free_unit_num(){
	return free_unit_num;
}

vector<Resource::ProcessInfo> Resource::get_waiting_list(){
	return waiting_list;
}


Resource::Resource(string RID, int resource_num, int free_unit_num)
	: RID(RID), resource_num(resource_num), free_unit_num(free_unit_num)
{
}

int Resource::request(string PID, int req_resource_num)
{
	if (req_resource_num > resource_num)
	{ //请求资源数非法
		throw "Error! Requested resource exceeded upper limit! ";
		return -1;
	}
	else
	{
		if (req_resource_num <= free_unit_num)
		{ //分配资源
			free_unit_num = free_unit_num - req_resource_num;
			return 0;
		}
		else
		{ //资源暂时不足，加入等待队列
			waiting_list.push_back({PID, req_resource_num});
			return 1;
		}
	}
}

string Resource::release(int release_resource_num)
{
	free_unit_num = free_unit_num + release_resource_num;

	if (!waiting_list.empty())
	{ //等待列表非空
		auto &next_process = *(waiting_list.begin());
		if (next_process.req_resource_num <= free_unit_num)
		{
			//分配资源
			string next_process_id = next_process.PID;
			free_unit_num = free_unit_num - next_process.req_resource_num;
			waiting_list.erase(waiting_list.begin());
			return next_process_id;
		}
		else
		{
			return "";
		}
	}
	else
	{
		return "";
	}
}