// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_SubShapes.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef BuildAPI_SubShapes_H_
#define BuildAPI_SubShapes_H_

#include "BuildAPI.h"

#include <BuildPlugin_SubShapes.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_SubShapes
/// \ingroup CPPHighAPI
/// \brief Interface for SubShapes feature.
class BuildAPI_SubShapes: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const ModelHighAPI_Selection& theBaseShape,
                              const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_SubShapes();

  INTERFACE_2(BuildPlugin_SubShapes::ID(),
              baseShape, BuildPlugin_SubShapes::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base shape */,
              subShapes, BuildPlugin_SubShapes::SUBSHAPES_ID(),
              ModelAPI_AttributeSelectionList, /** Sub-shapes */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBaseShape(const ModelHighAPI_Selection& theBaseShape);

  /// Modify sub-shapes attribute of the feature.
  BUILDAPI_EXPORT
  void setSubShapes(const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on SubShapes object.
typedef std::shared_ptr<BuildAPI_SubShapes> SubShapesPtr;

/// \ingroup CPPHighAPI
/// \brief Create SubShapes feature.
BUILDAPI_EXPORT
SubShapesPtr addSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Selection& theBaseShape,
                          const std::list<ModelHighAPI_Selection>& theSubShapes);

#endif // BuildAPI_SubShapes_H_
