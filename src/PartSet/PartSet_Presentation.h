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
  /// Creates AIS presentation for the given feature. It deals with features and
  /// constraints presentations
  /// \param theFeature a feature
  /// \param theSketch a feature sketch
  /// \param theShape a shape of the feature
  /// \param thePrevious a previous AIS object
  /// \return a created/changed AIS object with the feature parameters
  static Handle_AIS_InteractiveObject createPresentation(
                                              FeaturePtr theFeature,
                                              FeaturePtr theSketch,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePreviuos);
protected:
  /// Creates AIS presentation based on the given shape
  /// \param theFeature a feature
  /// \param theSketch a feature sketch
  /// \param theShape a shape of the feature
  /// \param thePrevious a previous AIS object
  /// \return a created/changed AIS object with the feature parameters
 static Handle_AIS_InteractiveObject createFeature(
                                              FeaturePtr theFeature,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePreviuos);
};

#endif
