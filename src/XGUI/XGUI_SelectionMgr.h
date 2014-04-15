#ifndef XGUI_SelectionMgr_H
#define XGUI_SelectionMgr_H

#include "XGUI.h"
#include "XGUI_Constants.h"
#include <QObject>

class XGUI_Workshop;
class XGUI_ObjectsBrowser;

/**\class XGUI_SelectionMgr
 * \ingroup GUI
 * \brief Selection manager. Provides selection event on selection in 
 * Object Browser and Viewer
 */
class XGUI_EXPORT XGUI_SelectionMgr : public QObject
{
  Q_OBJECT
public:
  XGUI_SelectionMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_SelectionMgr();

  //! Returns list of currently selected objects
  QFeatureList selectedData() const { return mySelectedData; }

  void connectObjectBrowser(XGUI_ObjectsBrowser* theOB);

signals:
  //! Emited when selection in a one of viewers was changed
  void selectionChanged();

public slots:
  void onSelectionChanged();

private:
  XGUI_Workshop* myWorkshop;
  XGUI_ObjectsBrowser* myObjectBrowser;

  //! List of selected features
  QFeatureList mySelectedData;
};

#endif
