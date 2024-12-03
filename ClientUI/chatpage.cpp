#include "chatpage.h"
#include "Functions.h"
#include "Login.h"
#include<WinSock2.h>
#include<string>
#include<Windows.h>
#include<qlistwidget.h>
#include<qstringlistmodel.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket, client_socket_c;	//信息socket、用户socket
extern std::string Utf8ToGbk(const std::string& utf8Str);	//UTF转GBK
extern int charToint(char* str);	//char转int
extern bool connect_status;			//连接状态
extern char e_server_ip[256];			//客户端ip
extern int client_count;
extern std::string s_username, s_password;
QStringList list, c_list;

extern std::string Utf8ToGbk(const std::string& utf8Str);

//接收在线用户信息线程
DWORD WINAPI Receive_clients(LPVOID lpThreadParameter) {
	QListWidget* message_list = (QListWidget*)lpThreadParameter;
	message_list->setSpacing(5);
	message_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	while (connect_status);
	while (1) {
		char c_client_count[256] = { 0 };
		recv(client_socket_c, c_client_count, 256, 0);
		c_list.clear();
		message_list->clear();
		client_count = charToint(c_client_count);
		char info[256] = { 0 };
		for (int i = 0; i < client_count; i++) {
			recv(client_socket_c, info, 256, 0);
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
		char buffer[1024] = { 0 };
		int ret = recv(client_socket, buffer, 1024, 0);
		if (ret <= 0) {
			MessageBox(NULL, L"服务器断开连接！", NULL, MB_OK);
			return -1;
		}
		list << buffer;
		message_list->addItem(buffer);
	}
	return 0;
}

chatpage::chatpage(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//创建接收线程
	CreateThread(NULL, 0, Receive_message, (LPVOID)ui.listWidget, 0, NULL);
	CreateThread(NULL, 0, Receive_clients, (LPVOID)ui.listWidget_2, 0, NULL);
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
		qmessage = ;
		list << qmessage;
		ui.listWidget->addItem(qmessage);
		::send(client_socket, messsage, 1024, 0);
		cleanup();
	}
}

//清空发送栏
void chatpage::cleanup()
{
	ui.lineEdit->clear();
}
