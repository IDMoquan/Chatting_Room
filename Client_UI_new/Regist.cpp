#include "Regist.h"
#include "variables.h"
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket;

Regist::Regist(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

Regist::~Regist()
{}

void Regist::regist_confirm() {
	const char *username = ui.lineEdit->text().toStdString().c_str();
	const char* password = ui.lineEdit_2->text().toStdString().c_str();
	send(client_socket, username, sizeof(username), 0);
	send(client_socket, password, sizeof(password), 0);
}
