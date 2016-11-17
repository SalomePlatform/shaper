// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionPlugin_Group.h
// Created:     14 Nov 2016
// Author:      Mikhail PONIKAROV

#ifndef COLLECTIONPLUGIN_FIELD_H_
#define COLLECTIONPLUGIN_FIELD_H_

#include "CollectionPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

/**\class CollectionPlugin_Field
 * \ingroup Plugins
 * \brief Feature for selection of sub-shapes of some shapes and assigning data on them.
 *
 * There is a set of containers that contains the following data that may be inserted by the user:
 * 
 * Selected sub-shapes (like in Groups).
 * Number of components for each selection.
 * Components names
 * Type of values: Boolean, Integer, Double or String.
 * Number of steps: for each step (indexes are zero-based) the values below are multiplied.
 * Stamp: integer identifier per each step
 * Defaul values for each components
 * Values for each component and each selected sub-shape: all are of specific type.
 */
class CollectionPlugin_Field : public ModelAPI_Feature
{
 public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_FIELD_ID("Field");
    return MY_FIELD_ID;
  }
  /// attribute name of selected entities list
  inline static const std::string& SELECTED_ID()
  {
    static const std::string MY_SELECTED_ID("selected");
    return MY_SELECTED_ID;
  }
  /// attribute name of components number
  inline static const std::string& COMPONENTS_NB_ID()
  {
    static const std::string MY_COMPONENTS_NB_ID("components_nb");
    return MY_COMPONENTS_NB_ID;
  }
  /// attribute name of componenets titles array
  inline static const std::string& COMPONENTS_NAMES_ID()
  {
    static const std::string MY_COMPONENTS_NAMES_ID("components_names");
    return MY_COMPONENTS_NAMES_ID;
  }
  /// attribute name of values types integer identifier
  inline static const std::string& VALUES_TYPE_ID()
  {
    static const std::string MY_VALUES_TYPE_ID("type");
    return MY_VALUES_TYPE_ID;
  }
  /// attribute name of number of steps
  inline static const std::string& STEPS_NB_ID()
  {
    static const std::string MY_STEPS_NB_ID("steps_nb");
    return MY_STEPS_NB_ID;
  }
  /// attribute name of stamps integer array
  inline static const std::string& STAMPS_ID()
  {
    static const std::string MY_STAMPS_ID("stamps");
    return MY_STAMPS_ID;
  }
  /// attribute name of list of tables that contain deafult values (row 0) and the custom values
  inline static const std::string& VALUES_ID()
  {
    static const std::string MY_VALUES_ID("values");
    return MY_VALUES_ID;
  }

  /// Returns the kind of a feature
  COLLECTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = CollectionPlugin_Field::ID();
    return MY_KIND;
  }

  /// Creates a new field result if needed
  COLLECTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  COLLECTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Result of fields is created on the fly and don't stored to the document
  COLLECTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  CollectionPlugin_Field();

};

#endif
