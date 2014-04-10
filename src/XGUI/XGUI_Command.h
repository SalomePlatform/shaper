#ifndef XGUI_Command_H
#define XGUI_Command_H

#include <QWidgetAction>

#define MIN_BUTTON_HEIGHT 18
#define MIN_BUTTON_WIDTH 40

/**\class XGUI_Command
 * \ingroup GUI
 * \brief Represents a command item in the application menu (Workbench)
 */
class XGUI_Command: public QWidgetAction
{
Q_OBJECT
public:
  XGUI_Command(const QString& theId, QObject * parent);
  XGUI_Command(const QString& theId, const QIcon& icon, const QString& text, QObject* parent);
  ~XGUI_Command();

  //! Returns true if the command is enabled
  virtual bool enabled() const;

  //! Set the command enabled
  virtual void enable();

  //! Set the command disabled
  virtual void disable();

  //! Returns Id of the command
  virtual QString getId() const
  {
    return myId;
  }

  //! Connect the command to a slot
  virtual void connectTo(const QObject* theResiver, const char* theSlot);

protected:
  //! Creates a command representation widget dependently on parent widget type
  virtual QWidget* createWidget(QWidget* theParent);

private:
  QString myId;
};

#endif
