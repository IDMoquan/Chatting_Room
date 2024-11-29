#include "chatpage.h"
#include "Functions.h"
#include "Login.h"
#include<WinSock2.h>
#include<string>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket;
extern std::string Utf8ToGbk(const std::string& utf8Str);

chatpage::chatpage(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//connect(ui.sendpushButton, SIGNAL(clicked()), this, SLOT(sendinfor()));
	//connect(ui.cancelpushButton, SIGNAL(clicked()), this, SLOT(cleanup()));
}

chatpage::~chatpage()
{}

void chatpage::sendinfor() {
	QString qmessage = ui.lineEdit->text();
	std::string s_message = qmessage.toStdString();
	const char* messsage = s_message.c_str();
	::send(client_socket, Utf8ToGbk(messsage).c_str(), 1024, 0);
	cleanup();
}

void chatpage::cleanup()
{
	ui.lineEdit->clear();
}
