#include <QApplication>

#include "QDaqIde.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qdaq);


    QApplication app(argc, argv);

    QDaqIDE mainWin;
    mainWin.show();

    return app.exec();
}

