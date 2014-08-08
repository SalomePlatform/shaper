#ifndef XGUI_Command_H
#define XGUI_Command_H

#include "XGUI.h"
#include <QWidgetAction>
#include <QPushButton>

#define MIN_BUTTON_HEIGHT 25
#define MIN_BUTTON_WIDTH 60

class XGUI_EXPORT XGUI_MenuButton: public QPushButton
{
  Q_OBJECT
 public:
  XGUI_MenuButton(const QIcon & icon, const QString & text, QWidget * parent = 0);

// protected:
//  virtual void enterEvent(QEvent * event);
//  virtual void leaveEvent(QEvent * event);

};

/**\class XGUI_Command
 * \ingroup GUI
 * \brief Represents a command item in the application menu (Workbench)
 */
class XGUI_EXPORT XGUI_Command: public QWidgetAction
{
Q_OBJECT
public:
  XGUI_Command(const QString& theId, QObject * parent, bool isCheckable = false);
  XGUI_Command(const QString& theId, const QIcon& icon, const QString& text, QObject* parent, bool isCheckable = false);
  ~XGUI_Command();

  //VSV: Don't use this method for compatibility with SALOME. Use the construction below
  /*virtual QString id() const
  {
    return data().toString();
  }*/

  const QStringList& nestedCommands() const;
  void setNestedCommands(const QStringList& myUnblockableCommands);

  //! Connect the command to a slot
  virtual void connectTo(const QObject* theResiver, const char* theSlot);

protected:
  //! Creates a command representation widget dependently on parent widget type
  virtual QWidget* createWidget(QWidget* theParent);

private:
  bool myCheckable;
  //! List of Ids of commands which WILL NOT be blocked when the command is on.
  QStringList myNestedCommands;
};

#endif
