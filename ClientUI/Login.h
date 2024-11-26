#pragma once

#include <QWidget>
#include "ui_Login.h"
#include "variables.h"
#include "Regist.h"

class Login : public QWidget
{
	Q_OBJECT

public:
	Login(QWidget *parent = nullptr);
	~Login();
private slots:
	void regist();
	void login();
private:
	Ui::LoginClass ui;
	Regist window_regist;
};
