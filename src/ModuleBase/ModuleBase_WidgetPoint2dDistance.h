// File:        ModuleBase_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov


#ifndef ModuleBase_WidgetPoint2dDistance_H
#define ModuleBase_WidgetPoint2dDistance_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetDoubleValue.h"

class ModuleBase_WidgetValue;
class GeomAPI_Pnt2d;

class MODULEBASE_EXPORT ModuleBase_WidgetPoint2dDistance: public ModuleBase_WidgetDoubleValue
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetPoint2dDistance(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetPoint2dDistance();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setValue(ModuleBase_WidgetValue* theValue);

protected:
  /// Set the second point which defines a value in the widget as a distance with a first point defined by feature
  void setPoint(FeaturePtr theFeature, const boost::shared_ptr<GeomAPI_Pnt2d>& thePnt);

private:
  std::string myFirstPntName;
};


#endif