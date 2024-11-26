#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include "Connect_Server.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

std::string localip;

std::string getlocalip() {
	int ret;
	char hostname[256] = { 0 };
	ret = gethostname(hostname, sizeof(hostname));
	hostent* host = gethostbyname(hostname);
	return inet_ntoa(*(struct in_addr*)*host->h_addr_list);
}

Connect_Server::Connect_Server(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(ip_confirmed()));
	localip = getlocalip();			//本机ip(char *)类型
}

Connect_Server::~Connect_Server()
{
}

void Connect_Server::ip_confirmed(){
	QString server_ip = ui.lineEdit->text();
	std::string s_server_ip = server_ip.toStdString();
	const char* c_server_ip = s_server_ip.c_str();
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	bool connect_seccess = false;	
	//设置属性
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr(c_server_ip);
	//连接服务器
	if (::connect(client_socket, (struct sockaddr*)&target, sizeof(target)) == -1) {
		return;
	}
	send(client_socket, localip.c_str(), 256, 0);
	window_login.show();
	this->close();
}