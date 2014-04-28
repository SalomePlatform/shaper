#ifndef XGUI_SelectionMgr_H
#define XGUI_SelectionMgr_H

#include "XGUI.h"
#include "XGUI_Constants.h"
#include <QObject>
#include <QModelIndexList>

#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

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

  //! Returns list of currently selected data objects
  QFeatureList selectedFeatures() const; 
  
  //! Returns list of currently selected QModelIndexes
  QModelIndexList selectedIndexes() const;

  //! Returns list of currently selected AIS objects
  void selectedAISObjects(AIS_ListOfInteractive& theList) const;

  //! Returns list of currently selected shapes
  void selectedShapes(NCollection_List<TopoDS_Shape>& theList) const;

  //! Connects the manager to all viewers accessible by Workshop
  void connectViewers();

signals:
  //! Emited when selection in a one of viewers was changed
  void selectionChanged();

private slots:
  void onObjectBrowserSelection();
  void onViewerSelection();

private:

  XGUI_Workshop* myWorkshop;
};

#endif
