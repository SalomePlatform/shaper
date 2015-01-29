// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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

/**
* \ingroup Modules
* Implementation of model widget for widget which provides input of a distance between two points
* In XML file can be defined as following:
* \code
* <point2ddistance id="CircleRadius" 
*   first_point="CircleCenter" 
*   label="Radius:" 
*   min="0" 
*   step="1.0" 
*   default="0" 
*   icon=":icons/radius.png" 
*   tooltip="Set radius">
*   <validator id="GeomValidators_Positive"/>
* </point2ddistance>
* \endcode
*/ 
class PARTSET_EXPORT PartSet_WidgetPoint2dDistance : public ModuleBase_WidgetDoubleValue
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetPoint2dDistance(QWidget* theParent, const Config_WidgetAPI* theData,
                                   const std::string& theParentId);

  virtual ~PartSet_WidgetPoint2dDistance();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Returns workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

  /// Set workshop
  /// \param theWork a pointer to workshop
  void setWorkshop(XGUI_Workshop* theWork) { myWorkshop = theWork; }

  /// \returns the sketch instance
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// Set sketch instance
  void setSketch(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

 protected slots:
   /// Process of mouse release
   /// \param theWnd a pointer to a window
   /// \param theEvent a mouse event
  void onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

   /// Process of mouse move
   /// \param theWnd a pointer to a window
   /// \param theEvent a mouse event
  void onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

protected:
  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Set the second point which defines a value in the widget as a distance with a first point defined by feature
  void setPoint(FeaturePtr theFeature, const std::shared_ptr<GeomAPI_Pnt2d>& thePnt);

private slots:
  /// Process values changed event
  void onValuesChanged();

 private:
  XGUI_Workshop* myWorkshop;
  std::string myFirstPntName;

  CompositeFeaturePtr mySketch;
};

#endif
