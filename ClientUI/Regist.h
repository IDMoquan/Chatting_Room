#pragma once

#include <QWidget>
#include "ui_Regist.h"

class Regist : public QWidget
{
	Q_OBJECT

public:
	Regist(QWidget *parent = nullptr);
	~Regist();

private:
	Ui::RegistClass ui;
};
