#include "QDaqLogFile.h"

#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QDateTime>

#include "QDaqObject.h"
#include "QDaqRoot.h"

QDaqLogFile::QDaqLogFile(bool tm, QChar delim, QObject *parent) : QObject(parent),
    file_(0), timestamp(tm), delimiter(delim)
{

}

QDaqLogFile::~QDaqLogFile()
{
    if (file_) delete file_;
}

bool QDaqLogFile::open(const QString &fname)
{
    if (file_) delete file_;
    file_ = new QFile(fname);
    QFile::OpenMode mode = file_->exists() ? QFile::Append : QFile::Truncate;
    mode |= QIODevice::WriteOnly;
    return file_->open(mode);
}

bool QDaqLogFile::isOpen()
{
    if (file_) return file_->isOpen();
    return false;
}

QString QDaqLogFile::getDecoratedName(const QString &simpleName)
{
    QString fname(simpleName), fullName;
    fname += QDate::currentDate().toString("_ddMMyyyy");
    fullName = QDaqObject::root()->logDir() + QChar('/') + fname + QString(".log");
    return fullName;
}

QDaqLogFile& QDaqLogFile::operator <<(const QString& str)
{
    if (!isOpen()) return *this;

    QTextStream stream(file_);

    if (timestamp) {
        QDateTime cdt = QDateTime::currentDateTime();
        stream << cdt.toString("dd.MM.yyyy") << delimiter
               << cdt.toString("hh:mm:ss") << delimiter;
    }

    stream << str << '\n';

    return *this;
}
