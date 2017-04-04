// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        FeaturesPlugin_MultiTranslation.h
// Created:     30 Jan 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESPLUGIN_MULTITRANSLATION_H_
#define FEATURESPLUGIN_MULTITRANSLATION_H_

#include <FeaturesPlugin.h>

#include <GeomAlgoAPI_Translation.h>

#include <ModelAPI_Feature.h>

/** \class FeaturesPlugin_MultiTranslation
 *  \ingroup Plugins
 *  \brief Feature for movement objects along one or two axes an arbitary number of times,
 *         making a copy every time.
 */
class FeaturesPlugin_MultiTranslation : public ModelAPI_Feature
{
  public:
  /// Multi-translation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_MULTITRANSLATION_ID("MultiTranslation");
    return MY_MULTITRANSLATION_ID;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name of a first direction.
  inline static const std::string& AXIS_FIRST_DIR_ID()
  {
    static const std::string MY_AXIS_FIRST_DIR_ID("axis_first_dir");
    return MY_AXIS_FIRST_DIR_ID;
  }

  /// Attribute name of step for the first direction.
  inline static const std::string& STEP_FIRST_DIR_ID()
  {
    static const std::string MY_STEP_FIRST_DIR_ID("step_first_dir");
    return MY_STEP_FIRST_DIR_ID;
  }

  /// Attribute name of number of copies for the first direction.
  inline static const std::string& NB_COPIES_FIRST_DIR_ID()
  {
    static const std::string MY_NB_COPIES_FIRST_DIR_ID("nb_first_dir");
    return MY_NB_COPIES_FIRST_DIR_ID;
  }

  /// Attribute name for use second dir.
  inline static const std::string& USE_SECOND_DIR_ID()
  {
    static const std::string MY_USE_SECOND_DIR_ID("use_second_dir");
    return MY_USE_SECOND_DIR_ID;
  }

  /// Attribute name of a second direction.
  inline static const std::string& AXIS_SECOND_DIR_ID()
  {
    static const std::string MY_AXIS_SECOND_DIR_ID("axis_second_dir");
    return MY_AXIS_SECOND_DIR_ID;
  }

  /// Attribute name of step for the second direction.
  inline static const std::string& STEP_SECOND_DIR_ID()
  {
    static const std::string MY_STEP_SECOND_DIR_ID("step_second_dir");
    return MY_STEP_SECOND_DIR_ID;
  }

  /// Attribute name of number of copies for the second direction.
  inline static const std::string& NB_COPIES_SECOND_DIR_ID()
  {
    static const std::string MY_NB_COPIES_SECOND_DIR_ID("nb_second_dir");
    return MY_NB_COPIES_SECOND_DIR_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_MultiTranslation::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_MultiTranslation();

private:
  /// Perform the multi translation in one direction.
  void performOneDirection();

  /// Perform the multi translation in two directions.
  void performTwoDirection();

  void loadNamingDS(std::list<std::shared_ptr<GeomAlgoAPI_Translation> > theListOfTranslationAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape);
};

#endif // FEATURESPLUGIN_MULTITRANSLATION_H_