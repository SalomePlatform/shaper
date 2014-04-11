#ifndef XGUI_SelectionMgr_H
#define XGUI_SelectionMgr_H

#include "XGUI_Constants.h"
#include <QObject>

class XGUI_Workshop;

/**\class XGUI_SelectionMgr
 * \ingroup GUI
 * \brief Selection manager. Provides selection event on selection in 
 * Object Browser and Viewer
 */
class XGUI_SelectionMgr : public QObject
{
  Q_OBJECT
public:
  XGUI_SelectionMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_SelectionMgr();

  //! Returns list of currently selected objects
  QFeatureList selectedData() const { return mySelectedData; }

signals:
  //! Emited when selection in a one of viewers was changed
  void selectionChanged();

public slots:
  void onSelectionChanged();

private:
  XGUI_Workshop* myWorkshop;

  //! List of selected features
  QFeatureList mySelectedData;
};

#endif
