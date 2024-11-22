#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<winsock2.h>
#include<iostream>
#include<vector>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

vector<string>ips;		//�洢��������������ip
vector<string>messages;	//������Ϣ����
int online_poeple = 0;		//��ǰ��������
int message_number = 0;

typedef struct {
	SOCKET socket;
	char *client_ip;
}Data;

//DWORD WINAPI Send(LPVOID lpThreadParameter) {
//	while (1) {
//		if (!messages.empty()) {
//
//		}
//	}
//	return 0;
//}

DWORD WINAPI thread_func(LPVOID lpThreadParameter) {
	//�𿪽ṹ���װ
	Data* data = (Data *)lpThreadParameter;
	SOCKET client_socket = data->socket;	//ȡ��socket
	char *client_ip = data->client_ip;		//ȡ��client_ip
	free(lpThreadParameter);				//�ͷ��ڴ�
	while (true) {
		//puts("555");
		char buffer[1024] = { 0 };
		//������Ϣ
		int ret = recv(client_socket, buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		message_number++;
		messages.push_back(buffer);
		cout << client_ip << ":" << messages.back();

		//send(client_socket, buffer, (int)strlen(buffer), 0);
	}
	printf("%s�ѶϿ���\n", client_ip);
	online_poeple--;
	closesocket(client_socket);
	return 0;
}

int main() {
	//��������
	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("WSA��������ʧ�ܣ�����������룺%d\n", GetLastError());
		return -1;
	}

	puts("WSA���������ɹ���");
	
	//����socket
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) {
		printf("����listen_socketʧ�ܣ�����������룺%d\n", GetLastError());
		return -1;
	}
	
	puts("����listen_socket�ɹ���");

	//��������
	struct sockaddr_in local = { 0 };
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);
	local.sin_addr.s_addr = inet_addr("0.0.0.0");
	//�󶨶˿�
	if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == -1) {
		printf("��Socketʧ�ܣ�����������룺%d\n", GetLastError());
		return -1;
	}
	puts("��Socket�ɹ���");
	
	//��������
	if (listen(listen_socket, 10) == -1) {
		printf("����listen_socket����ʧ�ܣ�����������룺%d\n", GetLastError());
		return -1;
	}

	puts("����listen_socket�����ɹ���");
	cout << "������IP��" << getlocalip() << endl;

	while (1) {
		SOCKET client_socket = accept(listen_socket, NULL, NULL);  //����
		if (client_socket == INVALID_SOCKET) {
			continue;		
		}
		
		char client_ip[256];

		recv(client_socket, client_ip, 256, 0);
		printf("%s�����ӣ�\n", client_ip);
		online_poeple++;

		//�����ڴ洴��Data����ָ�벢��ֵ(CreateThreadֻ�ܴ�һ����������װ��������ṹ��)
		Data* data = (Data*)calloc(1, sizeof(Data));
		if (data != NULL) {
			data->client_ip = client_ip;
			data->socket = client_socket;

			//�����߳�
			CreateThread(NULL, 0, thread_func, (LPVOID)data, 0, NULL);
		}
	}

	WSACleanup();

	return 0;
}