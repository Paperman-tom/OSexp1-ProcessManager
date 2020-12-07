#pragma once
#include <vector>
#include <string>
using namespace std;

class Resource
{
public:
	struct ProcessInfo
	{
		string PID;
		int req_resource_num;
	};
	Resource(string RID, int resource_num, int free_unit_num);

	int request(string PID, int req_resource_num);
	string release(int release_resource_num);
	bool isSingleResource(); //�Ƿ��ǵ�һ��Դ
	bool isFull();			 //�Ƿ��п�����Դ

private:
	string RID; //��ԴRID
	int resource_num;
	int free_unit_num; //���е�Ԫ��

	vector<ProcessInfo> waiting_list; //���������б������̵�PID��
};