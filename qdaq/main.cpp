#include <QApplication>

#include "QDaqConsole.h"

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(main);


    QApplication app(argc, argv);

    QDaqConsole mainWin;
    mainWin.show();

    return app.exec();
}

