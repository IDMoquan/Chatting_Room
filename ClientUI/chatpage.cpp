#include "chatpage.h"
#include "Functions.h"
#include "Login.h"
#include<WinSock2.h>
#include<string>
#include<Windows.h>
#include<qlistview.h>
#include<qstringlistmodel.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket;
extern std::string Utf8ToGbk(const std::string& utf8Str);
extern bool connect_status;
QStringList list;

extern std::string Utf8ToGbk(const std::string& utf8Str);



//接收服务器分发消息
DWORD WINAPI Receive_message(LPVOID lpThreadParameter) {
	QListView* message_list = (QListView*)lpThreadParameter;
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
		list.append(buffer);
		QStringListModel* listmodel = new QStringListModel(list);
		message_list->setModel(listmodel);
	}
	return 0;
}

chatpage::chatpage(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//创建接收线程
	CreateThread(NULL, 0, Receive_message, (LPVOID)ui.listView, 0, NULL);
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
		::send(client_socket, Utf8ToGbk(messsage).c_str(), 1024, 0);
		cleanup();
	}
}

//清空发送栏
void chatpage::cleanup()
{
	ui.lineEdit->clear();
}
