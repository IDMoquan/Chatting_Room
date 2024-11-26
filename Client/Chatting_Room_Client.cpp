#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include<winsock2.h>
#include<string>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

char server_ip[256] = { 0 };	//存储服务器ip

//创建接受线程
DWORD WINAPI receive(LPVOID lpThreadParameter) {
	SOCKET server_socket = *(SOCKET*)lpThreadParameter;
	char buffer[1024] = { 0 };
	while (1) {
		//puts("waiting");
		int ret = recv(server_socket, buffer, 1024, 0);		//接收服务端分发的消息
		if (ret <= 0) {
			break;
		}
		cout << buffer;				//输出消息
	}
	return 0;
}

int main() {
	//启动服务
	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("WSA服务启动失败！！！错误代码：%d\n", GetLastError());
		return -1;
	}
	puts("WSA服务启动成功");

	//获取本机ip并转化为char*类型
	string localip = getlocalip();						//本机ip(char *)类型
	const char* cc_localip = localip.c_str();			//本机ip(const char *)类型
	char* c_localip = const_cast<char*>(cc_localip);	//本机ip(char *)类型

	//创建Socket
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		printf("创建client_socket失败！！！错误代码：%d\n", GetLastError());
		return -1; 
	}
	puts("创建client_socket成功");

	bool connect_seccess = false;
	//输入五次没成功就停止
	for (int i = 1; i <= 5; i++) {
		printf("请输入IPV4地址：");
		scanf("%s", server_ip);
		//设置属性
		struct sockaddr_in target;
		target.sin_family = AF_INET;
		target.sin_port = htons(8080);
		target.sin_addr.s_addr = inet_addr(server_ip);
		//连接服务器
		if (connect(client_socket, (struct sockaddr*)&target, sizeof(target)) == -1) {
			printf("连接服务器失败！！！错误代码：%d\n", GetLastError());
			continue;
		}
		puts("连接服务器成功");
		connect_seccess = true;
		break;
	}
	if (connect_seccess == false) {
		return -1;
	}

	//向服务器发送ip地址
	send(client_socket, cc_localip, sizeof(localip), 0);
	Sleep(1000);
	system("cls");	//清屏
	printf("已连接至服务器:%s\n", server_ip);
	CreateThread(NULL, 0, receive, (LPVOID *)&client_socket, 0, NULL);

	getchar();	//清空输入缓冲区回车
	//发送信息
	while (1) {
		char s_buffer[1024] = { 0 };
		fgets(s_buffer, sizeof(s_buffer), stdin);	//输入
		send(client_socket, s_buffer, (int)strlen(s_buffer), 0);
	}

	closesocket(client_socket);

	return 0;
}