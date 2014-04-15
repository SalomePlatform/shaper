#ifndef XGUI_MenuGroupPanel_H
#define XGUI_MenuGroupPanel_H

#include "XGUI.h"
#include <QWidget>
#include <QMap>

class XGUI_Command;
class QGridLayout;

/**\class XGUI_MenuGroupPanel
 * \ingroup GUI
 * \brief Represents a one group in a page of main menu (workbench)
 */
class XGUI_EXPORT XGUI_MenuGroupPanel: public QWidget
{
Q_OBJECT
public:
  explicit XGUI_MenuGroupPanel(QWidget *parent = 0);

  //! Adding a new feature (Command) in the group
  XGUI_Command* addFeature(const QString& theId, const QString& theTitle, const QString& theTip,
                           const QIcon& theIcon, const QKeySequence& theKeys = QKeySequence());

  //! Returns already created command by its ID
  XGUI_Command* feature(const QString& theId) const;

  //! Returns list of created commands
  QList<XGUI_Command*> features() const { return myActions; }

protected:
  virtual void resizeEvent(QResizeEvent *theEvent);

private:
  void addWidget(QWidget* theWgt);
  void placeWidget(QWidget* theWgt);
  void addCommand(XGUI_Command* theAction);

  QList<XGUI_Command*> myActions;
  QWidgetList          myActionWidget;

  QGridLayout* myLayout;
  int myNewRow;
  int myNewCol;
  int myMaxRow;
};

#endif
