#ifndef XGUI_Command_H
#define XGUI_Command_H

#include "XGUI.h"
#include <QWidgetAction>

#define MIN_BUTTON_HEIGHT 25
#define MIN_BUTTON_WIDTH 60

/**\class XGUI_Command
 * \ingroup GUI
 * \brief Represents a command item in the application menu (Workbench)
 */
class XGUI_EXPORT XGUI_Command : public QWidgetAction
{
Q_OBJECT
 public:
  XGUI_Command(const QString& theId, const QString& theDocumentKind,
               QObject * parent, bool isCheckable = false);
  XGUI_Command(const QString& theId, const QString& theDocumentKind,
               const QIcon& icon, const QString& text, QObject* parent,
               bool isCheckable = false);
  ~XGUI_Command();

  const QString& documentKind() const;
  const QStringList& nestedCommands() const;
  void setNestedCommands(const QStringList& myUnblockableCommands);

  //! Connect the command to a slot
  virtual void connectTo(const QObject* theResiver, const char* theSlot);

 protected:
  //! Creates a command representation widget dependently on parent widget type
  virtual QWidget* createWidget(QWidget* theParent);

 private:
  bool myCheckable;

  QString myDocumentKind;
  //! List of Ids of commands which WILL NOT be blocked when the command is on.
  QStringList myNestedCommands;
};

#endif
