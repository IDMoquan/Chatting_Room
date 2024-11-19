#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib")

typedef struct {
	SOCKET socket;
	char *client_ip;
}Data;

DWORD WINAPI thread_func(LPVOID lpThreadParameter) {
	//拆开结构体包装
	Data* data = (Data *)lpThreadParameter;
	SOCKET client_socket = data->socket;	//取出socket
	char *client_ip = data->client_ip;		//取出client_ip
	free(lpThreadParameter);				//释放内存
	while (true) {
		//puts("555");
		char buffer[1024] = { 0 };
		int ret = recv(client_socket, buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		printf("%s: %s\n", client_ip, buffer);

		send(client_socket, buffer, (int)strlen(buffer), 0);
	}
	printf("%s已断开！\n", client_ip);
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

	while (1) {
		SOCKET client_socket = accept(listen_socket, NULL, NULL);  //阻塞
		if (client_socket == INVALID_SOCKET) {
			continue;		
		}
		
		char client_ip[256];

		recv(client_socket, client_ip, 256, 0);
		printf("%s已连接！\n", client_ip);

		//开辟内存创建Data类型指针并赋值(CreateThread只能传一个参数，包装多参数至结构体)
		Data* data = (Data*)calloc(1, sizeof(Data));
		if (data != NULL) {
			data->client_ip = client_ip;
			data->socket = client_socket;

			//创建线程
			CreateThread(NULL, 0, thread_func, (LPVOID)data, 0, NULL);
		}
	}

	WSACleanup();

	return 0;
}