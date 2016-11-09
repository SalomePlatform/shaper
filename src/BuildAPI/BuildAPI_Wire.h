// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Wire.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef BuildAPI_Wire_H_
#define BuildAPI_Wire_H_

#include "BuildAPI.h"

#include <BuildPlugin_Wire.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Wire
/// \ingroup CPPHighAPI
/// \brief Interface for Wire feature.
class BuildAPI_Wire: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Wire();

  INTERFACE_1(BuildPlugin_Wire::ID(),
              baseObjects, BuildPlugin_Wire::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Adds closed contour.
  BUILDAPI_EXPORT
  void addContour();

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Wire object.
typedef std::shared_ptr<BuildAPI_Wire> WirePtr;

/// \ingroup CPPHighAPI
/// \brief Create Wire feature.
BUILDAPI_EXPORT
WirePtr addWire(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_Wire_H_
