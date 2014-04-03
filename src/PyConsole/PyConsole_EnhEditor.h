

#ifndef PYCONSOLE_ENHEDITOR_H_
#define PYCONSOLE_ENHEDITOR_H_

#include "PyConsole.h"

#include "PyConsole_Editor.h"
#include <QObject>
#include <queue>

class PyConsole_EnhInterp;

/**
 * Enhanced Python editor handling tab completion.
 */
class PYCONSOLE_EXPORT PyConsole_EnhEditor: public PyConsole_Editor
{
  Q_OBJECT;

public:
  PyConsole_EnhEditor(PyConsole_EnhInterp * interp, QWidget * parent=0);
  virtual ~PyConsole_EnhEditor() {}

signals:
  /**
   * Signal emitted by the editor widget when the doc string should be updated.
   * @param doc a HTML block with the formatted doc string.
   * TODO: for now this signal is left uncaught.
   */
  void updateDoc(QString doc);

protected:
  /** List of separators identifying the last parsable token for completion */
  static const std::vector<QString> SEPARATORS;

  /** Maximum number of completions shown at once */
  static const int MAX_COMPLETIONS = 70;

  /** Are we in completion mode */
  bool _tab_mode;

  /** String on which the dir() comamnd is executed */
  QString _compl_before_point;
  /** String on which the results of the dir() are matched */
  QString _compl_after_point;

  /** Cursor position when <TAB> is hit */
  int _cursor_pos;

  /** Are we currently pasting several lines */
  bool _multi_line_paste;

  /** Queue of lines being pasted */
  std::queue<QString> _multi_line_content;

  // Overrides:
  virtual void   keyPressEvent ( QKeyEvent* event);
  virtual void   customEvent( QEvent* event);
  virtual void   mousePressEvent( QMouseEvent* event );
  virtual void   insertFromMimeData(const QMimeData * source);

  virtual PyInterp_Request* createTabRequest( const QString& input );
  virtual void handleTab();
  virtual void handleBackTab();
  virtual void clearCompletion();
  virtual void formatCompletion(const std::vector<QString> & matches, QString & result) const;
  virtual QString formatDocHTML(const QString & doc) const;

  virtual void multilinePaste(const QString & s);
  virtual void multiLineProcessNextLine();

private:
  void extractCommon(const std::vector<QString> & matches, QString & result) const;

};

#endif /* PYCONSOLE_ENHEDITOR_H_ */
