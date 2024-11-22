#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<winsock2.h>
#include<iostream>
#include<vector>
#include<queue>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

vector<pair<string, SOCKET>>ips;
queue<string>messages;		//发送信息缓冲
//int online_poeple = 0;	//当前在线人数
//int message_number = 0;	//缓冲区存有消息数量
bool status1 = true;		//是否有人正在连接
bool status2 = true;		//是否在发送信息

typedef struct {
	SOCKET socket;
	char *client_ip;
}Data;

//发送线程
DWORD WINAPI Send(LPVOID lpThreadParameter) {
	while (1) {
		if (!messages.empty() && status1) {
			string temp = messages.front();
			messages.pop();
			cout << temp << endl;
			//for (auto ip : ips) {
			//	while (!status2);
			//	//send(ip.second, temp.c_str(), sizeof(temp), 0);
			//}
			
		}
	}
	return 0;
}

//接受线程
DWORD WINAPI Receive(LPVOID lpThreadParameter) {
	//拆开结构体包装
	Data* data = (Data *)lpThreadParameter;
	SOCKET client_socket = data->socket;	//取出socket
	char *client_ip = data->client_ip;		//取出client_ip
	free(lpThreadParameter);				//释放内存
	ips.push_back(make_pair(client_ip, client_socket));
	status1 = true;
	while (true) {
		//puts("555");
		char buffer[1024] = { 0 };
		//接收消息
		int ret = recv(client_socket, buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		//message_number++;
		messages.push(buffer);
		cout << client_ip << ":" << messages.back();

		//send(client_socket, buffer, (int)strlen(buffer), 0);
	}
	printf("%s已断开！\n", client_ip);
	//online_poeple--;
	closesocket(client_socket);
	return 0;
}

int main() {
	//启动服务
	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("WSA服务启动失败！！！错误代码：%d\n", GetLastError());
		return -1;
	}

	puts("WSA服务启动成功！");
	
	//创建socket
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) {
		printf("创建listen_socket失败！！！错误代码：%d\n", GetLastError());
		return -1;
	}
	
	puts("创建listen_socket成功！");

	//设置属性
	struct sockaddr_in local = { 0 };
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);
	local.sin_addr.s_addr = inet_addr("0.0.0.0");
	//绑定端口
	if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == -1) {
		printf("绑定Socket失败！！！错误代码：%d\n", GetLastError());
		return -1;
	}
	puts("绑定Socket成功！");
	
	//启动监听
	if (listen(listen_socket, 10) == -1) {
		printf("启动listen_socket监听失败！！！错误代码：%d\n", GetLastError());
		return -1;
	}

	puts("启动listen_socket监听成功！");
	cout << "服务器IP：" << getlocalip() << endl;
	CreateThread(NULL, 0, Send, NULL, 0, NULL);

	while (1) {
		SOCKET client_socket = accept(listen_socket, NULL, NULL);  //阻塞
		if (client_socket == INVALID_SOCKET) {
			continue;		
		}
		
		status1 = false;
		char client_ip[256];

		recv(client_socket, client_ip, 256, 0);
		printf("%s已连接！\n", client_ip);
		//online_poeple++;

		//开辟内存创建Data类型指针并赋值(CreateThread只能传一个参数，包装多参数至结构体)
		Data* data = (Data*)calloc(1, sizeof(Data));
		if (data != NULL) {
			data->client_ip = client_ip;
			data->socket = client_socket;

			//创建线程
			CreateThread(NULL, 0, Receive, (LPVOID)data, 0, NULL);
		}
	}

	WSACleanup();

	return 0;
}