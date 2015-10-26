// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeSketch.h
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_CompositeSketch_H_
#define FeaturesPlugin_CompositeSketch_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_CompositeFeature.h>

#include <GeomAlgoAPI_Boolean.h>

/** \class FeaturesPlugin_CompositeSketch
 *  \ingroup Plugins
 *  \brief Interface for the composite sketch feature.
 */
class FeaturesPlugin_CompositeSketch : public ModelAPI_CompositeFeature
{
 public:
  /// Attribute name of sketch feature.
  inline static const std::string& SKETCH_OBJECT_ID()
  {
    static const std::string MY_SKETCH_OBJECT_ID("sketch");
    return MY_SKETCH_OBJECT_ID;
  }

  /// Attribute name of sketch feature.
  inline static const std::string& SKETCH_SELECTION_ID()
  {
    static const std::string MY_SKETCH_SELECTION_ID("sketch_selection");
    return MY_SKETCH_SELECTION_ID;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

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
  /// structures of the owner (the remove from the document will be done outside just after)
  FEATURESPLUGIN_EXPORT virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// removes also all sub-sketch
  FEATURESPLUGIN_EXPORT virtual void erase();

protected:
  FeaturesPlugin_CompositeSketch(){};

  /// Define this function to init attributes for extrusion/revolution.
  virtual void initMakeSolidsAttributes() = 0;

  /// Define this function to create solids from faces with extrusion/revolution.
  virtual void makeSolid(const std::shared_ptr<GeomAPI_Shape> theFace,
                         std::shared_ptr<GeomAPI_Shape>& theResult,
                         ListOfShape& theFromFaces,
                         ListOfShape& theToFaces,
                         std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape,
                         std::shared_ptr<GeomAPI_DataMapOfShapeShape>& theDataMap) = 0;

  /// Results naming.
  void loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                    const std::shared_ptr<GeomAPI_Shape>& theResult,
                    const ListOfShape& theFromFaces,
                    const ListOfShape& theToFaces,
                    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape,
                    const std::shared_ptr<GeomAPI_DataMapOfShapeShape>& theDataMap);
};

#endif
