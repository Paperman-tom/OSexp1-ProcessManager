## 操作系统实验1 进程和资源管理
---
### 项目结构
│  Dispatcher.cpp   //分派器类实现，管理进程队列  
│  Dispatcher.h     //分派器类头文件  
│  example.jpg      //示例输入输出  
│  main.cpp         //主程序，实现shell操作  
│  main.exe         //编译好的主程序  
│  Process.cpp      //进程类实现  
│  Process.h        //进程类头文件  
│  ProcessController.cpp        //PCB类实现  
│  ProcessController.h          //PCB类头文件  
│  README.md            //readme文件  
│  Resource.cpp     //资源类实现  
│  Resource.h       //资源类头文件  
│  ResourceController.cpp       //资源管理类实现  
│  ResourceController.h         //资源管理类头文件  
│  Tools.h      //实现其他用到的函数  

### Usage
> main.exe

> commands are as follows:  
> `cr <pname> <Priority>` 创建进程 进程名 优先级(0:init, 1:user, 2:system)  
> `to` 模拟时钟中断  
> `list ready` 查看各优先级下ready队列  
> `list block` 查看在各资源等待资源分派的阻塞进程队列  
> `list res` 查看各资源剩余情况  
> `list all` 查看所有进程列表  
> `req <resource_name> <requst_num>` 请求资源 资源名 请求数量  
> `rel <resource_name> <requst_num>` 释放资源 资源名 释放数量  
> `exit` 退出shell  

### 相关方法
> 1)	通过定义C++ enum枚举常量和struct结构实现进程状态定义  
> 2)	C++ map容器和vector容器实现不同的进程列表和资源列表  
> 3)	C++ list容器实现ready和block队列管理  
> 4)	使用基于优先级的抢占式调度策略，在同一优先级内使用时间片轮转（RR）  
