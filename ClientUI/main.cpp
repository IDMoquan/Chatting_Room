#include <stdio.h>
#include "Connect_Server.h"
#include <QtWidgets/QApplication>

using namespace std;
int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	Connect_Server w;
	w.show();
	return a.exec();
}