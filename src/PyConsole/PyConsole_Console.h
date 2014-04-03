
#ifndef PYCONSOLE_CONSOLE_H
#define PYCONSOLE_CONSOLE_H

#include "PyConsole.h"

//#include <SUIT_PopupClient.h>
#include <QWidget>
#include <QMap>

class PyConsole_Interp;
class PyConsole_Editor;
class PyConsole_EnhInterp;
class QMenu;

class PYCONSOLE_EXPORT PyConsole_Console : public QWidget//, public SUIT_PopupClient
{
  Q_OBJECT

public:
  //! Context popup menu actions flags
  enum
  {
    CopyId         = 0x01,                                           //!< "Copy" menu action
    PasteId        = 0x02,                                           //!< "Paste" menu action
    ClearId        = 0x04,                                           //!< "Clear" menu action
    SelectAllId    = 0x08,                                           //!< "Select All" menu action
    DumpCommandsId = 0x16,                                           //!< "DumpCommands" menu action
    All = CopyId | PasteId | ClearId | SelectAllId | DumpCommandsId  //!< all menu actions
  };

public:
  PyConsole_Console( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~PyConsole_Console();

  //! \brief Get python interperter
  PyConsole_Interp*   getInterp() { return myInterp; } 
  QFont               font() const;
  virtual void        setFont( const QFont& );

  bool                isSync() const;
  void                setIsSync( const bool );

  bool                isSuppressOutput() const;
  void                setIsSuppressOutput( const bool );

  bool                isShowBanner() const;
  void                setIsShowBanner( const bool );

  void                exec( const QString& );
  void                execAndWait( const QString& );

  virtual bool        eventFilter( QObject*, QEvent* );

  //! \brief Get popup client symbolic name
  virtual QString     popupClientType() const { return QString( "PyConsole" ); }
  virtual void        contextMenuPopup( QMenu* );

  void                setMenuActions( const int );
  int                 menuActions() const;

protected:
  void                createActions();
  void                updateActions();

  PyConsole_Console( QWidget* parent, PyConsole_Interp*,  PyConsole_Editor*);


  PyConsole_Interp*   myInterp;    //!< python interpreter
  PyConsole_Editor*   myEditor;    //!< python console editor widget
  QMap<int, QAction*> myActions;   //!< menu actions list
};

/**
 * Enhance console object providing auto-completion.
 * Similar to PyConsole_Console except that an enhanced interpreter and enhanced editor
 * are encapsulated.
 */
class PYCONSOLE_EXPORT PyConsole_EnhConsole: public PyConsole_Console
{
  Q_OBJECT

public:
  PyConsole_EnhConsole( QWidget* parent, PyConsole_EnhInterp* interp = 0);
  virtual ~PyConsole_EnhConsole() {}
};

#endif // PYCONSOLE_CONSOLE_H
