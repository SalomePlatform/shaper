#ifndef XGUI_Command_H
#define XGUI_Command_H

#include "XGUI.h"
#include <QWidgetAction>

#define MIN_BUTTON_HEIGHT 18
#define MIN_BUTTON_WIDTH 40

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

  //! Returns true if the command is enabled
  virtual bool enabled() const;

  //! Set the command enabled
  virtual void enable();

  //! Set the command disabled
  virtual void disable();

  //! Returns Id of the command
  virtual QString id() const
  {
    return myId;
  }

  const QStringList& unblockableCommands() const;
  void setUnblockableCommands(const QStringList& myUnblockableCommands);

  //! Connect the command to a slot
  virtual void connectTo(const QObject* theResiver, const char* theSlot);

protected:
  //! Creates a command representation widget dependently on parent widget type
  virtual QWidget* createWidget(QWidget* theParent);

private:
  QString myId;
  bool myCheckable;
  //! List of Ids of commands which WILL NOT be blocked when the command is on.
  QStringList myUnblockableCommands;
};

#endif
