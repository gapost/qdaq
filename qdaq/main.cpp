#include <QApplication>

#include "QDaqRoot.h"
#include "QDaqIde.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qdaq);


    QApplication app(argc, argv);

    QDaqRoot qdaq;

    QDaqIDE mainWin;
    mainWin.show();

    return app.exec();
}

