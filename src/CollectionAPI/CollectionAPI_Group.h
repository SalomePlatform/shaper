// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionAPI_Group.h
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#ifndef CollectionAPI_Group_H_
#define CollectionAPI_Group_H_

#include "CollectionAPI.h"

#include <CollectionPlugin_Group.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class CollectionAPI_Group
/// \ingroup CPPHighAPI
/// \brief Interface for Group feature.
class CollectionAPI_Group: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  COLLECTIONAPI_EXPORT
  explicit CollectionAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  COLLECTIONAPI_EXPORT
  CollectionAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Destructor.
  COLLECTIONAPI_EXPORT
  virtual ~CollectionAPI_Group();

  INTERFACE_1(CollectionPlugin_Group::ID(),
              groupList, CollectionPlugin_Group::LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Group list*/)

  /// Set main objects.
  COLLECTIONAPI_EXPORT
  void setGroupList(const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Dump wrapped feature
  COLLECTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Group object.
typedef std::shared_ptr<CollectionAPI_Group> GroupPtr;

/// \ingroup CPPHighAPI
/// \brief Create Group feature.
COLLECTIONAPI_EXPORT
GroupPtr addGroup(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theGroupList);

#endif // CollectionAPI_Group_H_
