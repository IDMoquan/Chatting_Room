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
	WSAStartup(MAKEWORD(2, 2), &wsaDATA);
	QApplication a(argc, argv);
	Connect_Server w;
	w.show();
	return a.exec();
}