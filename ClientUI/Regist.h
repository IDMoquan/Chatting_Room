#pragma once

#include <QWidget>
#include "ui_Regist.h"

class Regist : public QWidget
{
	Q_OBJECT

public:
	Regist(QWidget *parent = nullptr);
	~Regist();

private slots:
	void regist_confirm();
private:
	Ui::RegistClass ui;
};
