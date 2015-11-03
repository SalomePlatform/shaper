// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_CompositeBoolean_H_
#define FeaturesPlugin_CompositeBoolean_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_CompositeFeature.h>

#include <GeomAlgoAPI_Boolean.h>

/** \class FeaturesPlugin_CompositeBoolean
 *  \ingroup Plugins
 *  \brief Interface for the composite boolean feature.
 */
class FeaturesPlugin_CompositeBoolean : public ModelAPI_CompositeFeature
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

  /// Attribute name of objects for boolean operation.
  inline static const std::string& BOOLEAN_OBJECTS_ID()
  {
    static const std::string MY_BOOLEAN_OBJECTS_ID("boolean_objects");
    return MY_BOOLEAN_OBJECTS_ID;
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

  /// \return true if feature or reuslt belong to this composite feature as subs.
  FEATURESPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after)
  FEATURESPLUGIN_EXPORT virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// removes also all sub-sketch
  FEATURESPLUGIN_EXPORT virtual void erase();

protected:
  FeaturesPlugin_CompositeBoolean(){};

  /// Define this function to init attributes for extrusion/revolution.
  virtual void initMakeSolidsAttributes() = 0;

  /// Define this function to create solids from faces with extrusion/revolution.
  virtual void makeSolids(const ListOfShape& theFaces,
                          ListOfShape& theResults,
                          std::list<std::shared_ptr<GeomAPI_Interface>>& theAlgos) = 0;

  /// Results naming.
  void loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    const ListOfShape& theShells,
                    const std::list<std::shared_ptr<GeomAPI_Interface>>& theSolidsAlgos,
                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                    const ListOfShape& theTools,
                    const std::shared_ptr<GeomAPI_Shape> theResultShape,
                    GeomAlgoAPI_MakeShape& theMakeShape,
                    GeomAPI_DataMapOfShapeShape& theMapOfShapes);

protected:
  /// Type of boolean operation.
  GeomAlgoAPI_Boolean::OperationType myBooleanOperationType;
};

#endif
