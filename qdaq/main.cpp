#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QScriptEngine>

#include "QDaqRoot.h"
#include "QDaqSession.h"
#include "QDaqIde.h"
#include "QDaqConsole.h"

#include "QDaqPlotWidget.h"

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString &startupScript, bool &console, QString &errorMessage)
{
    const QCommandLineOption consoleOption(QStringList() << "c" << "console", "If a console is needed.");
    parser.addOption(consoleOption);

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
    Q_INIT_RESOURCE(qdaq);


    QApplication app(argc, argv);

    app.setApplicationName("qdaq");
    app.setApplicationDisplayName("QDaq");

    QCommandLineParser parser;
    QString startupScript, errorMessage;
    bool console;

    switch (parseCommandLine(parser, startupScript, console, errorMessage)) {
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

    if (startupScript.isEmpty()) {
        QDaqIDE* mainWin = qdaq.createIdeWindow();
        mainWin->show();
    }
    else {
        QDaqSession* s = qdaq.rootSession();

        QDaqConsole* console = new QDaqConsole(s);
        console->show();

        const char* intro = "QDaq - Qt-based Data Aqcuisition\n"
                "Version 1.0\n\n";
        console->stdOut(intro);



        s->evaluate(QString("print('Executing startup script %1')").arg(startupScript));
        s->evaluate(QString("exec('%1')").arg(startupScript));


        if (s->getEngine()->hasUncaughtException())
        {
            console->flush();
            // Do nothing, user interacts with console
        }
        else {
            delete console;
            if (QApplication::topLevelWidgets().isEmpty()) return 0;
        }
    }

    QDaqPlotWidget w;

    return app.exec();
}

