#ifndef XGUI_Workbench_H
#define XGUI_Workbench_H

#include <QWidget>
#include <QScrollArea>
#include <QList>

class XGUI_Command;
class XGUI_MenuGroupPanel;
class CommandsArea;

class QHBoxLayout;
class QPushButton;

class XGUI_Workbench: public QWidget
{
Q_OBJECT
public:
  XGUI_Workbench(QWidget* theParent);

  XGUI_MenuGroupPanel* addGroup(const QString& theId);
  XGUI_MenuGroupPanel* findGroup(const QString& theName);

private slots:
  void onLeftScroll();
  void onRightScroll();

protected:
  virtual void resizeEvent(QResizeEvent * theEvent);
  virtual bool eventFilter(QObject *theObj, QEvent *theEvent);

private:
  void addSeparator();
  bool isExceedsLeft();
  bool isExceedsRight();

  QWidget* myChildWidget;
  QHBoxLayout* myLayout;
  QList<XGUI_MenuGroupPanel*> myGroups;

  CommandsArea* myCommandsArea;
  QPushButton* myRightButton;
  QPushButton* myLeftButton;
};

#endif;
