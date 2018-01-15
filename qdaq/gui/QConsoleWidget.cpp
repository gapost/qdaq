#include "QConsoleWidget.h"

#include <QMenu>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>
#include <QCompleter>
#include <QStringListModel>
#include <QScrollBar>
#include <QAbstractItemView>


QConsoleWidget::QConsoleHistory QConsoleWidget::_history;

//-----------------------------------------------------------------------------

QConsoleWidget::QConsoleWidget(QWidget* parent)
: QPlainTextEdit(parent), _commandPromptInserted(false) {

  //setWindowFlags(windowFlags);
//	QTextCharFormat fmt = currentCharFormat();
//	fmt.setFontFamily("Courier New");
//	fmt.setFontFixedPitch ( true );
//	fmt.setFontPointSize(10);
//	setCurrentCharFormat(fmt);

    this->setMinimumSize(600,400);


	_defaultFormat = _stdOutFormat = _stdErrFormat = currentCharFormat();
	_stdOutFormat.setForeground(Qt::darkBlue);
	_stdErrFormat.setForeground(Qt::red);
	_stdErrFormat.setFontWeight(QFont::Bold);

 // _historyPosition            = 0;

  _completer = new QCompleter(this);
  _completer->setWidget(this);
  QObject::connect(_completer, SIGNAL(activated(const QString&)),
    this, SLOT(insertCompletion(const QString&)));

  clear();

  setTextInteractionFlags(Qt::TextEditorInteraction);

  //connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(stdOut(const QString&)));
  //connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(stdErr(const QString&)));
}

//-----------------------------------------------------------------------------

void QConsoleWidget::stdOut(const QString& s)
{
  _stdOut += s;
  int idx;
  while ((idx = _stdOut.lastIndexOf('\n'))!=-1) {
    consoleMessage(_stdOut.left(idx+1), _stdOutFormat);
    //std::cout << _stdOut.left(idx).toLatin1().data() << std::endl;
    _stdOut = _stdOut.mid(idx+1);
  }
}

void QConsoleWidget::stdErr(const QString& s)
{
  _stdErr += s;
  int idx;
  while ((idx = _stdErr.lastIndexOf('\n'))!=-1) {
    consoleMessage(_stdErr.left(idx+1), _stdErrFormat);
    //std::cout << _stdErr.left(idx).toLatin1().data() << std::endl;
    _stdErr = _stdErr.mid(idx+1);
  }
}

void QConsoleWidget::flushStdOut()
{
  if (!_stdOut.isEmpty()) {
    stdOut("\n");
  }
  if (!_stdErr.isEmpty()) {
    stdErr("\n");
  }
}

void QConsoleWidget::endSession()
{
	QWidget* w = window();
	w->close();
	//QCoreApplication::postEvent(window(),new QCloseEvent());
}

//-----------------------------------------------------------------------------

QConsoleWidget::~QConsoleWidget() {
}



//-----------------------------------------------------------------------------

void QConsoleWidget::clear() {

  QPlainTextEdit::clear();
  appendCommandPrompt();
}

//-----------------------------------------------------------------------------

QString QConsoleWidget::getCurrentLine()
{
  QTextCursor textCursor = this->textCursor();
  textCursor.movePosition(QTextCursor::End);

  // Select the text from the command prompt until the end of the block
  // and get the selected text.
  textCursor.setPosition(commandPromptPosition());
  textCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  return textCursor.selectedText();
}

//-----------------------------------------------------------------------------

void QConsoleWidget::executeLine(bool storeOnly)
{
  QString code = getCurrentLine();

  // i don't know where this trailing space is coming from, blast it!
  if (code.endsWith(" ")) {
    code.truncate(code.length()-1);
  }

  // start new block without prompt
  appendPlainText(QString());
  _commandPromptInserted = false;

  QTextCursor textCursor = this->textCursor();
	textCursor.movePosition(QTextCursor::End);
	setTextCursor(textCursor);

  if (!code.isEmpty()) {
    // Update the history
    //_history << code;
    //_historyPosition = _history.count();
	  _history.add(code);
    _currentMultiLineCode += code + "\n";

    //if (!storeOnly) {
	if (canEvaluate(_currentMultiLineCode)) {
      executeCode(_currentMultiLineCode);
      _currentMultiLineCode = "";
    }
	else storeOnly = true;
  }
  // Insert a new command prompt
  appendCommandPrompt(storeOnly);

}

void QConsoleWidget::executeCode(const QString& code)
{
  // put visible cursor to the end of the line
  //QTextCursor cursor = QPlainTextEdit::textCursor();
  //cursor.movePosition(QTextCursor::End);
  //setTextCursor(cursor);

  // mark position
  // int cursorPosition = this->textCursor().position();

  exec(code);
  flushStdOut();

  // bool textInserted = (this->textCursor().position() != cursorPosition);

  // If a message was inserted, then put another empty line before the command prompt
  // to improve readability.
  //if (textInserted) {
  //  appendPlainText(QString());
  //}
}


//-----------------------------------------------------------------------------

void QConsoleWidget::appendCommandPrompt(bool storeOnly) {
  if (storeOnly) {
    _commandPrompt = "...> ";
  } else {
    _commandPrompt = ">> ";
  }

  QTextCursor cursor = textCursor();
  cursor.movePosition(QTextCursor::End);
  setTextCursor(cursor);

  cursor.insertText(_commandPrompt,_defaultFormat);
  ensureCursorVisible();
  //appendPlainText(_commandPrompt);
  _commandPromptInserted = true;

}



//-----------------------------------------------------------------------------

void QConsoleWidget::setCurrentFont(const QColor& color, bool bold) {

  QTextCharFormat charFormat(_defaultFormat);

  QFont font(charFormat.font());
  font.setBold(bold);
  charFormat.setFont(font);

  QBrush brush(charFormat.foreground());
  brush.setColor(color);
  charFormat.setForeground(brush);

  setCurrentCharFormat(charFormat);
}



//-----------------------------------------------------------------------------

int QConsoleWidget::commandPromptPosition() {

  QTextCursor textCursor(this->textCursor());
  textCursor.movePosition(QTextCursor::End);

  return textCursor.block().position() + _commandPrompt.length();
}



//-----------------------------------------------------------------------------

void QConsoleWidget::insertCompletion(const QString& completion)
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
  if (tc.selectedText()==".") {
    tc.insertText(QString(".") + completion);
  } else {
    tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    tc.insertText(completion);
    setTextCursor(tc);
  }
}

//-----------------------------------------------------------------------------
void QConsoleWidget::handleTabCompletion()
{
  QTextCursor textCursor   = this->textCursor();
  int pos = textCursor.position();
  textCursor.setPosition(commandPromptPosition());
  textCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  int startPos = textCursor.selectionStart();

  int offset = pos-startPos;
  QString text = textCursor.selectedText();

  QString textToComplete;
  int cur = offset;
  while (cur--) {
    QChar c = text.at(cur);
    if (c.isLetterOrNumber() || c == '.' || c == '_') {
      textToComplete.prepend(c);
    } else {
      break;
    }
  }


  QString lookup;
  QString compareText = textToComplete;
  int dot = compareText.lastIndexOf('.');
  if (dot!=-1) {
    lookup = compareText.mid(0, dot);
    compareText = compareText.mid(dot+1, offset);
  }
  if (!lookup.isEmpty() || !compareText.isEmpty()) {
    compareText = compareText.toLower();
    QStringList found;
    QStringList l = introspection(lookup); // PythonQt::self()->introspection(_context, lookup, PythonQt::Anything);
    foreach (QString n, l) {
      if (n.toLower().startsWith(compareText)) {
        found << n;
      }
    }
    
    if (!found.isEmpty()) {
      _completer->setCompletionPrefix(compareText);
      _completer->setCompletionMode(QCompleter::PopupCompletion);
      _completer->setModel(new QStringListModel(found, _completer));
      _completer->setCaseSensitivity(Qt::CaseInsensitive);
      QTextCursor c = this->textCursor();
      c.movePosition(QTextCursor::StartOfWord);
      QRect cr = cursorRect(c);
      cr.setWidth(_completer->popup()->sizeHintForColumn(0)
        + _completer->popup()->verticalScrollBar()->sizeHint().width());
      cr.translate(0,8);
      _completer->complete(cr);
    } else {
      _completer->popup()->hide();
    }
  } else {
    _completer->popup()->hide();
  }
}

void QConsoleWidget::keyPressEvent(QKeyEvent* event) {

	  int key = event->key();

  if (_completer && _completer->popup()->isVisible()) {
    // The following keys are forwarded by the completer to the widget
    switch (key) {
    case Qt::Key_Return:
      if (!_completer->popup()->currentIndex().isValid()) {
        insertCompletion(_completer->currentCompletion());
        _completer->popup()->hide();
        event->accept();
      }
      event->ignore();
      return;
      break;
    case Qt::Key_Enter:
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
    
      event->ignore();
      return; // let the completer do default behavior
    default:
      break;
    }
  }

  if (_history.isActive())
  {
	  switch (key)
	  {
		case Qt::Key_Up:
		case Qt::Key_Down:
			if (_history.move(key==Qt::Key_Up)) 
				//changeHistory();
				replaceCommandLine(_history.currentValue());
			else QApplication::beep();
			event->accept();
			return;
		default:
			_history.deactivate();
			break;
	  }
  }

  bool        eventHandled = false;
  QTextCursor textCursor   = this->textCursor();

  switch (key) {
	  
  case Qt::Key_Escape:
		replaceCommandLine(QString());
		eventHandled = true;
		break;


  case Qt::Key_Left:

    // Moving the cursor left is limited to the position
    // of the command prompt.

    if (textCursor.position() <= commandPromptPosition()) {

      QApplication::beep();
      eventHandled = true;
    }
    break;

  case Qt::Key_Up:
	  if (!_history.isActive()) _history.activate(getCurrentLine());

  //case Qt::Key_Down:
	  if (_history.move(key==Qt::Key_Up)) 
		 replaceCommandLine(_history.currentValue());
	  else QApplication::beep();

	  // Display the previous command in the history
    //if (_historyPosition>0) {
      //_historyPosition--;
      //changeHistory();
    //}

    // Display the next command in the history
    //if (_historyPosition+1<_history.count()) {
     // _historyPosition++;
      //changeHistory();
    //}
    eventHandled = true;
    break;

  case Qt::Key_Return:

    executeLine(event->modifiers() & Qt::ShiftModifier);
    eventHandled = true;
    break;

  case Qt::Key_Backspace:

    if (textCursor.hasSelection()) {

      cut();
      eventHandled = true;

    } else {

      // Intercept backspace key event to check if
      // deleting a character is allowed. It is not
      // allowed, if the user wants to delete the
      // command prompt.

      if (textCursor.position() <= commandPromptPosition()) {

        QApplication::beep();
        eventHandled = true;
      }
    }
    break;

  case Qt::Key_Delete:

    cut();
    eventHandled = true;
    break;

  default:

    if (key >= Qt::Key_Space && key <= Qt::Key_division) {

      if (textCursor.hasSelection() && !verifySelectionBeforeDeletion()) {

        // The selection must not be deleted.
        eventHandled = true;

      } else {

        // The key is an input character, check if the cursor is
        // behind the last command prompt, else inserting the
        // character is not allowed.

        int commandPromptPosition = this->commandPromptPosition();
        if (textCursor.position() < commandPromptPosition) {

          textCursor.setPosition(commandPromptPosition);
          setTextCursor(textCursor);
        }
      }
    }
  }

  if (eventHandled) {

    _completer->popup()->hide();
    event->accept();

  } else {

    QPlainTextEdit::keyPressEvent(event);
    QString text = event->text();
    if (!text.isEmpty()) {
      handleTabCompletion();
    } else {
      _completer->popup()->hide();
    }
    eventHandled = true;
  }
}



//-----------------------------------------------------------------------------

void QConsoleWidget::cut() {

  bool deletionAllowed = verifySelectionBeforeDeletion();
  if (deletionAllowed) {
    QPlainTextEdit::cut();
  }
}



//-----------------------------------------------------------------------------

bool QConsoleWidget::verifySelectionBeforeDeletion() {

  bool deletionAllowed = true;

  
  QTextCursor textCursor = this->textCursor();

  int commandPromptPosition = this->commandPromptPosition();
  int selectionStart        = textCursor.selectionStart();
  int selectionEnd          = textCursor.selectionEnd();

  if (textCursor.hasSelection()) {

    // Selected text may only be deleted after the last command prompt.
    // If the selection is partly after the command prompt set the selection
    // to the part and deletion is allowed. If the selection occurs before the
    // last command prompt, then deletion is not allowed.

    if (selectionStart < commandPromptPosition ||
      selectionEnd < commandPromptPosition) {

      // Assure selectionEnd is bigger than selection start
      if (selectionStart > selectionEnd) {
        int tmp         = selectionEnd;
        selectionEnd    = selectionStart;
        selectionStart  = tmp;
      }

      if (selectionEnd < commandPromptPosition) {

        // Selection is completely before command prompt,
        // so deletion is not allowed.
        QApplication::beep();
        deletionAllowed = false;

      } else {

        // The selectionEnd is after the command prompt, so set
        // the selection start to the commandPromptPosition.
        selectionStart = commandPromptPosition;
        textCursor.setPosition(selectionStart);
        textCursor.setPosition(selectionStart, QTextCursor::KeepAnchor);
        setTextCursor(textCursor);
      }
    }

  } else { // if (hasSelectedText())

    // When there is no selected text, deletion is not allowed before the
    // command prompt.
    if (textCursor.position() < commandPromptPosition) {

      QApplication::beep();
      deletionAllowed = false;
    }
  }

  return deletionAllowed;
}



//-----------------------------------------------------------------------------

void QConsoleWidget::replaceCommandLine(const QString& str) {

  // Select the text after the last command prompt ...
  QTextCursor textCursor = this->textCursor();
  textCursor.movePosition(QTextCursor::End);
  textCursor.setPosition(commandPromptPosition(), QTextCursor::KeepAnchor);

  // ... and replace it with the history text.
  //textCursor.insertText(_history.value(_historyPosition));
  //textCursor.insertText(_history.currentValue());
  textCursor.insertText(str);

  textCursor.movePosition(QTextCursor::End);
  setTextCursor(textCursor);
}



//-----------------------------------------------------------------------------

void QConsoleWidget::consoleMessage(const QString & message, const QTextCharFormat& fmt) {

	if (_commandPromptInserted)
	{
		QTextCursor tc = textCursor();
		int editpos = tc.position();
		tc.movePosition(QTextCursor::End);
		editpos = tc.position() - editpos;
		tc.movePosition(QTextCursor::StartOfBlock);
		tc.insertBlock();
		tc.movePosition(QTextCursor::PreviousBlock);
		//tc.insertHtml(message);
		tc.insertText(message,fmt);
		tc.movePosition(QTextCursor::End);
		tc.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,editpos);
		setTextCursor(tc);
	}
	else
	{
		//appendPlainText(QString());
		//insertPlainText(message);
		//insertHtml(message);
		textCursor().insertText(message,fmt);
	}

	ensureCursorVisible();

	// Reset all font modifications done by the html string
	setCurrentCharFormat(_defaultFormat);
}

/////////////////// QConsoleWidget::QConsoleHistory /////////////////////

#define HISTORY_FILE "_command_history.lst"

QConsoleWidget::QConsoleHistory::QConsoleHistory(void) : pos_(0), active_(false), maxsize_(10000)
{
    QFile f(HISTORY_FILE);
    if (f.open(QFile::ReadOnly)) {
        QTextStream is(&f);
        while(!is.atEnd()) add(is.readLine());
    }
}
QConsoleWidget::QConsoleHistory::~QConsoleHistory(void)
{
    QFile f(HISTORY_FILE);
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream os(&f);
        int n = strings_.size();
        while(n>0) os << strings_.at(--n) << endl;
    }
}

void QConsoleWidget::QConsoleHistory::add(const QString& str)
{
    if (strings_.size() == maxsize_) strings_.pop_back();
    strings_.push_front(str);
    active_ = false;
}

void QConsoleWidget::QConsoleHistory::activate(const QString& tk)
{
    active_ = true;
    token_ = tk;
    pos_ = -1;
}

bool QConsoleWidget::QConsoleHistory::move(bool dir)
{
    if (active_)
    {
        int next = indexOf ( dir, pos_ );
        if (pos_!=next)
        {
            pos_=next;
            return true;
        }
        else return false;
    }
    else return false;
}

int QConsoleWidget::QConsoleHistory::indexOf(bool dir, int from) const
{
    int i = from, to = from;
    if (dir)
    {
        while(i < strings_.size()-1)
        {
            const QString& si = strings_.at(++i);
            if (si.startsWith(token_)) { return i; }
        }
    }
    else
    {
        while(i > 0)
        {
            const QString& si = strings_.at(--i);
            if (si.startsWith(token_)) { return i; }
        }
        return -1;
    }
    return to;
}

