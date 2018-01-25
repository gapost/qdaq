#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QMessageBox>

#include "core/QDaqRoot.h"
#include "gui/QDaqIde.h"
#include "core/QDaqSession.h"

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
        s->exec(startupScript);

        if (QApplication::topLevelWidgets().isEmpty()) return 0;
    }

    return app.exec();
}

