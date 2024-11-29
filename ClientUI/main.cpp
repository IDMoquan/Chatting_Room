#include <stdio.h>
#include "Connect_Server.h"
#include "Login.h"
#include <QtWidgets/QApplication>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
int main(int argc, char* argv[])
{
	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) == -1) {
		MessageBox(NULL, L"WSA启动失败！", NULL, MB_OK);
		return -1;
	}
	QApplication a(argc, argv);
	Connect_Server w;
	w.show();
	return a.exec();
}