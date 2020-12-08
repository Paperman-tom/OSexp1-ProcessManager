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
	//资源初始化
	int resource_num = 4;
	ResourceController rc = ResourceController(4);

	//队列初始化
	Dispatcher os = Dispatcher();

	//进程管理初始化
	ProcessController pc = ProcessController();

	//创建Init 进程
	const Process::Status default_status = {Process::ready, Process::ReadyL};
	int process_counter = 1;

	Process Init_1("Init_1", Tools::toString(process_counter), default_status, Process::Init, "");
	//添加进程到进程表
	pc.insert_process(Init_1.get_PID(), Init_1);
	//添加进程到ready队列并开始执行
	os.insert_ready_list(Init_1.get_PID(), Init_1.get_priority()).change_ready_to_running(Init_1.get_priority());
	cout << "*Process init is running " << endl;
	//test shell 循环接受命令
	while (true)
	{
		cout << "shell >";

		//检测当前进程（新进程的父进程）
		string parent_process_id = os.get_running_process();
		//获取（父）进程的引用
		Process &parent_process = (*(pc.process_map.find(parent_process_id))).second;

		//获取命令
		string command;
		cin >> command;

		if (command == "cr")
		{ //创建新进程
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

			//根据父进程创建子进程，获取子进程的引用
			Process &child_process = parent_process.add_children_process(process_name,
																		 Tools::toString(process_counter),
																		 default_status,
																		 process_priority);
			//添加子进程到进程表
			pc.insert_process(child_process.get_PID(), child_process);
			//添加子进程到ready队列
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
				// string running_process_id = os.get_running_process();
				// Process &running_process = (*(pc.process_map.find(running_process_id))).second;
				// cout << "Process " << running_process.get_name() << " is running. ";
				// cout << "Process " << parent_process.get_name() << " is ready" << endl;
			}
			else
			{
				string running_process_id = os.get_running_process();
				Process &running_process = (*(pc.process_map.find(running_process_id))).second;
				cout << "Process " << running_process.get_name() << " is running. " << endl;
			}
		}
		else if (command == "to")
		{ //timeout
			pc.timeout(os);
		}
		else if (command == "req")
		{ //当前进程请求资源
			string resource_name;
			int req_resource_num;
			cin >> resource_name >> req_resource_num;
			string RID = Tools::trans_name_to_RID(resource_name);
			Resource &resource = rc.get_resource(RID);

			//请求资源
			int get_resource = resource.request(parent_process.get_PID(), req_resource_num);
			if (get_resource == 0)
			{ //获得资源，在进程PCB中记录
				cout << "Process " << parent_process.get_name() << " requests " << req_resource_num << " " << resource_name << endl;
				parent_process.get_resource(RID, req_resource_num);
			}
			else if (get_resource == 1)
			{ //未获得资源，在进程阻塞
				parent_process.change_status(Process::request);
				//进入阻塞队列
				os.change_running_to_list(parent_process.get_PID(), parent_process.get_priority(), false);
				//调度下一个ready进程进入running
				pc.scheduler(os);
				cout << "Process " << parent_process.get_name() << " is blocked" << endl;
			}
			else
			{ //获取资源出错
				cout << "Error in get resource!" << endl;
			}
		}
		else if (command == "rel")
		{ //当前进程释放资源
			string resource_name;
			int req_resource_num;
			cin >> resource_name >> req_resource_num;
			string RID = Tools::trans_name_to_RID(resource_name);
			Resource &resource = rc.get_resource(RID);
			//释放资源并检测下一个应进入ready队列的进程
			string next_process_id = resource.release(parent_process.relase_resource(RID));
			cout << "Process " << parent_process.get_name() << " releases " << req_resource_num << " " << resource_name << endl;
			if (next_process_id != "")
			{ //可满足下一进程
				Process &set_ready_process = (*(pc.process_map.find(next_process_id))).second;
				//进程从block队列进入ready队列
				os.out_block_list(set_ready_process.get_priority()).insert_ready_list(set_ready_process.get_PID(), set_ready_process.get_priority());
			}
		}
		else if (command == "de")
		{ //删除当前进程及子进程
			string process_name;
			cin >> process_name;
			string process_id = Tools::trans_name_to_PID(process_name, pc);
			Process &process = pc.get_process(process_id);
			//释放资源
			string set_ready_process_list[4] = {"", "", "", ""};
			process.relase_all_resource(rc, set_ready_process_list);
			for (int i = 0; i < set_ready_process_list->size(); i++)
			{ //遍历返回的可调度至ready的进程名列表
				if (set_ready_process_list[i] != "")
				{
					Process &set_ready_process = (*(pc.process_map.find(set_ready_process_list[i]))).second;
					//进程从block队列进入ready队列
					os.out_block_list(set_ready_process.get_priority()).insert_ready_list(set_ready_process.get_PID(), set_ready_process.get_priority());
					set_ready_process.change_status(Process::release);
					cout << "wake up process " << set_ready_process.get_name() << " ." << endl;
				}
			}
			//从进程表删除子进程
			auto &children = process.get_children();
			auto children_it = children.begin();
			while (children_it != children.end())
			{
				pc.destroy_process(children_it->second.get_PID());
				++children_it;
			}
			//从进程表删除进程
			if (parent_process.get_PID() == process.get_PID())
			{
				pc.timeout(os);
			}
			pc.destroy_process(process.get_PID());
		}
		else if (command == "list")
		{
			string list_name;
			cin >> list_name;
			if (list_name == "ready")
			{
				cout << "Sys(2):";
				auto r2 = os.ready_list[2].begin();
				while (r2 != os.ready_list[2].end())
				{
					if (*r2 == "")
					{
						++r2;
						continue;
					}
					Process &nextP = (*(pc.process_map.find(*r2))).second;
					cout << nextP.get_name() << "-";
					++r2;
				}
				cout << "\b" << endl;
				cout << "User(1):";
				auto r1 = os.ready_list[1].begin();
				while (r1 != os.ready_list[1].end())
				{
					if (*r1 == "")
					{
						++r1;
						continue;
					}
					Process &nextP = (*(pc.process_map.find(*r1))).second;
					cout << nextP.get_name() << "-";
					++r1;
				}
				cout << "\b" << endl;
				cout << "Init(0):";
				auto r0 = os.ready_list[0].begin();
				while (r0 != os.ready_list[0].end())
				{
					if (*r0 == "")
					{
						++r0;
						continue;
					}
					Process &nextP = (*(pc.process_map.find(*r0))).second;
					cout << nextP.get_name() << "-";
					++r0;
				}
				cout << "\b" << endl;
			}
			else if (list_name == "res")
			{
				for (int i = 1; i <= 4; i++)
				{
					cout << "R" << i << " ";
					Resource &res = rc.get_resource(to_string(i));
					cout << res.get_free_unit_num() << endl;
				}
			}
			else if (list_name == "block")
			{
				for (int i = 1; i <= 4; i++)
				{
					cout << "R" << i << " ";
					Resource &res = rc.get_resource(to_string(i));
					auto res_waiting_list = res.get_waiting_list();
					auto p_waiting = res_waiting_list.begin();
					while (p_waiting != res_waiting_list.end())
					{
						Process &nextP = (*(pc.process_map.find(p_waiting->PID))).second;
						cout << " " << nextP.get_name() << "(" << p_waiting->req_resource_num << ")";
						++p_waiting;
					}
					cout << endl;
				}
			}
			else
			{
				auto it = pc.process_map.begin();
				while (it != pc.process_map.end())
				{
					cout << endl
						 << it->second.get_name() << "::" << it->second.get_PID() << endl;
					++it;
				}
			}
		}
		else if (command == "exit")
		{
			break;
		}
	}

	cout << "END" << endl;
	return 0;
}