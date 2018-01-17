#include <QtGui>

#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>


#include "QDaqScriptEditor.h"

QDaqScriptEditor::QDaqScriptEditor()
{
    setAttribute(Qt::WA_DeleteOnClose);
	setTabStopWidth ( 40 );

    isUntitled = true;

	QTextCharFormat fmt = currentCharFormat();
	fmt.setFontFamily("Courier New");
	fmt.setFontFixedPitch ( true );
	fmt.setFontPointSize(10);
	setCurrentCharFormat(fmt);

}

void QDaqScriptEditor::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool QDaqScriptEditor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Script Editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    return true;
}

bool QDaqScriptEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool QDaqScriptEditor::saveAs()
{
	QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                "Save script file as",
                                curFile,
                                tr("Script Files (*.js);;All Files (*)"),
                                &selectedFilter,
                                options);

    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool QDaqScriptEditor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Script Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString QDaqScriptEditor::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void QDaqScriptEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void QDaqScriptEditor::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool QDaqScriptEditor::maybeSave()
{
    if (document()->isModified()) {
	QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Script Editor"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
		     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void QDaqScriptEditor::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString QDaqScriptEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
