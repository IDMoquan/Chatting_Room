#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include<winsock2.h>
#include<string>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

char server_ip[256] = { 0 };


//���������߳�
DWORD WINAPI receive(LPVOID lpThreadParameter) {
	SOCKET server_socket = *(SOCKET*)lpThreadParameter;
	char buffer[1024] = { 0 };
	while (1) {
		//puts("waiting");
		int ret = recv(server_socket, buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		cout << buffer;
	}
	return 0;
}

int main() {
	//��������
	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("WSA��������ʧ�ܣ�����������룺%d\n", GetLastError());
		return -1;
	}
	puts("WSA���������ɹ�");

	//��ȡ����ip��ת��Ϊchar*����
	string localip = getlocalip();
	const char* cc_localip = localip.c_str();
	char* c_localip = const_cast<char*>(cc_localip);

	//����Socket
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		printf("����client_socketʧ�ܣ�����������룺%d\n", GetLastError());
		return -1; 
	}
	puts("����client_socket�ɹ�");


	for (int i = 1; i <= 5; i++) {
		printf("������IPV4��ַ��");
		scanf("%s", server_ip);
		//��������
		struct sockaddr_in target;
		target.sin_family = AF_INET;
		target.sin_port = htons(8080);
		target.sin_addr.s_addr = inet_addr(server_ip);
		//���ӷ�����
		if (connect(client_socket, (struct sockaddr*)&target, sizeof(target)) == -1) {
			printf("���ӷ�����ʧ�ܣ�����������룺%d\n", GetLastError());
			continue;
		}
		puts("���ӷ������ɹ�");
		break;
	}
	
	//����ip��ַ
	send(client_socket, cc_localip, sizeof(localip), 0);
	Sleep(1000);
	system("cls");
	printf("��������������:%s\n", server_ip);
	CreateThread(NULL, 0, receive, (LPVOID *)&client_socket, 0, NULL);

	getchar();	//������뻺�����س�
	//������Ϣ
	while (1) {
		char s_buffer[1024] = { 0 };
		//printf("�����룺");
		fgets(s_buffer, sizeof(s_buffer), stdin);
		//puts("����ɹ�");
	/*	strcat(c_localip, ":");
		strcat(c_localip, s_buffer);*/
		send(client_socket, s_buffer, (int)strlen(s_buffer), 0);

		/*char r_buffer[1024] = { 0 };
		int ret = recv(client_socket, r_buffer, 1024, 0);
		if (ret <= 0) {
			break;
		}
		printf("%s\n", r_buffer);*/
	}

	closesocket(client_socket);

	return 0;
}