#pragma once

#include <QWidget>
#include "ui_chatpage.h"

class chatpage : public QWidget
{
	Q_OBJECT

public:
	chatpage(QWidget *parent = nullptr);
	~chatpage();

private:
	Ui::chatpageClass ui;
};
