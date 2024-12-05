#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include "Connect_Server.h"
#include <winsock2.h>
#include <qregularexpression>
#include <QRegularExpressionValidator>
#pragma comment(lib, "ws2_32.lib")

std::string localip;
SOCKET client_socket, client_socket_c;
bool connect_status = true;
const char* status = "login";
int client_count = 0;
char e_server_ip[256] = { 0 };
char c_username[1024] = { 0 };
const int username_length = 1024;
const int password_length = 1024;
const int message_length = 1024;

int charToint(char* str) {
	int res = 0;
	for (int i = 0; i < strlen(str); i++) {
		res *= 10;
		res += (int)str[i] - 48;
	}
	return res;
}

//std::string Utf8ToGbk(const std::string& utf8Str) {
//	// 首先计算需要的宽字符串长度
//	int wideLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
//	std::vector<wchar_t> wideStr(wideLength);
//
//	// 将UTF-8转换为宽字符串
//	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideLength);
//
//	// 计算GBK字符串长度
//	int gbkLength = WideCharToMultiByte(CP_ACP, 0, &wideStr[0], -1, nullptr, 0, nullptr, nullptr);
//	std::vector<char> gbkStr(gbkLength);
//
//	// 将宽字符串转换为GBK字符串
//	WideCharToMultiByte(CP_ACP, 0, &wideStr[0], -1, &gbkStr[0], gbkLength, nullptr, nullptr);
//
//	return std::string(gbkStr.begin(), gbkStr.end() - 1); // 减去末尾的空字符
//}

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
	ui.lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$")));
	ui.lineEdit->setPlaceholderText("192.168.0.1");
	localip = getlocalip();			//本机ip(char *)类型
	this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");//渐变色
}

Connect_Server::~Connect_Server()
{
}

void Connect_Server::ip_confirmed(){
	QString server_ip = ui.lineEdit->text();
	std::string s_server_ip = server_ip.toStdString();
	const char* c_server_ip = s_server_ip.c_str();
	strcpy(e_server_ip, c_server_ip);
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	client_socket_c = socket(AF_INET, SOCK_STREAM, 0);
	//设置属性
	struct sockaddr_in target = { 0 };
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr(c_server_ip);

	struct sockaddr_in c_tar = { 0 };
	c_tar.sin_family = AF_INET;
	c_tar.sin_port = htons(8081);
	c_tar.sin_addr.s_addr = inet_addr(c_server_ip);
	bool connect_seccess = false;	
	//连接服务器
	if (::connect(client_socket, (struct sockaddr*)&target, sizeof(target)) == -1) {
		MessageBox(NULL, L"登录失败1！", NULL, MB_OK);
		return;
	}
	if (::connect(client_socket_c, (struct sockaddr*)&c_tar, sizeof(c_tar)) == -1) {
		MessageBox(NULL, L"登录失败2！", NULL, MB_OK);
		return;
	}
	send(client_socket, localip.c_str(), 256, 0);
	window_login.show();
	send(client_socket, status, 256, 0);
	this->close();
}