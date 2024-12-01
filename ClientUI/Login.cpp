#include "Login.h"

int reg_window_count = 0;
extern SOCKET client_socket;
extern std::string Utf8ToGbk(const std::string& utf8Str);

Login::Login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(login()));
	this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");//渐变色

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
	std::string s_username, s_password, gs_username, gs_password;
	const char* username, * password;
	char back_info[256];
	qusername = ui.lineEdit->text();
	qpassword = ui.lineEdit_2->text();
	s_username = qusername.toStdString();
	s_password = qpassword.toStdString();
	gs_username = Utf8ToGbk(s_username);
	gs_password = Utf8ToGbk(s_password);
	username = gs_username.c_str();
	password = gs_password.c_str();
	//界面美化
	
	//发送用户名密码
	::send(client_socket, username, 1024, 0);
	::send(client_socket, password, 1024, 0);
	//接收返回信息
	recv(client_socket, back_info, 256, 0);
	//登陆成功
	if (!strcmp(back_info, "accept")) {
		window_chat.show();
		//MessageBox(NULL, L"登录成功！", NULL, MB_OK);
		this->close();
	}
	else {
		MessageBox(NULL, L"登录失败！", NULL, MB_OK);
	}
	}