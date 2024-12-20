﻿#include "Regist.h"
#include<WinSock2.h>
#include<cwchar>
#include<cstring>
#include<qregularexpression.h>
#include<qregularexpressionvalidator>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket;
QString qusername, qpassword;
std::string s_username, s_password;
const char* username, * password;
char back_info[256];
const int username_length = 1024;
const int password_length = 1024;
const int message_length = 1024;
//extern std::string Utf8ToGbk(const std::string& utf8Str);

Regist::Regist(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.lineEdit_2->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_2->setEchoMode(QLineEdit::Password);
	this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");//渐变色
}

Regist::~Regist()
{}

void Regist::closeEvent(QCloseEvent* event) {
	//const char* ext = "exit";
	const char* status = "login";
	send(client_socket, status, 256, 0);
	//send(client_socket, status, 256, 0);
	event->accept();
}

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
	send(client_socket, username, username_length, 0);
	send(client_socket, password, password_length, 0);
	recv(client_socket, back_info, 256, 0);
	//int wideLength = MultiByteToWideChar(CP_UTF8, 0, back_info, -1, NULL, 0);
	//wchar_t* wideString = new wchar_t[wideLength];
	//MultiByteToWideChar(CP_UTF8, 0, back_info, -1, wideString, wideLength);
	//LPCWSTR lpWideString = wideString;
	//MessageBox(NULL, lpWideString, L"Error!", MB_OK);
	//delete[] wideString;
	if (!strcmp("accept", back_info)) {
		this->close();
	}
	else {
		MessageBox(NULL, L"用户已被注册！", L"Error!", MB_OK);
	}
}
