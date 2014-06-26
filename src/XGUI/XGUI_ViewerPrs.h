// File:        XGUI_ViewerPrs.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_ViewerPrs_H
#define XGUI_ViewerPrs_H

#include "XGUI.h"

#include <boost/shared_ptr.hpp>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <ModelAPI_Feature.h>

/**\class XGUI_ViewerPrs
 * \ingroup GUI
 * \brief Presentation. Provides container to have feature, shape and/or selection owner.
 */
class XGUI_EXPORT XGUI_ViewerPrs
{
public:
  /// Constructor
  XGUI_ViewerPrs();
  /// Constructor
  /// \param theFeature a model feature
  /// \param theShape a viewer shape
  /// \param theOwner a selection owner
  XGUI_ViewerPrs(FeaturePtr theFeature,
                 const TopoDS_Shape& theShape,
                 Handle_SelectMgr_EntityOwner theOwner);
  /// Destructor
  virtual ~XGUI_ViewerPrs();

  /// Sets the feature.
  /// \param theFeature a feature instance
  void setFeature(FeaturePtr theFeature);

  /// Returns the feature.
  /// \return a feature instance
  FeaturePtr feature() const;

  /// Returns the presentation owner
  /// \param the owner
  void setOwner(Handle_SelectMgr_EntityOwner theOwner);

  /// Returns the presentation owner
  /// \return an owner
  Handle_SelectMgr_EntityOwner owner() const;

  /// Sets the shape
  /// \param theShape a shape instance
  void setShape(const TopoDS_Shape& theShape);

  /// Returns the shape
  /// \return a shape instance
  const TopoDS_Shape& shape() const;

  bool operator==(const XGUI_ViewerPrs&);

private:
  FeaturePtr myFeature; /// the feature
  Handle(SelectMgr_EntityOwner) myOwner; /// the selection owner
  TopoDS_Shape myShape; /// the shape
};

#endif
