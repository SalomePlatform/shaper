// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Partition.h
// Created:     31 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef FeaturesPlugin_Partition_H_
#define FeaturesPlugin_Partition_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Partition.h>

/// \class FeaturesPlugin_Partition
/// \ingroup Plugins
/// \brief Feature for applying of Partition operations on Shapes. Partition makes conjunctional
/// faces of solids as shared. The result of partitions is a compsolid.
class FeaturesPlugin_Partition : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Partition");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Partition::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Partition();

private:
  /// Stores result of generation.
  void storeResult(ListOfShape& theObjects,
                   ListOfShape& thePlanes,
                   const GeomShapePtr theResultShape,
                   const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                   const int theIndex = 0);
};

#endif
