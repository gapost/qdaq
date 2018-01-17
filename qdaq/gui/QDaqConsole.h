#ifndef _SCRIPTCONSOLE_H_
#define _SCRIPTCONSOLE_H_

#include "QDaqGlobal.h"
#include "QConsoleWidget.h"

class QDaqSession;

class RTLAB_GUI_EXPORT QDaqConsole : public QConsoleWidget
{
    QDaqSession* session;
    QString execCode_;
public:
    explicit QDaqConsole(const QString& startupScript = QString());

protected slots:
    void deferedEvaluate();

protected:
	void exec(const QString& code);
	bool canEvaluate(const QString& code);

    QStringList introspection(const QString& lookup);

	virtual void closeEvent ( QCloseEvent * event );

  //! derived key press event
  virtual void keyPressEvent (QKeyEvent * e);
};

#endif

