// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Wire.h
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_Wire_H_
#define BuildPlugin_Wire_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Wire
/// \ingroup Plugins
/// \brief Feature for creation of wire from sketch edges or existing wires.
class BuildPlugin_Wire: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Wire();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Wire");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_Wire::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();

  /// Performs some functionality by action id.
  /// \param[in] theAttributeId action key id.
  /// \return false in case if action not perfomed.
  BUILDPLUGIN_EXPORT virtual bool customAction(const std::string& theActionId);

private:
  /// Action: Adds to the list of segments other segments of the sketcher connected to
  /// the already selected ones to create a closed contour.
  /// \return false in case if no countours have been added.
  bool addContour();
};

#endif
