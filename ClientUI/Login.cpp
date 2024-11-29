#include "Login.h"

int reg_window_count = 0;
extern SOCKET client_socket;


Login::Login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(login()));
}

Login::~Login()
{}

void Login::regist(){
	if (window_regist.isVisible() == false) {
		window_regist.show();
		const char* status = "regist";
		send(client_socket, status, 256, 0);
	}
	else {
		MessageBox(NULL, L"注册窗口已经开启！", NULL, MB_OK);
	}
}

void Login::login() {
	//MessageBox(NULL, L"登录！", NULL, MB_OK);
	QString qusername, qpassword;
	std::string s_username, s_password;
	const char* username, * password;
	char back_info[256];
	qusername = ui.lineEdit->text();
	qpassword = ui.lineEdit_2->text();
	s_username = qusername.toStdString();
	s_password = qpassword.toStdString();
	username = s_username.c_str();
	password = s_password.c_str();
	//发送用户名密码
	::send(client_socket, username, 256, 0);
	::send(client_socket, password, 256, 0);
	//接收返回信息
	recv(client_socket, back_info, 256, 0);
	//登陆成功
	if (!strcmp(back_info, "accept")) {
		this->close();
		MessageBox(NULL, L"登录成功！", NULL, MB_OK);
	}
	else {
		MessageBox(NULL, L"登录失败！", NULL, MB_OK);
	}
}