
#ifndef PYCONSOLE_EDITOR_H
#define PYCONSOLE_EDITOR_H

#include "PyConsole.h"

#include <QTextEdit>

class PyConsole_Interp;
class PyInterp_Request;
class QEventLoop;

class PYCONSOLE_EXPORT PyConsole_Editor : public QTextEdit
{
  Q_OBJECT;

public:
  PyConsole_Editor( PyConsole_Interp* theInterp, QWidget *theParent = 0 );
  ~PyConsole_Editor();
  
  virtual void   addText( const QString& str, const bool newBlock = false, const bool isError = false );
  bool           isCommand( const QString& str ) const;

  virtual void   exec( const QString& command );
  void           execAndWait( const QString& command );

  bool           isSync() const;
  void           setIsSync( const bool );

  bool           isSuppressOutput() const;
  void           setIsSuppressOutput(const bool);

  bool           isShowBanner() const;
  void           setIsShowBanner( const bool );

  virtual QSize  sizeHint() const;

public slots:
    void           cut();
    void           paste();
    void           clear();
    void           handleReturn();
    void           onPyInterpChanged( PyConsole_Interp* );
    //.void           dump();

protected:
  virtual void   dropEvent( QDropEvent* event );
  virtual void   mouseReleaseEvent( QMouseEvent* event );
  virtual void   keyPressEvent ( QKeyEvent* event);
  virtual void   customEvent( QEvent* event);

  virtual PyInterp_Request* createRequest( const QString& );

  /** Convenience function */
  inline int promptSize() const { return myPrompt.size(); }

  PyConsole_Interp* myInterp;           //!< python interpreter

  QString           myCommandBuffer;    //!< python command buffer
  QString           myCurrentCommand;   //!< currently being printed command
  QString           myPrompt;           //!< current command line prompt
  int               myCmdInHistory;     //!< current history command index
  QStringList       myHistory;          //!< commands history buffer
  QEventLoop*       myEventLoop;        //!< internal event loop
  QString           myBanner;           //!< current banner
  bool              myShowBanner;       //!< 'show banner' flag
  QStringList       myQueue;            //!< python commands queue
  bool              myIsSync;           //!< synchronous mode flag
  bool              myIsSuppressOutput; //!< suppress output flag
};

#endif // PYCONSOLE_EDITOR_H
