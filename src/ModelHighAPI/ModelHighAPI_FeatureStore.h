// Copyright (C) 2016-20xx CEA/DEN, EDF R&D -->

// File:        ModelHighAPI_FeatureStore.h
// Created:     12 August 2016
// Author:      Mikhail PONIKAROV

#ifndef ModelHighAPI_FeatureStore_H_
#define ModelHighAPI_FeatureStore_H_

#include "ModelHighAPI.h"

#include <map>
#include <list>
#include <string>
#include <memory>

class ModelAPI_Feature;
class ModelAPI_Data;
class GeomAPI_Shape;
class ModelAPI_Attribute;

typedef std::shared_ptr<ModelAPI_Feature>  FeaturePtr;
typedef std::shared_ptr<ModelAPI_Attribute>  AttributePtr;

/**\class ModelHighAPI_FeatureStore
 * \ingroup CPPHighAPI
 * \brief Allows to compare the original and the dumped/executed feature
 */
class ModelHighAPI_FeatureStore {
  /// dumps of attributes of the stored feature (id -> dump)
  std::map<std::string, std::string> myAttrs;
   /// dumps of attributes of results (list of results id -> dumps)
  std::list<std::map<std::string, std::string> > myRes;
public:
  // unused constructor for the map container needs
  ModelHighAPI_FeatureStore() {}
  // constructor that initializes this object by feature to store
  ModelHighAPI_FeatureStore(FeaturePtr theFeature);
  // compares the stored feature information with the given feature
  std::string compare(FeaturePtr theFeature);

private:
  /// stores the information about all attributes of data in map
  void storeData(std::shared_ptr<ModelAPI_Data> theData, 
                 std::map<std::string, std::string>& theAttrs);
  /// compares the information about all attributes of data with map
  /// returns not empty string with error if something is different
  std::string compareData(std::shared_ptr<ModelAPI_Data> theData, 
                 std::map<std::string, std::string>& theAttrs);
  /// dumps the attribute content to the string
  std::string dumpAttr(const AttributePtr& theAttr);
  /// dumps the shape main charatceristics to string
  std::string dumpShape(std::shared_ptr<GeomAPI_Shape>& theShape);
};

#endif