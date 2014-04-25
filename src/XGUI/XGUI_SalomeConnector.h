#ifndef XGUI_SALOMECONNECTOR_H
#define XGUI_SALOMECONNECTOR_H

#include "XGUI.h"

#include <QString>

class QMainWindow;

class XGUI_EXPORT XGUI_SalomeConnector
{
public:
  virtual void addFeature(const QString& theWBName,
                          const QString& theId, 
                          const QString& theTitle, 
                          const QString& theTip,
                          const QIcon& theIcon, 
                          bool isCheckable,
                          QObject* reciever,
                          const char* member,
                          const QKeySequence& theKeys) = 0;

  virtual void addEditCommand(const QString& theId,
                              const QString& theTitle,
                              const QString& theTip,
                              const QIcon& theIcon, 
                              bool isCheckable,
                              QObject* reciever,
                              const char* member,
                              const QKeySequence& theKeys) = 0;
  virtual void addEditMenuSeparator() = 0;

  virtual QMainWindow* desktop() const = 0;

  virtual QString commandId(const QAction* theCmd) const = 0;
  virtual QAction* command(const QString& theId) const = 0;
};

#endif