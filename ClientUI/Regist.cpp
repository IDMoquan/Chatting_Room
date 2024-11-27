#include "Regist.h"
#include "variables.h"
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket;
QString qusername, qpassword;
std::string s_username, s_password;
const char* username, * password;

Regist::Regist(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

Regist::~Regist()
{}

void Regist::regist_confirm() {
	//分布转化，不然会指向已销毁实例
	qusername = ui.lineEdit->text();
	qpassword = ui.lineEdit_2->text();
	//std::wstring ws_username = qusername.toStdWString();
	//LPCWSTR M_username = ws_username.c_str();
	//MessageBox(NULL, M_username, L"Error!", MB_OK);
	s_username = qusername.toStdString();
	s_password = qpassword.toStdString();
	username = s_username.c_str();
	password = s_password.c_str();
	//向服务器发送账号密码
	send(client_socket, username, 256, 0);
	send(client_socket, password, 256, 0);
}
