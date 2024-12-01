#include "Login.h"

int reg_window_count = 0;
extern SOCKET client_socket;
extern std::string Utf8ToGbk(const std::string& utf8Str);
extern bool connect_status;

Login::Login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(login()));
	ui.lineEdit->setStyleSheet("font: 25 14pt '微软雅黑 Light';" //字体
		"color: rgb(31,31,31);"		//字体颜色
		"padding-left:20px;"       //内边距-字体缩进
		"background-color: rgb(255, 255, 255);" //背景颜色
		"border:2px solid rgb(20,196,188);border-radius:15px;");//边框粗细-颜色-圆角设置 
	ui.lineEdit_2->setStyleSheet("font: 25 14pt '微软雅黑 Light';" //字体
		"color: rgb(31,31,31);"		//字体颜色
		"padding-left:20px;"       //内边距-字体缩进
		"background-color: rgb(255, 255, 255);" //背景颜色
		"border:2px solid rgb(20,196,188);border-radius:15px;");//边框粗细-颜色-圆角设置 
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
		connect_status = false;
		this->close();
	}
	else {
		MessageBox(NULL, L"登录失败！", NULL, MB_OK);
	}
	}