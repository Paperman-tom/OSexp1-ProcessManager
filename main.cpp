#include "Tools.h"
#include "Resource.h"
#include "ResourceController.h"
#include "Process.h"
#include "ProcessController.h"
#include "Dispatcher.h"
#include <iostream>
#include <fstream>

static string trans_name_to_PID(string process_name, ProcessController pc)
{
	map<string, Process> &process_map = pc.process_map;
	auto process_map_it = process_map.begin();
	while (process_map_it != process_map.end())
	{
		if (process_map_it->second.get_name() == process_name)
		{
			return process_map_it->second.get_PID();
		}
		++process_map_it;
	}
}
int main()
{
	//��Դ��ʼ��
	int resource_num = 4;
	ResourceController rc = ResourceController(4);

	//���г�ʼ��
	Dispatcher os = Dispatcher();

	//���̹����ʼ��
	ProcessController pc = ProcessController();

	//����Init ����
	const Process::Status default_status = {Process::ready, Process::ReadyL};
	int process_counter = 1;

	Process Init_1("Init_1", Tools::toString(process_counter), default_status, Process::Init, "");
	//��ӽ��̵����̱�
	pc.insert_process(Init_1.get_PID(), Init_1);
	//��ӽ��̵�ready���в���ʼִ��
	os.insert_ready_list(Init_1.get_PID(), Init_1.get_priority()).change_ready_to_running(Init_1.get_priority());
	cout << "��Process init is running " << endl;
	//test shell ѭ����������
	while (true)
	{
		cout << "shell >";

		//��⵱ǰ���̣��½��̵ĸ����̣�
		string parent_process_id = os.get_running_process();
		//��ȡ���������̵�����
		Process &parent_process = (*(pc.process_map.find(parent_process_id))).second;

		//��ȡ����
		string command;
		cin >> command;

		if (command == "cr")
		{ //�����½���
			string process_name;
			int process_priority_val;
			cin >> process_name >> process_priority_val;
			Process::PriorityType process_priority;
			switch (process_priority_val)
			{
			case 0:
				process_priority = Process::Init;
				break;
			case 1:
				process_priority = Process::User;
				break;
			case 2:
				process_priority = Process::System;
				break;
			default:
				process_priority = Process::Init;
				break;
			}
			process_counter++;

			//���ݸ����̴����ӽ��̣���ȡ�ӽ��̵�����
			Process &child_process = parent_process.add_children_process(process_name,
																		 Tools::toString(process_counter),
																		 default_status,
																		 process_priority);
			//����ӽ��̵����̱�
			pc.insert_process(child_process.get_PID(), child_process);
			//����ӽ��̵�ready����
			os.insert_ready_list(child_process.get_PID(), child_process.get_priority());

			//cout<< "new process name:" << child_process.get_name()
			//		<< "   "
			//		<<"new process id:" << child_process.get_PID()
			//		<<"   "
			//		<<"new process priority:"<< child_process.get_priority ()
			//		<< endl;
			if (parent_process.get_priority() < child_process.get_priority())
			{
				pc.timeout(os);
			}
		}
		else if (command == "to")
		{ //timeout
			pc.timeout(os);
		}
		else if (command == "req")
		{ //��ǰ����������Դ
			string resource_name;
			int req_resource_num;
			cin >> resource_name >> req_resource_num;
			string RID = Tools::trans_name_to_RID(resource_name);
			Resource &resource = rc.get_resource(RID);

			//������Դ
			int get_resource = resource.request(parent_process.get_PID(), req_resource_num);
			if (get_resource == 0)
			{ //�����Դ���ڽ���PCB�м�¼
				//cout << "get res " << RID << ":" << req_resource_num << endl;
				parent_process.get_resource(RID, req_resource_num);
			}
			else if (get_resource == 1)
			{ //δ�����Դ���ڽ�������
				parent_process.change_status(Process::request);
				//������������
				os.change_running_to_list(parent_process.get_PID(), parent_process.get_priority(), false);
				//������һ��ready���̽���running
				pc.scheduler(os);
			}
			else
			{ //��ȡ��Դ����
				cout << "Error in get resource!" << endl;
			}
		}
		else if (command == "rel")
		{ //��ǰ�����ͷ���Դ
			string resource_name;
			int req_resource_num;
			cin >> resource_name >> req_resource_num;
			string RID = Tools::trans_name_to_RID(resource_name);
			Resource &resource = rc.get_resource(RID);
			//�ͷ���Դ�������һ��Ӧ����ready���еĽ���
			string next_process_id = resource.release(parent_process.relase_resource(RID));
			if (next_process_id != "")
			{ //��������һ����
				Process &set_ready_process = (*(pc.process_map.find(next_process_id))).second;
				//���̴�block���н���ready����
				os.out_block_list(set_ready_process.get_priority()).insert_ready_list(set_ready_process.get_PID(), set_ready_process.get_priority());
			}
		}
		else if (command == "de")
		{ //ɾ����ǰ���̼��ӽ���
			string process_name;
			cin >> process_name;
			string process_id = Tools::trans_name_to_PID(process_name, pc);
			Process &process = pc.get_process(process_id);
			//�ͷ���Դ
			string set_ready_process_list[4] = {"", "", "", ""};
			process.relase_all_resource(rc, set_ready_process_list);
			for (int i = 0; i < set_ready_process_list->size(); i++)
			{ //�������صĿɵ�����ready�Ľ������б�
				if (set_ready_process_list[i] != "")
				{
					Process &set_ready_process = (*(pc.process_map.find(set_ready_process_list[i]))).second;
					//���̴�block���н���ready����
					os.out_block_list(set_ready_process.get_priority()).insert_ready_list(set_ready_process.get_PID(), set_ready_process.get_priority());
					set_ready_process.change_status(Process::release);
				}
			}
			//�ӽ��̱�ɾ���ӽ���
			auto &children = process.get_children();
			auto children_it = children.begin();
			while (children_it != children.end())
			{
				pc.destroy_process(children_it->second.get_PID());
				++children_it;
			}
			//�ӽ��̱�ɾ������
			if (parent_process.get_PID() == process.get_PID())
			{
				pc.timeout(os);
			}
			pc.destroy_process(process.get_PID());
		}
		else if (command == "list")
		{
			auto it = pc.process_map.begin();
			while (it != pc.process_map.end())
			{
				cout << endl
					 << it->second.get_name() << "::" << it->second.get_PID() << endl;
				++it;
			}
		}
		else if (command == "exit")
		{
			break;
		}
		cout << parent_process.get_name() << endl;
	}

	cout << "END" << endl;
	return 0;
}