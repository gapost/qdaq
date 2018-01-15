#ifndef _QCONSOLEWIDGET_H_
#define _QCONSOLEWIDGET_H_

#include "QDaqGlobal.h"

#include <QPlainTextEdit>


class QCompleter;

//-------------------------------------------------------------------------------
//! A simple console for python scripting
class RTLAB_GUI_EXPORT QConsoleWidget : public QPlainTextEdit
{
  Q_OBJECT

public:
  QConsoleWidget(QWidget* parent = 0);

  ~QConsoleWidget();

public slots:
  //! execute current line
  void executeLine(bool storeOnly);

  //! output from console
  void consoleMessage(const QString & message, const QTextCharFormat& fmt);

  //! get history
  // QStringList history() { return _history; }

  //! set history
  // void setHistory(const QStringList& h) { _history = h; _historyPosition = 0; }

  //! clear the console
  void clear();

  //! overridden to control which characters a user may delete
  virtual void cut();

  //! output redirection
  void stdOut(const QString& s);
  //! output redirection
  void stdErr(const QString& s);
  //! output redirection
  void endSession();

  void insertCompletion(const QString&);

  //! Appends a newline and command prompt at the end of the document.
  void appendCommandPrompt(bool storeOnly = false);

protected:
  //! derived key press event
  virtual void keyPressEvent (QKeyEvent * e);

  //! handle the pressing of tab
  void handleTabCompletion();

  //! Returns the position of the command prompt
  int commandPromptPosition();

  //! Returns if deletion is allowed at the current cursor
  //! (with and without selected text)
  bool verifySelectionBeforeDeletion();

  //! Sets the current font
  void setCurrentFont(const QColor& color = QColor(0,0,0), bool bold = false);

  //! change the history according to _historyPos
  //void changeHistory();
  void replaceCommandLine(const QString& str);

  //! flush output that was not yet printed
  void flushStdOut();

  QString getCurrentLine();

  virtual void exec(const QString& code) = 0;
  virtual bool canEvaluate(const QString& code) = 0;
  virtual QStringList introspection(const QString& lookup) = 0;

private:
  void executeCode(const QString& code);


  class QConsoleHistory
  {
      QStringList strings_;
      int pos_;
      QString token_;
      bool active_;
      int maxsize_;
  public:
      QConsoleHistory(void);
      ~QConsoleHistory(void);

      void add(const QString& str);

      const QString& currentValue() const
      {
          return pos_ == -1 ? token_ : strings_.at(pos_);
      }

      void activate(const QString& tk = QString());
      void deactivate() { active_ = false; }
      bool isActive() const { return active_; }

      bool move(bool dir);

      int indexOf(bool dir, int from) const;
  };

  static QConsoleHistory _history;

  QString _clickedAnchor;
  QString _storageKey;
  QString _commandPrompt;
  bool _commandPromptInserted;

  QString _currentMultiLineCode;

  QString _stdOut;
  QString _stdErr;

  //QTextCharFormat _defaultTextCharacterFormat;
  QTextCharFormat _defaultFormat;
  QTextCharFormat _stdOutFormat;
  QTextCharFormat _stdErrFormat;
  QCompleter* _completer;

  //QTextStream
};



#endif
