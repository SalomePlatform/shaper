#ifndef XGUI_MenuGroupPanel_H
#define XGUI_MenuGroupPanel_H

#include <QWidget>
#include <QMap>

class XGUI_Command;
class QGridLayout;

/**\class XGUI_MenuGroupPanel
 * \ingroup GUI
 * \brief Represents a one group in a page of main menu (workbench)
 */
class XGUI_MenuGroupPanel: public QWidget
{
Q_OBJECT
public:
  explicit XGUI_MenuGroupPanel(QWidget *parent = 0);

  //! Adding a new feature (Command) in the group
  XGUI_Command* addFeature(const QString& theId, const QString& theTitle, const QString& theTip,
                           const QIcon& theIcon, const QKeySequence& theKeys = QKeySequence());

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
