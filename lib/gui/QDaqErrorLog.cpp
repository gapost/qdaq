#include "QDaqErrorLog.h"

#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "QDaqRoot.h"

QDaqErrorLog::QDaqErrorLog(QWidget* p) : QWidget(p)
{
    tableWidget = new QTableWidget(this);

    tableWidget->setColumnCount(4);
    QStringList headers;
    headers << "Time" << "Object" << "Type" << "Description";
    tableWidget->setHorizontalHeaderLabels(headers);

    tableWidget->setShowGrid(false);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->verticalHeader()->hide();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setWordWrap(false);

    tableWidget->setColumnWidth(0,200);
    tableWidget->setColumnWidth(1,200);
    tableWidget->setColumnWidth(2,200);

    QHeaderView* hdr = tableWidget->horizontalHeader();
    hdr->setMinimumSectionSize(300);
    hdr->setStretchLastSection(true);

    clearButton = new QPushButton(QIcon(":/images/clear.png"),QString(),this);
    connect(clearButton,SIGNAL(clicked()),this,SLOT(clear()));
    clearButton->setMaximumHeight(24);
    clearButton->setIconSize(QSize(16,16));
    clearButton->setFlat(true);
    clearButton->setToolTip(tr("Clear List"));

    QVBoxLayout* vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(clearButton);
    hlayout->addStretch();
    vlayout->addLayout(hlayout);
    vlayout->addWidget(tableWidget);
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(6,0,6,6);

    setLayout(vlayout);



    QList<QDaqError> lst = QDaqObject::root()->errorQueue()->errorQueue();
    foreach(const QDaqError& e, lst)
    {
        onError(e);
    }



    connect(QDaqObject::root(),SIGNAL(error(QDaqError)),this,SLOT(onError(QDaqError)),Qt::QueuedConnection);
}

QDaqErrorLog::~QDaqErrorLog(void)
{

}

void QDaqErrorLog::onError(const QDaqError &e)
{
    tableWidget->insertRow(0);
    tableWidget->setItem(0,0,new QTableWidgetItem(e.t.toString("hh:mm:ss.zzz")));
    tableWidget->setItem(0,1,new QTableWidgetItem(e.objectName));
    tableWidget->setItem(0,2,new QTableWidgetItem(e.type));
    tableWidget->setItem(0,3,new QTableWidgetItem(e.descr));
    tableWidget->resizeRowToContents(0);
}

void QDaqErrorLog::clear()
{
    tableWidget->clear();
    QStringList headers;
    headers << "Time" << "Object" << "Type" << "Description";
    tableWidget->setHorizontalHeaderLabels(headers);
}
