// File:        PartSet_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#ifndef PartSet_WidgetPoint2dDistance_H
#define PartSet_WidgetPoint2dDistance_H

#include "PartSet.h"
#include <ModuleBase_WidgetDoubleValue.h>

#include <ModelAPI_CompositeFeature.h>

class GeomAPI_Pnt2d;
class XGUI_Workshop;
class ModuleBase_IViewWindow;
class QMouseEvent;

class PARTSET_EXPORT PartSet_WidgetPoint2dDistance : public ModuleBase_WidgetDoubleValue
{
Q_OBJECT
 public:
  /// Constructor
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetPoint2dDistance(QWidget* theParent, const Config_WidgetAPI* theData,
                                   const std::string& theParentId);

  virtual ~PartSet_WidgetPoint2dDistance();

  /// The methiod called when widget is activated
  virtual void activate();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  //virtual bool setValue(ModuleBase_WidgetValue* theValue);

  XGUI_Workshop* workshop() const { return myWorkshop; }

  void setWorkshop(XGUI_Workshop* theWork) { myWorkshop = theWork; }

  /// \returns the sketch instance
  CompositeFeaturePtr sketch() const { return mySketch; }
  void setSketch(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

 protected slots:
  void onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);
  void onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

protected:
  /// Set the second point which defines a value in the widget as a distance with a first point defined by feature
  void setPoint(FeaturePtr theFeature, const std::shared_ptr<GeomAPI_Pnt2d>& thePnt);

 private:
  XGUI_Workshop* myWorkshop;
  std::string myFirstPntName;

  CompositeFeaturePtr mySketch;
};

#endif
