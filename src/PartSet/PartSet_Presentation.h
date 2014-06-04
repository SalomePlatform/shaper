// File:        PartSet_Presentation.h
// Created:     02 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Presentation_H
#define PartSet_Presentation_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>

#include <ModelAPI_Feature.h>

#include <boost/shared_ptr.hpp>

class Handle_AIS_InteractiveObject;

/*!
 \class PartSet_Presentation
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_Presentation
{
public:
  /// Creates AIS presentation for the given feature
  /// \param theFeature a feature
  /// \return the presentation
  static Handle_AIS_InteractiveObject createPresentation(
                                              FeaturePtr theFeature,
                                              FeaturePtr theSketch,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePrevPrs);
protected:
  static Handle_AIS_InteractiveObject createFeature(
                                              FeaturePtr theFeature,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePrevPrs);

  static Handle_AIS_InteractiveObject createSketchConstraintLength(
                                              FeaturePtr theFeature,
                                              FeaturePtr theSketch,
                                              Handle_AIS_InteractiveObject thePrevPrs);
};

#endif
