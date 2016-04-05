// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeSketch.h
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_CompositeSketch_H_
#define FeaturesPlugin_CompositeSketch_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_CompositeFeature.h>

#include <ModelAPI_ResultBody.h>

/// \class FeaturesPlugin_CompositeSketch
/// \ingroup Plugins
/// \brief Interface for the composite sketch feature.
class FeaturesPlugin_CompositeSketch : public ModelAPI_CompositeFeature
{
public:
  /// Attribute name of sketch feature.
  inline static const std::string& SKETCH_ID()
  {
    static const std::string MY_SKETCH_ID("sketch");
    return MY_SKETCH_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base");
    return MY_BASE_OBJECTS_ID;
  }

  /// Appends a feature to the sketch sub-elements container.
  FEATURESPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// \return the number of sub-elements.
  FEATURESPLUGIN_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// \return the sub-feature by zero-base index.
  FEATURESPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex, bool forTree = false);

  /// \return the sub-feature unique identifier in this composite feature by zero-base index.
  FEATURESPLUGIN_EXPORT virtual int subFeatureId(const int theIndex) const;

  /// \return true if feature or result belong to this composite feature as subs.
  FEATURESPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after).
  FEATURESPLUGIN_EXPORT virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Removes also all sub-sketch.
  FEATURESPLUGIN_EXPORT virtual void erase();

protected:
  enum InitFlags {
    InitSketchLauncher   = 1 << 0,
    InitBaseObjectsList  = 1 << 1
  };

  FeaturesPlugin_CompositeSketch(){};

  /// Initializes composite sketch attributes.
  void initCompositeSketchAttribtues(const int theInitFlags);

  /// \brief Returns list of base shapes.
  /// \param[out] theBaseShapesList list of base shapes (warning: list not cleared).
  /// \param[in] theIsMakeShells if true make shells from faces with shared edges.
  void getBaseShapes(ListOfShape& theBaseShapesList, const bool theIsMakeShells = true);

  /// Checks make shape algo.
  bool isMakeShapeValid(const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);

  /// Stores result of generation.
  void storeResult(const GeomShapePtr theBaseShape,
                   const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                   const int theIndex = 0);

  /// Stores generation history.
  void storeGenerationHistory(ResultBodyPtr theResultBody,
                              const GeomShapePtr theBaseShape,
                              const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                              int& theTag);

  /// Used to store from and to shapes.
  void storeShapes(ResultBodyPtr theResultBody,
                   const GeomAPI_Shape::ShapeType theBaseShapeType,
                   const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theMapOfSubShapes,
                   const ListOfShape& theShapes,
                   const std::string theName,
                   int& theTag);

};

#endif
