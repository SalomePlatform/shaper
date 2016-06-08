// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Group.h
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Group_H_
#define FeaturesAPI_Group_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Group.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class FeaturesAPI_Group
/// \ingroup CPPHighAPI
/// \brief Interface for Group feature.
class FeaturesAPI_Group: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Group();

  INTERFACE_1(FeaturesPlugin_Group::ID(),
              groupList, FeaturesPlugin_Group::LIST_ID()(), ModelAPI_AttributeSelectionList, /** Group list*/)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setGroupList(const std::list<ModelHighAPI_Selection>& theGroupList);
};

/// Pointer on Group object.
typedef std::shared_ptr<FeaturesAPI_Group> GroupPtr;

/// \ingroup CPPHighAPI
/// \brief Create Group feature.
FEATURESAPI_EXPORT
GroupPtr addGroup(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theGroupList);

#endif // FeaturesAPI_Group_H_
