// File:        ModuleBase_WidgetValueFeature.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetValueFeature_H
#define ModuleBase_WidgetValueFeature_H

#include <ModuleBase.h>
#include <ModuleBase_WidgetValue.h>

#include <ModelAPI_Result.h>

#include <boost/shared_ptr.hpp>

class GeomAPI_Pnt2d;

/**\class ModuleBase_WidgetValueFeature
 * \ingroup GUI
 * \brief Custom widget value. The widget contains a feature and 2D point.
 */
class MODULEBASE_EXPORT ModuleBase_WidgetValueFeature : public ModuleBase_WidgetValue
{
public:
  /// Constructor
  ModuleBase_WidgetValueFeature();
  /// Destructor
  virtual ~ModuleBase_WidgetValueFeature();

  /// Fill the widget values by given point
  /// \param thePoint the point
  void setObject(const ObjectPtr& theFeature);

  /// Returns the widget values by given point
  /// \return theFeature the current feature
  const ObjectPtr& object() const;

  /// Fill the widget values by given point
  /// \param thePoint the point
  void setPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Returns the widget point
  /// \return the current point
  const boost::shared_ptr<GeomAPI_Pnt2d>& point() const;

private:
  ObjectPtr myResult;
  boost::shared_ptr<GeomAPI_Pnt2d> myPoint;
};

#endif
