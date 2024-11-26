#include "Login.h"

int reg_window_count = 0;

Login::Login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(regist()));
}

Login::~Login()
{}

void Login::regist(){
	if (window_regist.isVisible() == false) {
		window_regist.show();
	}
	else {
		MessageBox(NULL, "注册窗口已经开启！", NULL, MB_OK);
	}
}

void Login::login() {
	
}