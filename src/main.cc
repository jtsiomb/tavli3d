#include <QApplication>
#include "mainwin.h"
#include "opt.h"

int main(int argc, char **argv)
{
	if(!init_options(argc, argv)) {
		return -1;
	}

	QApplication a(argc, argv);
	MainWin w;
	w.show();

	return a.exec();
}
