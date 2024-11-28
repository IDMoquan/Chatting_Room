#include "Login.h"

int reg_window_count = 0;
extern SOCKET client_socket;

Login::Login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(regist()));
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
	
}