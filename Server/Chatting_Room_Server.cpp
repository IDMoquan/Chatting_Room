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

//int online_poeple = 0;		//��ǰ��������
//int message_number = 0;		//������������Ϣ����
bool status1 = true;		//�Ƿ�������������
bool status2 = true;		//�Ƿ��ڷ�����Ϣ

typedef struct {
	SOCKET socket;
	char *client_ip;
}Data;

vector<Data>clients;
queue<string>messages;		//������Ϣ����


//�����߳�
DWORD WINAPI Send(LPVOID lpThreadParameter) {
	while (1) {
		if (!messages.empty()) {
			string temp = messages.front();
			messages.pop();
			//puts("666");
			cout << temp;
			for (auto clt : clients) {
				//while (!status2);
				send(clt.socket, temp.c_str(), sizeof(temp), 0);
				//cout << "send" << endl;
			}
			
		}
	}
	return 0;
}

//�����߳�
DWORD WINAPI Receive(LPVOID lpThreadParameter) {
	//�𿪽ṹ���װ
	Data* data = (Data *)lpThreadParameter;
	SOCKET client_socket = data->socket;	//ȡ��socket
	char *client_ip = data->client_ip;		//ȡ��client_ip
	clients.push_back(*data);
	free(lpThreadParameter);				//�ͷ��ڴ�
	status1 = true;
	while (true) {
		//puts("555");
		char buffer[1024] = { 0 };
		//������Ϣ
		int ret = recv(client_socket, buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		//message_number++;
		messages.push(buffer);
		//puts("123");
		//cout << client_ip << ":" << buffer;

		//send(client_socket, buffer, (int)strlen(buffer), 0);
	}
	printf("%s�ѶϿ���\n", client_ip);
	//online_poeple--;
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
	CreateThread(NULL, 0, Send, NULL, 0, NULL);

	while (1) {
		SOCKET client_socket = accept(listen_socket, NULL, NULL);  //����
		if (client_socket == INVALID_SOCKET) {
			continue;		
		}
		
		status1 = false;
		char client_ip[256];

		recv(client_socket, client_ip, 256, 0);
		printf("%s�����ӣ�\n", client_ip);
		//online_poeple++;

		//�����ڴ洴��Data����ָ�벢��ֵ(CreateThreadֻ�ܴ�һ����������װ��������ṹ��)
		Data* data = (Data*)calloc(1, sizeof(Data));
		if (data != NULL) {
			data->client_ip = client_ip;
			data->socket = client_socket;

			//�����߳�
			CreateThread(NULL, 0, Receive, (LPVOID)data, 0, NULL);
		}
	}

	WSACleanup();

	return 0;
}