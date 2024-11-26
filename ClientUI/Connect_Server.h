#pragma once

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include "ui_Connect_Server.h"
#include "Login.h"
#include "variables.h"

class Connect_Server : public QWidget
{
	Q_OBJECT

public:
	Connect_Server(QWidget *parent = nullptr);
	~Connect_Server();
private slots:
	void ip_confirmed();
private:
	Ui::Connect_ServerClass ui;
	Login window_login;
};
