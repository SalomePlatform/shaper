// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionCut.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_ExtrusionCut_H_
#define FeaturesPlugin_ExtrusionCut_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_CompositeFeature.h>

/** \class FeaturesPlugin_ExtrusionCut
 *  \ingroup Plugins
 */
class FeaturesPlugin_ExtrusionCut : public ModelAPI_CompositeFeature
{
 public:
  /// Revolution kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("ExtrusionCut");
    return MY_REVOLUTION_ID;
  }

  /// attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face
  inline static const std::string& CUTLIST_ID()
  {
    static const std::string MY_CUT_LIST_ID("cut_objects");
    return MY_CUT_LIST_ID;
  }

  /// attribute name of an object to which the extrusion grows
  inline static const std::string& SKETCH_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("sketch");
    return MY_TO_OBJECT_ID;
  }

  /// attribute name of extrusion size
  inline static const std::string& TO_SIZE_ID()
  {
    static const std::string MY_TO_SIZE_ID("to_size");
    return MY_TO_SIZE_ID;
  }

  /// attribute name of an object to which the extrusion grows
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// attribute name of tool object
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }

  /// attribute name of extrusion size
  inline static const std::string& FROM_SIZE_ID()
  {
    static const std::string MY_FROM_SIZE_ID("from_size");
    return MY_FROM_SIZE_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_ExtrusionCut::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_ExtrusionCut();

  /// appends a feature to the sketch sub-elements container
  FEATURESPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);
  
  /// Returns the number of sub-elements
  FEATURESPLUGIN_EXPORT virtual int numberOfSubs() const;

  /// Returns the sub-feature by zero-base index
  FEATURESPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex) const;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  FEATURESPLUGIN_EXPORT virtual int subFeatureId(const int theIndex) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  FEATURESPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after)
  FEATURESPLUGIN_EXPORT virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);
};

#endif
