// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Shell.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef BuildAPI_Shell_H_
#define BuildAPI_Shell_H_

#include "BuildAPI.h"

#include <BuildPlugin_Shell.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Shell
/// \ingroup CPPHighAPI
/// \brief Interface for Shell feature.
class BuildAPI_Shell: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Shell(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Shell(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Shell();

  INTERFACE_1(BuildPlugin_Shell::ID(),
              baseObjects, BuildPlugin_Shell::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Shell object.
typedef std::shared_ptr<BuildAPI_Shell> ShellPtr;

/// \ingroup CPPHighAPI
/// \brief Create Shell feature.
BUILDAPI_EXPORT
ShellPtr addShell(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_Shell_H_
