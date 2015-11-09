// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_MultiTranslation.h
// Created: 21 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_MultiTranslation_H_
#define SketchPlugin_MultiTranslation_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_MultiTranslation
 *  \ingroup Plugins
 *  \brief Feature for creation of a number of shifted copies of a list of objects
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() for initial list of objects and
 *  SketchPlugin_Constraint::ENTITY_B() for the list of created objects
 *
 *  The list of created objects contains initial and copied objects of each object given. The
 *  number copies is the NUMBER_OF_OBJECTS_ID() minus 1. At the start of the list, there are
 *  collected N copies of first object from initial list, then N copies of second object etc.
 */
class SketchPlugin_MultiTranslation : public SketchPlugin_ConstraintBase
{
 public:
  /// Multi-translation kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_TRANSLATION_ID("SketchMultiTranslation");
    return MY_CONSTRAINT_TRANSLATION_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MultiTranslation::ID();
    return MY_KIND;
  }

  /// List of objects to be mirrored
  inline static const std::string& TRANSLATION_LIST_ID()
  {
    static const std::string MY_TRANSLATION_LIST_ID("MultiTranslationList");
    return MY_TRANSLATION_LIST_ID;
  }

  /// attribute name for first point
  inline static const std::string& VALUE_TYPE()
  {
    static const std::string VALUE_TYPE_ATTR("ValueType");
    return VALUE_TYPE_ATTR;
  }

  /// Start point of translation
  inline static const std::string& START_POINT_ID()
  {
    static const std::string MY_START_POINT_ID("MultiTranslationStartPoint");
    return MY_START_POINT_ID;
  }
  /// Start point of translation
  inline static const std::string& START_FULL_POINT_ID()
  {
    static const std::string MY_START_FULL_POINT_ID("MultiTranslationFullStartPoint");
    return MY_START_FULL_POINT_ID;
  }

  /// End point of translation
  inline static const std::string& END_POINT_ID()
  {
    static const std::string MY_END_POINT_ID("MultiTranslationEndPoint");
    return MY_END_POINT_ID;
  }
  /// End point of translation
  inline static const std::string& END_FULL_POINT_ID()
  {
    static const std::string MY_END_FULL_POINT_ID("MultiTranslationFullEndPoint");
    return MY_END_FULL_POINT_ID;
  }

  /// Total number of objects, initial and translated objects
  inline static const std::string& NUMBER_OF_OBJECTS_ID()
  {
    static const std::string MY_NUMBER_OF_OBJECTS_ID("MultiTranslationObjects");
    return MY_NUMBER_OF_OBJECTS_ID;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_MultiTranslation();

private:
  ObjectPtr copyFeature(ObjectPtr theObject);

private:
  bool myBlockValue; /// a boolean state to avoid recusive value change in attributeChanged
};

#endif
