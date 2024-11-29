#pragma once

#include <QWidget>
#include "ui_chatpage.h"
//#include "Login.h"

class chatpage : public QWidget
{
	Q_OBJECT

public:
	chatpage(QWidget *parent = nullptr);
	~chatpage();
private slots:
	void sendinfor();
	void cleanup();
private:
	Ui::chatpageClass ui;
};
