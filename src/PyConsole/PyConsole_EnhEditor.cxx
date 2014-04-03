

#include "PyConsole.h"
#include <Python.h>

#include <QKeyEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QRegExp>
#include <QMimeData>

#include "PyConsole_EnhEditor.h"
#include "PyConsole_EnhInterp.h"
#include "PyConsole_Request.h"
#include "PyInterp_Dispatcher.h"

// Initialize list of valid separators
static const char * tmp_a[] = {" ", "(", "[","+", "-", "*", "/", ";", "^", "="};
const std::vector<QString> PyConsole_EnhEditor::SEPARATORS = \
    std::vector<QString>(tmp_a, tmp_a + sizeof(tmp_a)/sizeof(tmp_a[0]));

/**
 * Constructor.
 * @param interp the interpreter linked to the editor
 * @param parent parent widget
 */
PyConsole_EnhEditor::PyConsole_EnhEditor(PyConsole_EnhInterp * interp, QWidget * parent) :
     PyConsole_Editor(interp, parent),
     _tab_mode(false),
     _cursor_pos(-1),
     _multi_line_paste(false),
     _multi_line_content()
{
  document()->setUndoRedoEnabled(true);
}

/**
 * Overrides. Catches the TAB and Ctrl+TAB combinations.
 * @param event
 */
void PyConsole_EnhEditor::keyPressEvent ( QKeyEvent* event)
{
  // check if <Ctrl> is pressed
  bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
  // check if <Shift> is pressed
  bool shftPressed = event->modifiers() & Qt::ShiftModifier;

  if (event->key() == Qt::Key_Tab && !shftPressed)
    {
      if (!ctrlPressed)
        handleTab();
      else
        {
          clearCompletion();
          handleBackTab();
        }
      PyConsole_Editor::keyPressEvent(event);
    }
  else
    {
      // If ctrl is not pressed (and sth else is pressed with it),
      // or if ctrl is not pressed alone
      if (!ctrlPressed || (ctrlPressed && event->key() != Qt::Key_Control))
        {
          clearCompletion();
          _cursor_pos = -1;
        }
      // Discard ctrl pressed alone:
      if (event->key() != Qt::Key_Control)
        PyConsole_Editor::keyPressEvent(event);
    }
}

/**
 * Whenever the mouse is clicked, clear the completion.
 * @param e
 */
void PyConsole_EnhEditor::mousePressEvent(QMouseEvent* e)
{
  clearCompletion();
  _cursor_pos = -1;
  PyConsole_Editor::mousePressEvent(e);
}

/**
 * Clear in the editor the block of text displayed after having hit <TAB>.
 */
void PyConsole_EnhEditor::clearCompletion()
{
  // Delete completion text if present
  if (_tab_mode)
    {
      // Remove completion display
      document()->undo();
      // Remove trailing line return:
      QTextCursor tc(textCursor());
      tc.setPosition(document()->characterCount()-1);
      setTextCursor(tc);
      textCursor().deletePreviousChar();
      // TODO: before wait for any TAB event to be completed
      static_cast<PyConsole_EnhInterp *>(myInterp)->clearCompletion();
    }
  _tab_mode = false;
}

/**
 * Handle the sequence of events after having hit <TAB>
 */
void PyConsole_EnhEditor::handleTab()
{
  if (_tab_mode)
    {
      // Already tab mode - nothing to do !
      return;
    }

  QTextCursor cursor(textCursor());

  // Cursor at end of input
  cursor.movePosition(QTextCursor::End);
  setTextCursor(cursor);

  // Save cursor position if needed
  if (_cursor_pos == -1)
    _cursor_pos = textCursor().position();

  // get last line
  QTextBlock par = document()->end().previous();
  if ( !par.isValid() ) return;

  // Switch to completion mode
  _tab_mode = true;

  QString cmd = par.text().mid(promptSize());

  // Post completion request
  // Editor will be informed via a custom event that completion has been run
  PyInterp_Request* req = createTabRequest(cmd);
  PyInterp_Dispatcher::Get()->Exec(req);
}

/**
 * Handles what happens after hitting Ctrl-TAB
 */
void PyConsole_EnhEditor::handleBackTab()
{
  QTextCursor cursor(textCursor());

  if (_cursor_pos == -1)
    {
      // Invalid cursor position - we can't do anything:
      return;
    }
  // Ensure cursor is at the end of command line
  cursor.setPosition(_cursor_pos);
  cursor.movePosition(QTextCursor::EndOfLine);
  //setCursor(cursor);

  // Delete last completed text
  int i = cursor.position() - _cursor_pos;
  cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, i);
  cursor.removeSelectedText();
  _cursor_pos = -1;
}

/**
 * Create the Python requested that will be posted to the interpreter to
 * get the completions.
 * @param input line typed by the user at the time TAB was hit
 * @return a CompletionCommand
 * @sa CompletionCommand
 */
PyInterp_Request* PyConsole_EnhEditor::createTabRequest( const QString& input )
{
  // Parse input to extract on what part the dir() has to be executed
  QString input2(input);

  // Split up to the last syntaxical separator
  int lastSp = -1;
  for (std::vector<QString>::const_iterator i = SEPARATORS.begin(); i != SEPARATORS.end(); i++)
    {
      int j = input2.lastIndexOf(*i);
      if (j > lastSp)
        lastSp = j;
    }
  if (lastSp >= 0)
    input2 = input.mid(lastSp+1);

  // Detect a qualified name (with a point)
  int lastPt = input2.lastIndexOf(QString("."));

  // Split the 2 surrounding parts of the qualified name
  if (lastPt != -1)
    {
      _compl_before_point = input2.left(lastPt);
      _compl_after_point = input2.mid(lastPt+1);
    }
  else
    {
      // No point found - do a global matching -
      // (the following will call dir() with an empty string)
      _compl_after_point = input2;
      _compl_before_point = QString("");
    }

  return new CompletionCommand( static_cast<PyConsole_EnhInterp *>(myInterp), _compl_before_point,
                               _compl_after_point, this, isSync() );
}

/**
 * Format completion results - this is where we should create 3 columns etc ...
 * @param matches list of possible completions
 * @param result return value
 */
void PyConsole_EnhEditor::formatCompletion(const std::vector<QString> & matches, QString & result) const
{
  int sz = matches.size();

  if (sz > MAX_COMPLETIONS)
    {
      sz = MAX_COMPLETIONS;
      result.append("[Too many matches! Displaying first ones only ...]\n");
    }

  for (int i = 0; i < sz; ++i)
    {
      result.append(matches[i]);
      result.append("\n");
    }
}

/**
 * Override. Catches the events generated by the enhanced interpreter after the execution
 * of a completion request.
 * @param event
 */
void PyConsole_EnhEditor::customEvent( QEvent* event )
{
  std::vector<QString> matches;
  QString first_match, comple_text, doc, base;
  QTextCursor cursor(textCursor());
  QTextBlockFormat bf;
  QTextCharFormat cf;
  PyConsole_EnhInterp * interp = static_cast<PyConsole_EnhInterp *>(myInterp);
  int cursorPos;

  switch( event->type() )
  {
    case PyInterp_Event::ES_TAB_COMPLETE_OK:
      // Extract corresponding matches from the interpreter
      matches = interp->getLastMatches();

      if (matches.size() == 0)
        {
          // Completion successful but nothing returned.
          _tab_mode = false;
          _cursor_pos = -1;
          return;
        }

      // Only one match - complete directly and update doc string window
      doc = interp->getDocStr();
      if (matches.size() == 1)
        {
          first_match = matches[0].mid(_compl_after_point.size());
          cursor.insertText(first_match);
          _tab_mode = false;
          if (doc == QString(""))
            emit updateDoc(formatDocHTML("(no documentation available)\n"));
          else
            emit updateDoc(formatDocHTML(doc));
        }
      else
        {
          // Detect if there is a common base to all available completion
          // In this case append this base to the text already
          extractCommon(matches, base);
          first_match = base.mid(_compl_after_point.size());
          cursor.insertText(first_match);

          // If this happens to match exaclty the first completion
          // also provide doc
          if (base == matches[0])
            {
              doc = formatDocHTML(doc);
              emit updateDoc(doc);
            }

          // Print all matching completion in a "undo-able" block
          cursorPos = cursor.position();
          cursor.insertBlock();
          cursor.beginEditBlock();

          // Insert all matches
          QTextCharFormat cf;
          cf.setForeground(QBrush(Qt::darkGreen));
          cursor.setCharFormat(cf);
          formatCompletion(matches, comple_text);
          cursor.insertText(comple_text);
          cursor.endEditBlock();

          // Position cursor where it was before inserting the completion list:
          cursor.setPosition(cursorPos);
          setTextCursor(cursor);
        }
      break;
    case PyInterp_Event::ES_TAB_COMPLETE_ERR:
      // Tab completion was unsuccessful, switch off mode:
      _tab_mode = false;
      _cursor_pos = -1;
      break;
    case PyInterp_Event::ES_OK:
    case PyInterp_Event::ES_ERROR:
    case PyInterp_Event::ES_INCOMPLETE:
      // Before everything else, call super()
      PyConsole_Editor::customEvent(event);
      // If we are in multi_paste_mode, process the next item:
      multiLineProcessNextLine();
      break;
    default:
      PyConsole_Editor::customEvent( event );
      break;
  }
}

/**
 * Extract the common leading part of all strings in matches.
 * @param matches
 * @param result
 */
void PyConsole_EnhEditor::extractCommon(const std::vector<QString> & matches, QString & result) const
{
  result = "";
  int charIdx = 0;

  if (matches.size() < 2)
    return;

  while (true)
    {
      if (charIdx >= matches[0].size())
        return;
      QChar ch = matches[0][charIdx];
      for (int j = 1; j < matches.size(); j++)
        if (charIdx >= matches[j].size() || matches[j][charIdx] != ch)
          return;
      result += ch;
      charIdx++;
    }
}

/**
 * Format the doc string in HTML format with the first line in bold blue
 * @param doc initial doc string
 * @return HTML string
 */
QString PyConsole_EnhEditor::formatDocHTML(const QString & doc) const
{
  QString templ = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" ") +
      QString(" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n ") +
      QString("<html><head><meta name=\"qrichtext\" content=\"1\" /> ") +
      QString("<style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style> ") +
      QString("</head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n") +
      QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">") +
      QString("<span style=\" font-weight:600; color:#0000ff;\">%1</span></p>") +
      QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%2</p>") +
      QString("</body></html>");

  QString fst, rest("");

  // Extract first line of doc
  int idx = doc.indexOf("\n");
  if (idx > 0)
    {
      fst = doc.left(idx);
      rest = doc.mid(idx+1);
    }
  else
    {
      fst = doc;
    }

  fst = fst.replace("\n", " ");
  rest = rest.replace("\n", " ");
  return templ.arg(fst).arg(rest);
}

/**
 * Handle properly multi-line pasting. Qt4 doc recommends overriding this function.
 * If the pasted text doesn't contain a line return, no special treatment is done.
 * @param source
 */
void PyConsole_EnhEditor::insertFromMimeData(const QMimeData * source)
{
  if (_multi_line_paste)
    return;

  if (source->hasText())
    {
      QString s = source->text();
      if (s.contains("\n"))
        multilinePaste(s);
      else
        PyConsole_Editor::insertFromMimeData(source);
    }
  else
    {
      PyConsole_Editor::insertFromMimeData(source);
    }
}


void PyConsole_EnhEditor::multilinePaste(const QString & s)
{
  // Turn on multi line pasting mode
  _multi_line_paste = true;

  // Split the string:
  QString s2 = s;
  s2.replace("\r", ""); // Windows string format converted to Unix style

  QStringList lst = s2.split(QChar('\n'), QString::KeepEmptyParts);

  // Perform the proper paste operation for the first line to handle the case where
  // sth was already there:
  QMimeData source;
  source.setText(lst[0]);
  PyConsole_Editor::insertFromMimeData(&source);

  // Prepare what will have to be executed after the first line:
  _multi_line_content = std::queue<QString>();
  for (int i = 1; i < lst.size(); ++i)
    _multi_line_content.push(lst[i]);

  // Trigger the execution of the first (mixed) line
  handleReturn();

  // See customEvent() and multiLineProcessNext() for the rest of the handling.
}

/**
 * Process the next line in the queue of a multiple copy/paste:
 */
void PyConsole_EnhEditor::multiLineProcessNextLine()
{
  if (!_multi_line_paste)
    return;

  QString line(_multi_line_content.front());
  _multi_line_content.pop();
  if (!_multi_line_content.size())
    {
      // last line in the queue, just paste it
      addText(line, false, false);
      _multi_line_paste = false;
    }
  else
    {
      // paste the line and simulate a <RETURN> key stroke
      addText(line, false, false);
      handleReturn();
    }
}
