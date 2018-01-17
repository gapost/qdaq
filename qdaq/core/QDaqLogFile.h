#ifndef QDAQLOGFILE_H
#define QDAQLOGFILE_H

#include <QObject>

class QFile;

class QDaqLogFile : public QObject
{
    QFile* file_;

    bool timestamp;
    QChar delimiter;

public:


    QDaqLogFile(bool tm = true, QChar delim = QChar('\t'), QObject* parent = 0);
    ~QDaqLogFile();

    bool open(const QString& fname);
    bool isOpen();

    // returns a fully decorated name:
    // logFolder/simpleName_ddMMyyyy_x.log
    static QString getDecoratedName(const QString& simpleName);

    QDaqLogFile& operator<< (const QString& str);

};

#endif // RTLOGFILE_H
