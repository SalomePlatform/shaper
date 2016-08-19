// Copyright (C) 2016-20xx CEA/DEN, EDF R&D -->

// File:        ModelHighAPI_Dumper.h
// Created:     1 August 2016
// Author:      Artem ZHIDKOV

#ifndef ModelHighAPI_Dumper_H_
#define ModelHighAPI_Dumper_H_

#include "ModelHighAPI.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stack>
#include <string>

class GeomAPI_Pnt;
class GeomAPI_Dir;

class GeomDataAPI_Dir;
class GeomDataAPI_Point;
class GeomDataAPI_Point2D;

class ModelAPI_Attribute;
class ModelAPI_AttributeBoolean;
class ModelAPI_AttributeDouble;
class ModelAPI_AttributeInteger;
class ModelAPI_AttributeRefAttr;
class ModelAPI_AttributeRefAttrList;
class ModelAPI_AttributeReference;
class ModelAPI_AttributeRefList;
class ModelAPI_AttributeSelection;
class ModelAPI_AttributeSelectionList;
class ModelAPI_AttributeString;
class ModelAPI_CompositeFeature;
class ModelAPI_Document;
class ModelAPI_Entity;
class ModelAPI_Feature;
class ModelAPI_Object;
class ModelAPI_Result;

typedef std::shared_ptr<ModelAPI_Document> DocumentPtr;
typedef std::shared_ptr<ModelAPI_Entity>   EntityPtr;
typedef std::shared_ptr<ModelAPI_Feature>  FeaturePtr;
typedef std::shared_ptr<ModelAPI_Result>   ResultPtr;

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
  /// \param theEntity        [in] named entity
  /// \param theSaveNotDumped [in] if \c true, the entity should be stored as not dumped (will be dumped automatically)
  /// \param theUseEntityName [in] if \c true, the entity name should be used "as is" without changing default name
  /// \return name of the entity
  MODELHIGHAPI_EXPORT
  const std::string& name(const EntityPtr& theEntity, bool theSaveNotDumped = true, bool theUseEntityName = false);

  /// Returns name of parent composite feature for specified entity
  MODELHIGHAPI_EXPORT
  const std::string& parentName(const FeaturePtr& theFeature);

  /// Dump parameter feature only
  virtual void dumpParameter(const FeaturePtr& theFeature) = 0;
  /// Dump given feature
  virtual void dumpFeature(const FeaturePtr& theFeature, const bool theForce = false) = 0;

  /// Return name of getter for corresponding attribute
  virtual std::string attributeGetter(const FeaturePtr& theFeature,
                                      const std::string& theAttrName) const = 0;

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
  /// Dump boolean
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const bool theValue);
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
  ModelHighAPI_Dumper& operator<<(const FeaturePtr& theEntity);

  /// Dump result
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const ResultPtr& theResult);

  /// Dump Attribute
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_Attribute>& theAttr);
  /// Dump Object
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Dump AttributeRefAttr
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeRefAttr>& theRefAttr);
  /// Dump AttributeRefAttrList as follows:
  /// "[obj1, obj2, obj3, ...]"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeRefAttrList>& theRefAttrList);
  /// Dump AttributeRefList as follows:
  /// "[obj1, obj2, obj3, ...]"
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeRefList>& theRefList);
  /// Dump AttributeSelection
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeSelection>& theAttrSelect);
  /// Dump AttributeSelectionList
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeSelectionList>& theAttrSelList);
  /// Dump AttributeReference
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const std::shared_ptr<ModelAPI_AttributeReference>& theReference);

  /// Clear dump buffer
  MODELHIGHAPI_EXPORT
  void clear(bool bufferOnly = false);
  /// clear list of not dumped entities
  MODELHIGHAPI_EXPORT void clearNotDumped();

protected:
  /// Dump "setName" command if last entity had user-defined name
  MODELHIGHAPI_EXPORT void dumpEntitySetName();

private:
  ModelHighAPI_Dumper(const ModelHighAPI_Dumper&);
  const ModelHighAPI_Dumper& operator=(const ModelHighAPI_Dumper&);

  /// Iterate all features in document and dump them into intermediate buffer
  bool process(const std::shared_ptr<ModelAPI_Document>& theDoc);

  /// Dump composite feature and all it sub-features
  bool process(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite, bool isForce = false);

  /// Iterate all features in composite feature and dump them into intermediate buffer
  /// \param theComposite   [in] parent composite feature
  /// \param theDumpModelDo [in] shows that command "model.do()" should be written at the end
  MODELHIGHAPI_EXPORT
  bool processSubs(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite, bool theDumpModelDo = false);

  /// Check the entity is already dumped
  bool isDumped(const EntityPtr& theEntity) const;

  /// Stores names of results for the given feature
  void saveResultNames(const FeaturePtr& theFeature);

private:
  typedef std::map<EntityPtr, std::pair<std::string, std::string> > EntityNameMap;
  typedef std::map<std::string, std::set<std::string> >             ModulesMap;
  typedef std::map<DocumentPtr, std::map<std::string, int> >        NbFeaturesMap;

  struct LastDumpedEntity {
    EntityPtr            myEntity; // last dumped entity
    bool                 myUserName; // the entity hase user-defined name
    std::list<ResultPtr> myResultsWithName; // results of this entity, which has user-defined names

    LastDumpedEntity(EntityPtr theEntity, bool theUserName, const std::list<ResultPtr>& theResults)
      : myEntity(theEntity), myUserName(theUserName), myResultsWithName(theResults)
    {}
  };
  typedef std::stack<LastDumpedEntity>                              DumpStack;

  static ModelHighAPI_Dumper* mySelf;

  std::ostringstream  myDumpBuffer;         ///< intermediate buffer to store dumping data
  std::ostringstream  myFullDump;           ///< full buffer of dumped data

  ModulesMap          myModules;            ///< modules and entities to be imported
  EntityNameMap       myNames;              ///< names of the entities
  DumpStack           myEntitiesStack;      ///< stack of dumped entities

  NbFeaturesMap       myFeatureCount;       ///< number of features of each kind

protected:
  std::set<EntityPtr> myNotDumpedEntities;  ///< list of entities, used by other features but not dumped yet

  friend class SketchAPI_Sketch;
};

#endif
