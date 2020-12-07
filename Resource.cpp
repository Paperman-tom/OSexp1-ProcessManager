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

Resource::Resource(string RID, int resource_num, int free_unit_num)
	: RID(RID), resource_num(resource_num), free_unit_num(free_unit_num)
{
}

int Resource::request(string PID, int req_resource_num)
{
	if (req_resource_num > resource_num)
	{ //������Դ���Ƿ�
		throw "Error! Requested resource exceeded upper limit! ";
		return -1;
	}
	else
	{
		if (req_resource_num <= free_unit_num)
		{ //������Դ
			free_unit_num = free_unit_num - req_resource_num;
			return 0;
		}
		else
		{ //��Դ��ʱ���㣬����ȴ�����
			waiting_list.push_back({PID, req_resource_num});
			return 1;
		}
	}
}

string Resource::release(int release_resource_num)
{
	free_unit_num = free_unit_num + release_resource_num;

	if (!waiting_list.empty())
	{ //�ȴ��б�ǿ�
		auto &next_process = *(waiting_list.begin());
		if (next_process.req_resource_num <= free_unit_num)
		{
			//������Դ
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