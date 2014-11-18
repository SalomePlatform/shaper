// File:        ModuleBase_WidgetValueFeature.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetValueFeature_H
#define ModuleBase_WidgetValueFeature_H

#include <ModuleBase.h>
#include <ModuleBase_WidgetValue.h>

#include <ModelAPI_Result.h>

#include <memory>

class GeomAPI_Pnt2d;

/**\class ModuleBase_WidgetValueFeature
 * \ingroup GUI
 * \brief Custom widget value. The widget contains a feature and 2D point.
 */
class ModuleBase_WidgetValueFeature : public ModuleBase_WidgetValue
{
 public:
  /// Constructor
  MODULEBASE_EXPORT ModuleBase_WidgetValueFeature();
  /// Destructor
  MODULEBASE_EXPORT virtual ~ModuleBase_WidgetValueFeature();

  /// Fill the widget values by given point
  /// \param thePoint the point
  MODULEBASE_EXPORT void setObject(const ObjectPtr& theFeature);

  /// Returns the widget values by given point
  /// \return theFeature the current feature
  MODULEBASE_EXPORT const ObjectPtr& object() const;

  /// Fill the widget values by given point
  /// \param thePoint the point
  MODULEBASE_EXPORT void setPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Returns the widget point
  /// \return the current point
  MODULEBASE_EXPORT const std::shared_ptr<GeomAPI_Pnt2d>& point() const;

 private:
  ObjectPtr myResult;
  std::shared_ptr<GeomAPI_Pnt2d> myPoint;
};

#endif
