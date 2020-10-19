#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QScriptEngine>

#include <QLibraryInfo>

#include "QDaqRoot.h"
#include "QDaqSession.h"
#include "QDaqUi.h"
#include "QDaqIde.h"
#include "QDaqConsole.h"

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString &startupScript, bool &console, bool & debug, QString &errorMessage)
{
    const QCommandLineOption consoleOption(QStringList() << "c" << "console", "Start a script console window.");
    parser.addOption(consoleOption);

    const QCommandLineOption debugOption(QStringList() << "d" << "debug", "Start in script-debugging mode.");
    parser.addOption(debugOption);

    parser.addPositionalArgument("script", "A .js script to run at startup.");
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments())) {
        errorMessage = parser.errorText();
        return CommandLineError;
    }

    if (parser.isSet(versionOption))
        return CommandLineVersionRequested;

    if (parser.isSet(helpOption))
        return CommandLineHelpRequested;

    console = parser.isSet(consoleOption);
    debug = parser.isSet(debugOption);

    const QStringList positionalArguments = parser.positionalArguments();

    if (positionalArguments.size() > 1) {
        errorMessage = "Several 'script' arguments specified.";
        return CommandLineError;
    }
    if (!positionalArguments.empty()) {
        startupScript = positionalArguments.first();
        QFile f(startupScript);
        if (!f.exists()) {
            errorMessage = "Could not find startup script file: " + startupScript;
            return CommandLineError;
        }
    }

    return CommandLineOk;
}


int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(qdaq);


    QApplication app(argc, argv);

    app.setApplicationName("qdaq");
    app.setApplicationDisplayName("QDaq");

    QCommandLineParser parser;
    QString startupScript, errorMessage;
    bool console, debug;

    switch (parseCommandLine(parser, startupScript, console, debug, errorMessage)) {
    case CommandLineOk:
        break;
    case CommandLineError:
#ifdef Q_OS_WIN
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
                             "<html><head/><body><h2>" + errorMessage + "</h2><pre>"
                             + parser.helpText() + "</pre></body></html>");
#else
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
#endif
        return 1;
    case CommandLineVersionRequested:
#ifdef Q_OS_WIN
        QMessageBox::information(0, QGuiApplication::applicationDisplayName(),
                                 QGuiApplication::applicationDisplayName() + ' '
                                 + QCoreApplication::applicationVersion());
#else
        printf("%s %s\n", qPrintable(QGuiApplication::applicationDisplayName()),
               qPrintable(QCoreApplication::applicationVersion()));
#endif
        return 0;
    case CommandLineHelpRequested:
#ifdef Q_OS_WIN
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
                             "<html><head/><body><pre>"
                             + parser.helpText() + "</pre></body></html>");
        return 0;
#else
        parser.showHelp();
        return 0;
        Q_UNREACHABLE();
#endif
    }

    QDaqRoot qdaq;
    QDaqUi ui;
    QDaqSession* s = qdaq.rootSession();

    if (debug) s->debug(true);

    if (startupScript.isEmpty() && !console) {
        QDaqIDE* mainWin = ui.createIdeWindow();
        mainWin->show();
    }
    else {
        // Start a console to run the startup script
        // QDaqConsole* daqConsole = new QDaqConsole(s);
        QDaqConsoleTabWidget* daqConsole = new QDaqConsoleTabWidget;
        daqConsole->addConsole();
        daqConsole->show();

        daqConsole->currentConsole()->writeStdOut("QDaq - Qt-based Data Aqcuisition\n");
        daqConsole->currentConsole()->writeStdOut(QString("Version %1\n\n").arg(QDaq::Version()));

        if (!startupScript.isEmpty()) {
            daqConsole->currentConsole()->writeStdOut(
                        QString("log('Executing startup script %1')").arg(startupScript)
                        );
            s->eval(QString("exec('%1')").arg(startupScript));
            s->waitForFinished(-1);
        }


        if (s->scriptEngine()->hasUncaughtException())
        {
            daqConsole->currentConsole()->onRequestInput(">> ");
            // Do nothing, user interacts with console
        }
        else {
            // if user did not ask for a console, delete the console
            if (!console) delete daqConsole;
            else // otherwise start interactive session
                daqConsole->currentConsole()->onRequestInput(">> ");

            // if no other windows, exit
            if (QApplication::topLevelWidgets().isEmpty()) return 0;
        }
    }

    return app.exec();
}
