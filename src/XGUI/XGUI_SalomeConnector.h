#ifndef XGUI_SALOMECONNECTOR_H
#define XGUI_SALOMECONNECTOR_H

#include "XGUI.h"

#include <QString>
#include <QStringList>

class QMainWindow;
class XGUI_SalomeViewer;

/**
* An interface which provides a connection of XGUI functionality 
* with functionality of SALOME module interface.
*/
class XGUI_EXPORT XGUI_SalomeConnector
{
public:
  //! Creates a feature (command) in SALOME desktop
  //! \param theWBName - a workbench name
  //! \param theId - an id of the feature
  //! \param theTitle - a menu item string
  //! \param theTip - a tooltip string (help)
  //! \param theIcon - icon
  //! \param isCheckable - is checkable or not
  //! \param reciever - QObject which will react on the command call
  //! \param member - a method of receiver which will be called on the command
  //! \param theKeys - hot keys
  virtual void addFeature(const QString& theWBName,
                          const QString& theId, 
                          const QString& theTitle, 
                          const QString& theTip,
                          const QIcon& theIcon, 
                          bool isCheckable,
                          QObject* reciever,
                          const char* member,
                          const QKeySequence& theKeys) = 0;

  //! Creates a command in Edit menu of SALOME desktop
  //! \param theId - an id of the feature
  //! \param theTitle - a menu item string
  //! \param theTip - a tooltip string (help)
  //! \param theIcon - icon
  //! \param isCheckable - is checkable or not
  //! \param reciever - QObject which will react on the command call
  //! \param member - a method of receiver which will be called on the command
  //! \param theKeys - hot keys
  virtual void addEditCommand(const QString& theId,
                              const QString& theTitle,
                              const QString& theTip,
                              const QIcon& theIcon, 
                              bool isCheckable,
                              QObject* reciever,
                              const char* member,
                              const QKeySequence& theKeys) = 0;

  //! Insert separator into Edit menu of SALOME desktop
  virtual void addEditMenuSeparator() = 0;

  //! Returns desktop window of SALOME
  virtual QMainWindow* desktop() const = 0;

  //! Returns command string Id by QAction instance
  virtual QString commandId(const QAction* theCmd) const = 0;

  //! Returns QAction instance by command string Id
  virtual QAction* command(const QString& theId) const = 0;

  //! Set nested actions dependent on command Id
  //! \param theId - the command ID
  //! \param theActions - the list of nested actions
  virtual void setNestedActions(const QString& theId, const QStringList& theActions) = 0;

  //! Returns list of nested actions according to the given command ID
  virtual QStringList nestedActions(const QString& theId) const = 0;

  //! Returns interface to Salome viewer
  virtual XGUI_SalomeViewer* viewer() const = 0;
};

#endif