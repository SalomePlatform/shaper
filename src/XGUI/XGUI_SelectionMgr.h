#ifndef XGUI_SelectionMgr_H
#define XGUI_SelectionMgr_H

#include "XGUI_Constants.h"
#include <QObject>

class XGUI_Workshop;

class XGUI_SelectionMgr : public QObject
{
  Q_OBJECT
public:
  XGUI_SelectionMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_SelectionMgr();

  QFeatureList selectedData() const { return mySelectedData; }


signals:
  void selectionChanged();

public slots:
  void onSelectionChanged();

private:
  XGUI_Workshop* myWorkshop;

  QFeatureList mySelectedData;
};

#endif;