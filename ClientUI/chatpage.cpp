#define _CRT_SECURE_NO_WARNINGS
#include "chatpage.h"
#include "Functions.h"
#include "Login.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <qlistwidget.h>
#include <qstringlistmodel.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket, client_socket_c;	//信息socket、用户socket
//extern std::string Utf8ToGbk(const std::string& utf8Str);	//UTF转GBK
extern int charToint(char* str);	//char转int
extern bool connect_status;			//连接状态
extern char e_server_ip[256];			//客户端ip
extern int client_count;
extern char c_username[1024];
extern const int username_length = 1024;
extern const int password_length = 1024;
extern const int message_length = 1024;
QStringList list, c_list;

struct Q2type {
	QListWidget* message_list;
	QLabel* client_count;
};

//接收在线用户信息线程
DWORD WINAPI Receive_clients(LPVOID lpThreadParameter) {
	//if (lpThreadParameter == NULL) {
	//	MessageBox(NULL, L"服务器断开连接！", NULL, MB_OK);
	//	return -1;
	//}
	struct Q2type data = *(struct Q2type *)lpThreadParameter;
	QListWidget* message_list = data.message_list;
	QLabel *label_count = data.client_count;
	message_list->setSpacing(5);
	message_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	while (connect_status);
	while (1) {
		char c_client_count[256] = { 0 };
		recv(client_socket_c, c_client_count, 256, 0);
		c_list.clear();
		message_list->clear();
		client_count = charToint(c_client_count);
		label_count->setText(c_client_count);
		char info[username_length] = { 0 };
		for (int i = 0; i < client_count; i++) {
			recv(client_socket_c, info, username_length, 0);
			c_list.append(info);
		}
		QStringListModel* c_listmodel = new QStringListModel(c_list);
		message_list->addItems(c_list);
	}
}

//接收服务器分发消息线程
DWORD WINAPI Receive_message(LPVOID lpThreadParameter) {
	QListWidget* message_list = (QListWidget*)lpThreadParameter;
	message_list->setSpacing(5);
	message_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	while (connect_status);
	while (1) {
		char buffer[message_length];
		//Sleep(100);
		int ret = recv(client_socket, buffer, message_length, 0);
		list.clear();
		//Sleep(100);
		if (ret <= 0) {
			MessageBox(NULL, L"服务器断开连接！", NULL, MB_OK);
			return -1;
		}
		list << buffer;
		message_list->addItems(list);
		message_list->scrollToBottom();
	}
	return 0;
}

chatpage::chatpage(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//connect(ui.lineEdit, SIGNAL("returnPressed()"), this, SLOT(sendinfor()));
	//创建接收线程
	struct Q2type* type = (struct Q2type*)calloc(1, sizeof(struct Q2type));
	if (type != NULL) {
		type->message_list = ui.listWidget_2;
		type->client_count = ui.label_count;
		CreateThread(NULL, 0, Receive_message, (LPVOID)ui.listWidget, 0, NULL);
		CreateThread(NULL, 0, Receive_clients, (LPVOID)type, 0, NULL);
	}
	else {
		MessageBox(NULL, L"空指针错误", NULL, MB_OK);
	}
}

chatpage::~chatpage()
{}

//发送信息
void chatpage::sendinfor() {
	QString qmessage = ui.lineEdit->text();
	std::string s_message = qmessage.toStdString();
	//判断非空
	if (s_message.empty()) {
		//MessageBox(NULL, L"空", NULL, MB_OK);
	}
	else {
		const char* messsage = s_message.c_str();
		//qmessage = c_username + ':' + qmessage;
		QListWidgetItem* item;
		list.clear();
		char fin_username[username_length + message_length + 1];
		sprintf(fin_username, "%s:%s", c_username, messsage);
		list << fin_username;
		item = new QListWidgetItem(fin_username, ui.listWidget);
		item->setTextAlignment(Qt::AlignRight);
		ui.listWidget->show();
		ui.listWidget->scrollToBottom();
		::send(client_socket, messsage, message_length, 0);
		cleanup();
	}
}

//清空发送栏
void chatpage::cleanup()
{
	ui.lineEdit->clear();
}
