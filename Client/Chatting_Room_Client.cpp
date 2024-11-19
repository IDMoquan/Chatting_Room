#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include<winsock2.h>
#include<string>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
	//启动服务
	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("WSA服务启动失败！！！错误代码：%d\n", GetLastError());
		return -1;
	}
	puts("WSA服务启动成功");

	//获取本机ip并转化为char*类型
	string localip = getlocalip();
	const char* cc_localip = localip.c_str();
	char* c_localip = const_cast<char*>(cc_localip);

	//创建Socket
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		printf("创建client_socket失败！！！错误代码：%d\n", GetLastError());
		return -1; 
	}
	puts("创建client_socket成功");

	//设置属性
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("127.0.0.1");

	//连接服务器
	if (connect(client_socket, (struct sockaddr*)&target, sizeof(target)) == -1) {
		printf("连接服务器失败！！！错误代码：%d\n", GetLastError());
		closesocket(client_socket);
		return -1;
	}
	puts("连接服务器成功");
	
	//发送ip地址
	send(client_socket, cc_localip, sizeof(localip), 0);

	//发送信息
	while (1) {
		char s_buffer[1024] = { 0 };
		printf("请输入：");
		scanf("%s", s_buffer);
	/*	strcat(c_localip, ":");
		strcat(c_localip, s_buffer);*/
		send(client_socket, s_buffer, (int)strlen(s_buffer), 0);

		char r_buffer[1024] = { 0 };
		int ret = recv(client_socket, r_buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		printf("%s\n", r_buffer);
	}

	closesocket(client_socket);

	return 0;
}