#pragma once
#include <map>
#include "ResourceController.h"
using namespace std;

class Process
{
public:
	enum ProcessType
	{
		none, ready, running, blocked
	};
	enum ListType
	{
		ReadyL, BlockedL
	};
	enum PriorityType
	{
		Init, User, System
	};
	enum ProcessOperate
	{
		create, destroy, request, release, timeout, dispatch
	};
	struct Status
	{
		ProcessType process_type;
		ListType list_type;
	};
	Process(string name, string PID, Status status, PriorityType priority, string parent_process);
	Process();
	PriorityType get_priority();
	string get_PID();
	string get_name();
	void set_process_status(ProcessType process_type);
	void set_list_status(ListType list_type);

	Process &add_children_process(string name, string PID, Status status, PriorityType priority); //����ӽ���
	void set_parent_process(string PID);														  //���ø�����

	void get_resource(string RID, int resource_num); //��ȡ��Դ
	int relase_resource(string RID);				 //�ͷ���Դ
	//string* relase_all_resource(ResourceController resource_controller);//�ͷ���Դ
	void relase_all_resource(ResourceController resource_controller, string *set_ready_process); //�ͷ���Դ
	void change_status(ProcessOperate operater);

	map<string, Process> &get_children();

private:
	string PID; //����PID
	string name;

	//Not used part
	string cpu_state = "";
	string memory = "";
	string open_files = "";
	//Not used part END

	map<string, int> other_resource; //��Դ�б�
	Status status;					 //����״̬
	PriorityType priority;			 //�������ȼ�
	string parent_process;			 //������PID
	map<string, Process> children;	 //�ӽ����б�
};