#include "chatpage.h"
#include "variables.h"
#include "Login.h"
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET client_socket;

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
	::send(client_socket, messsage, 1024, 0);
	cleanup();
}

void chatpage::cleanup()
{
	ui.lineEdit->clear();
}
