// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
class ModelAPI_AttributeStringArray;
class ModelAPI_CompositeFeature;
class ModelAPI_Document;
class ModelAPI_Entity;
class ModelAPI_Feature;
class ModelAPI_Folder;
class ModelAPI_Object;
class ModelAPI_Result;

typedef std::shared_ptr<ModelAPI_Document> DocumentPtr;
typedef std::shared_ptr<ModelAPI_Entity>   EntityPtr;
typedef std::shared_ptr<ModelAPI_Feature>  FeaturePtr;
typedef std::shared_ptr<ModelAPI_Folder>   FolderPtr;
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

  /// Set/unset flag to dump selection attributes by geometrical properties:
  /// inner point in the selected shape
  void setSelectionByGeometry(bool theDumpByGeom = true)
  { myGeometricalSelection = theDumpByGeom; }

  /// Dump given document into the file
  /// \return \c true, if succeed
  MODELHIGHAPI_EXPORT
  bool process(const std::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theFileName);

  /// Add module to list of imported modules
  /// \param theModuleName  name of the module to be imported
  /// \param theObject      name of the entity to be imported
  ///                       from the module (if empty, while module will be imported)
  MODELHIGHAPI_EXPORT
  void importModule(const std::string& theModuleName,
                    const std::string& theObject = std::string());

  /// Returns name of specified entity
  /// \param theEntity        [in] named entity
  /// \param theSaveNotDumped [in]
  ///    if \c true, the entity should be stored as not dumped (will be dumped automatically)
  /// \param theUseEntityName [in]
  ///    if \c true, the entity name should be used "as is" without changing default name
  /// \return name of the entity
  MODELHIGHAPI_EXPORT
  const std::string& name(const EntityPtr& theEntity, bool theSaveNotDumped = true,
                          bool theUseEntityName = false);

  /// Returns name of parent composite feature for specified entity
  MODELHIGHAPI_EXPORT
  const std::string& parentName(const FeaturePtr& theFeature);

  /// Dump parameter feature only
  virtual void dumpParameter(const FeaturePtr& theFeature) = 0;
  /// Dump given feature
  virtual void dumpFeature(const FeaturePtr& theFeature, const bool theForce = false) = 0;
  /// Dump folder
  virtual void dumpFolder(const FolderPtr& theFolder) = 0;

  /// Set feature postponed until all its dependencies are not dumped.
  /// The name of the feature is stored anyway.
  MODELHIGHAPI_EXPORT
  void postpone(const EntityPtr& theEntity);

  /// Set a feature that should not be dumped anyway
  MODELHIGHAPI_EXPORT
  void doNotDumpFeature(const FeaturePtr& theFeature)
  { myFeaturesToSkip.insert(theFeature); }

  /// Dump sub-feature name and color, without dumping feature creation.
  /// Used for features which creates sub-features in their execute method.
  /// \param theSubFeatureGet [in] method for getting sub-feature (e.g. "Feature_1.subFeature(0)")
  /// \param theSubFeature    [in] sub-feature
  MODELHIGHAPI_EXPORT
  void dumpSubFeatureNameAndColor(const std::string theSubFeatureGet,
                                  const FeaturePtr& theSubFeature);

  /// Return name of getter for corresponding attribute
  virtual std::string attributeGetter(const FeaturePtr& theFeature,
                                      const std::string& theAttrName) const = 0;

  /// Return name of wrapper feature
  virtual std::string featureWrapper(const FeaturePtr& theFeature) const = 0;

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

  /// Dump folder
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const FolderPtr& theFolder);

  /// Dump result
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Dumper& operator<<(const ResultPtr& theResult);

  /// Dump Attribute
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_Attribute>& theAttr);
  /// Dump Object
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Dump AttributeRefAttr
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_AttributeRefAttr>& theRefAttr);
  /// Dump AttributeRefAttrList as follows:
  /// "[obj1, obj2, obj3, ...]"
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_AttributeRefAttrList>& theRefAttrList);
  /// Dump AttributeRefList as follows:
  /// "[obj1, obj2, obj3, ...]"
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_AttributeRefList>& theRefList);
  /// Dump AttributeSelection
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_AttributeSelection>& theAttrSelect);
  /// Dump AttributeSelectionList
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_AttributeSelectionList>& theAttrSelList);
  /// Dump AttributeReference
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
  operator<<(const std::shared_ptr<ModelAPI_AttributeReference>& theReference);
  /// Dump AttributeStringArray
  MODELHIGHAPI_EXPORT ModelHighAPI_Dumper&
    operator<<(const std::shared_ptr<ModelAPI_AttributeStringArray>& theArray);

  /// Clear dump buffer
  MODELHIGHAPI_EXPORT
  void clear(bool bufferOnly = false);
  /// clear list of not dumped entities
  MODELHIGHAPI_EXPORT void clearNotDumped();

  /// Check the entity is already dumped
  MODELHIGHAPI_EXPORT
  bool isDumped(const EntityPtr& theEntity) const;
  /// Check theRefAttr is already dumped
  MODELHIGHAPI_EXPORT
  bool isDumped(const std::shared_ptr<ModelAPI_AttributeRefAttr>& theRefAttr) const;
  /// Check all objects in theRefList are already dumped
  MODELHIGHAPI_EXPORT
  bool isDumped(const std::shared_ptr<ModelAPI_AttributeRefList>& theRefList) const;

protected:
  /// Dump "setName" command if last entity had user-defined name
  MODELHIGHAPI_EXPORT void dumpEntitySetName();

private:
  ModelHighAPI_Dumper(const ModelHighAPI_Dumper&);
  const ModelHighAPI_Dumper& operator=(const ModelHighAPI_Dumper&);

  /// Iterate all features in document and dump them into intermediate buffer
  bool process(const std::shared_ptr<ModelAPI_Document>& theDoc);

  /// Dump composite feature and all it sub-features
  bool process(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite,
               bool isForce = false);

  /// Iterate all features in composite feature and dump them into intermediate buffer
  /// \param theComposite   [in] parent composite feature
  /// \param theDumpModelDo [in] shows that command "model.do()" should be written at the end
  MODELHIGHAPI_EXPORT
  bool processSubs(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite,
                   bool theDumpModelDo = false);

  /// Stores names of results for the given feature
  void saveResultNames(const FeaturePtr& theFeature);

  /// Check the result feature has default color
  bool isDefaultColor(const ResultPtr& theResult) const;

  /// Check the result feature has default deflection
  bool isDefaultDeflection(const ResultPtr& theResult) const;

  /// Check the result feature has default transparency
  bool isDefaultTransparency(const ResultPtr& theResult) const;

  /// Dump postponed entities
  void dumpPostponed(bool theDumpFolders = false);

private:
  struct EntityName {
    std::string myCurrentName; ///< default name of current feature
    std::string myUserName;    ///< user-defined name
    bool        myIsDefault;   ///< \c true if the name is default
    bool        myIsDumped;    ///< shows that the names of the feature are already stored

    EntityName() {}

    EntityName(const std::string& theCurName, const std::string& theUserName, bool theDefault)
      : myCurrentName(theCurName),
        myUserName(theUserName),
        myIsDefault(theDefault),
        myIsDumped(false)
    {}
  };

  typedef std::map<EntityPtr, EntityName>                                      EntityNameMap;
  typedef std::map<std::string, std::set<std::string> >                        ModulesMap;
  typedef std::map<DocumentPtr, std::map<std::string, std::pair<int, int> > >  NbFeaturesMap;

  struct LastDumpedEntity {
    EntityPtr            myEntity;   ///< last dumped entity
    bool                 myUserName; ///< the entity hase user-defined name
    /// results of this entity, which has user-defined names or specific colors
    std::list<ResultPtr> myResults;

    LastDumpedEntity(EntityPtr theEntity, bool theUserName,
      const std::list<ResultPtr>& theResults = std::list<ResultPtr>())
      : myEntity(theEntity), myUserName(theUserName), myResults(theResults)
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

  /// features which should not be dumped (like coincidence and tangency created by tangent arc)
  std::set<FeaturePtr> myFeaturesToSkip;

  std::list<EntityPtr> myPostponed; ///< list of postponed entities (sketch constraints or folders)
  bool myDumpPostponedInProgress; ///< processing postponed is in progress

  bool myGeometricalSelection; ///< dump selection not by naming, but by coordinates of inner point

protected:
  /// list of entities, used by other features but not dumped yet
  std::set<EntityPtr> myNotDumpedEntities;

  friend class SketchAPI_Sketch;
  friend class ModelHighAPI_Folder;
};

#endif
