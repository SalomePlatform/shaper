// Copyright (C) 2016-20xx CEA/DEN, EDF R&D -->

// File:        ModelHighAPI_Dumper.h
// Created:     1 August 2016
// Author:      Artem ZHIDKOV

#ifndef ModelHighAPI_Dumper_H_
#define ModelHighAPI_Dumper_H_

#include "ModelHighAPI.h"

#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>

class GeomAPI_Pnt;
class GeomAPI_Dir;

class GeomDataAPI_Dir;
class GeomDataAPI_Point;
class GeomDataAPI_Point2D;

class ModelAPI_AttributeBoolean;
class ModelAPI_AttributeDouble;
class ModelAPI_AttributeInteger;
class ModelAPI_AttributeSelection;
class ModelAPI_AttributeString;
class ModelAPI_CompositeFeature;
class ModelAPI_Document;
class ModelAPI_Entity;
class ModelAPI_Feature;

typedef std::shared_ptr<ModelAPI_Entity>  EntityPtr;
typedef std::shared_ptr<ModelAPI_Feature> FeaturePtr;

/**\class ModelHighAPI_Dumper
 * \ingroup CPPHighAPI
 * \brief Dump engine for the model
 */
class ModelHighAPI_Dumper
{
public:
  /// Default constructor
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper();

  /// Sets instance of a dumper
  MODELHIGHAPI_EXPORT
  static void setInstance(ModelHighAPI_Dumper* theDumper);

  /// Returns instance of a dumper
  MODELHIGHAPI_EXPORT
  static ModelHighAPI_Dumper* getInstance();

  /// Destructor
  virtual ~ModelHighAPI_Dumper() {}

  /// Dump given document into the file
  /// \return \c true, if succeed
  MODELHIGHAPI_EXPORT
  bool process(const std::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theFileName);

  /// Add module to list of imported modules
  /// \param theModuleName  name of the module to be imported
  /// \param theObject      name of the entity to be imported from the module (if empty, while module will be imported)
  MODELHIGHAPI_EXPORT
  void importModule(const std::string& theModuleName,
                    const std::string& theObject = std::string());

  /// Returns name of specified entity
  MODELHIGHAPI_EXPORT
  const std::string& name(const EntityPtr& theEntity);

  /// Returns name of parent composite feature for specified entity
  MODELHIGHAPI_EXPORT
  const std::string& parentName(const FeaturePtr& theFeature);

  /// Dump given feature
  virtual void dumpFeature(const FeaturePtr& theFeature, const bool theForce = false) = 0;

  /// Save all dumps into specified file
  MODELHIGHAPI_EXPORT
  bool exportTo(const std::string& theFileName);

  /// Dump character
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const char theChar);
  /// Dump string
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const char* theString);
  /// Dump string
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::string& theString);
  /// Dump integer
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const int theValue);
  /// Dump real
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const double theValue);
  /// Dump std::endl
  friend
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(ModelHighAPI_Dumper& theDumper,
                                  std::basic_ostream<char>& (*theEndl)(std::basic_ostream<char>&));

  /// Dump GeomAPI_Pnt in the following form:
  /// "GeomAPI_Pnt(X, Y, Z)"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<GeomAPI_Pnt>& thePoint);
  /// Dump GeomAPI_Dir
  /// "GeomAPI_Dir(X, Y, Z)"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<GeomAPI_Dir>& theDir);

  /// Dump GeomDataAPI_Dir in the following form:
  /// "X, Y, Z"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<GeomDataAPI_Dir>& theDir);
  /// Dump GeomDataAPI_Point in the following form:
  /// "X, Y, Z"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<GeomDataAPI_Point>& thePoint);
  /// Dump GeomDataAPI_Point2D in the following form:
  /// "X, Y"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<GeomDataAPI_Point2D>& thePoint);

  /// Dump AttributeBoolean
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeBoolean>& theAttrBool);
  /// Dump AttributeInteger
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeInteger>& theAttrInt);
  /// Dump AttributeDouble
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeDouble>& theAttrReal);
  /// Dump AttributeString
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeString>& theAttrStr);
  /// Dump name of entity and remember to dump "setName" if the entity has user-defined name
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const EntityPtr& theEntity);

  /// Dump AttributeSelection
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeSelection>& theAttrSelect);

protected:
  /// Dump "setName" command if last entity had user-defined name
  MODELHIGHAPI_EXPORT void dumpEntitySetName();

  /// Clear dump buffer
  void clear();

private:
  ModelHighAPI_Dumper(const ModelHighAPI_Dumper&);
  const ModelHighAPI_Dumper& operator=(const ModelHighAPI_Dumper&);

  /// Iterate all features in document and dump them into intermediate buffer
  bool process(const std::shared_ptr<ModelAPI_Document>& theDoc);

  /// Iterate all features in composite feature and dump them into intermediate buffer
  bool process(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite);

private:
  typedef std::map<EntityPtr, std::pair<std::string, bool> > EntityNameMap;
  typedef std::map<std::string, std::set<std::string> >      ModulesMap;

  static ModelHighAPI_Dumper* mySelf;

  std::ostringstream  myDumpBuffer;         ///< intermediate buffer to store dumping data
  ModulesMap          myModules;            ///< modules and entities to be imported
  EntityNameMap       myNames;              ///< names of the entities
  EntityPtr           myLastEntityWithName; ///< not null, if last dumped entity had user defined name
};

#endif
