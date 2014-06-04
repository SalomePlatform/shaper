// File:        PartSet_Presentation.h
// Created:     02 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Presentation_H
#define PartSet_Presentation_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>

#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;
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
                                              boost::shared_ptr<ModelAPI_Feature> theFeature,
                                              boost::shared_ptr<ModelAPI_Feature> theSketch,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePrevPrs);
protected:
  static Handle_AIS_InteractiveObject createFeature(
                                              boost::shared_ptr<ModelAPI_Feature> theFeature,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePrevPrs);

  static Handle_AIS_InteractiveObject createSketchConstraintLength(
                                              boost::shared_ptr<ModelAPI_Feature> theFeature,
                                              boost::shared_ptr<ModelAPI_Feature> theSketch,
                                              Handle_AIS_InteractiveObject thePrevPrs);
};

#endif
