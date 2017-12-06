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

#include "ModelHighAPI_Dumper.h"

#include <Config_PropManager.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Entity.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Folder.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>

#include <PartSetPlugin_Part.h>

#include <OSD_OpenFile.hxx>

#include <fstream>

static int gCompositeStackDepth = 0;

ModelHighAPI_Dumper* ModelHighAPI_Dumper::mySelf = 0;

ModelHighAPI_Dumper::ModelHighAPI_Dumper()
{
  clear();
}

void ModelHighAPI_Dumper::setInstance(ModelHighAPI_Dumper* theDumper)
{
  if (mySelf == 0)
    mySelf = theDumper;
}

ModelHighAPI_Dumper* ModelHighAPI_Dumper::getInstance()
{
  return mySelf;
}

void ModelHighAPI_Dumper::clear(bool bufferOnly)
{
  myDumpBuffer.str("");
  myDumpBuffer << std::setprecision(16);

  clearNotDumped();

  if (!bufferOnly) {
    myFullDump.str("");
    myFullDump << std::setprecision(16);

    myNames.clear();
    myModules.clear();
    myFeatureCount.clear();
    while (!myEntitiesStack.empty())
      myEntitiesStack.pop();

    myPostponed.clear();
    myDumpPostponedInProgress = false;
  }
}

void ModelHighAPI_Dumper::clearNotDumped()
{
  myNotDumpedEntities.clear();
}

// Convert string to integer. If the string is not a number, return -1
static int toInt(const std::string& theString)
{
  std::string::const_iterator aChar = theString.begin();
  for (; aChar != theString.end(); ++aChar)
    if (!std::isdigit(*aChar))
      break;
  if (aChar != theString.end())
    return -1; // not a number
  return std::stoi(theString);
}

const std::string& ModelHighAPI_Dumper::name(const EntityPtr& theEntity,
                                             bool theSaveNotDumped,
                                             bool theUseEntityName)
{
  EntityNameMap::const_iterator aFound = myNames.find(theEntity);
  if (aFound != myNames.end())
    return aFound->second.myCurrentName;

  // entity is not found, store it
  std::string aName, aKind;
  bool isDefaultName = false;
  bool isSaveNotDumped = theSaveNotDumped;
  std::ostringstream aDefaultName;
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theEntity);
  if (aFeature) {
    aName = aFeature->name();
    aKind = aFeature->getKind();
  } else {
    FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(theEntity);
    if (aFolder) {
      aName = aFolder->data()->name();
      aKind = ModelAPI_Folder::ID();
      isSaveNotDumped = false;
    }
  }

  ObjectPtr anObject = std::dynamic_pointer_cast<ModelAPI_Object>(theEntity);
  if (anObject) {
    DocumentPtr aDoc = anObject->document();
    int& aNbFeatures = myFeatureCount[aDoc][aKind];
    aNbFeatures += 1;

    size_t anIndex = aName.find(aKind);
    if (anIndex == 0 && aName[aKind.length()] == '_') { // name starts with "FeatureKind_"
      std::string anIdStr = aName.substr(aKind.length() + 1);
      int anId = toInt(anIdStr);

      // Check number of already registered objects of such kind. Index of current object
      // should be the same to identify feature's name as automatically generated.
      if (aNbFeatures == anId) {
        // name is not user-defined
        isDefaultName = true;
      }
    }

    // obtain default name for the feature
    if (theUseEntityName)
      aDefaultName << aName;
    else {
      int aFullIndex = 0;
      NbFeaturesMap::const_iterator aFIt = myFeatureCount.begin();
      for (; aFIt != myFeatureCount.end(); ++aFIt) {
        std::map<std::string, int>::const_iterator aFound = aFIt->second.find(aKind);
        if (aFound != aFIt->second.end())
          aFullIndex += aFound->second;
      }
      aDefaultName << aKind << "_" << aFullIndex;
    }
  }

  myNames[theEntity] = EntityName(aDefaultName.str(), aName, isDefaultName);
  if (isSaveNotDumped)
    myNotDumpedEntities.insert(theEntity);

  // store names of results
  if (aFeature)
    saveResultNames(aFeature);

  return myNames[theEntity].myCurrentName;
}

const std::string& ModelHighAPI_Dumper::parentName(const FeaturePtr& theEntity)
{
  const std::set<AttributePtr>& aRefs = theEntity->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
  for (; aRefIt != aRefs.end(); ++aRefIt) {
    CompositeFeaturePtr anOwner = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(
        ModelAPI_Feature::feature((*aRefIt)->owner()));
    if (anOwner)
      return name(anOwner);
  }

  static const std::string DUMMY;
  return DUMMY;
}

void ModelHighAPI_Dumper::saveResultNames(const FeaturePtr& theFeature)
{
  // Default name of the feature
  bool isFeatureDefaultName = myNames[theFeature].myIsDefault;

  // Save only names of results which is not correspond to default feature name
  const std::list<ResultPtr>& aResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
  for (int i = 0; aResIt != aResults.end(); ++aResIt, ++i) {
    std::pair<std::string, bool> aName = ModelAPI_Tools::getDefaultName(*aResIt, i);
    std::string aDefaultName = aName.first;
    std::string aResName = (*aResIt)->data()->name();

    bool isUserDefined = !(isFeatureDefaultName && aDefaultName == aResName);

    myNames[*aResIt] = EntityName(aResName,
        (isUserDefined ? aResName : std::string()), !isUserDefined);

    // check names of sub-results for CompSolid
    ResultCompSolidPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aResIt);
    if (aCompSolid) {
      int aNbSubs = aCompSolid->numberOfSubs();
      for (int j = 0; j < aNbSubs; ++j) {
        ResultPtr aSub = aCompSolid->subResult(j);
        std::string aSubName = aSub->data()->name();
        aName = ModelAPI_Tools::getDefaultName(aSub, j);
        aDefaultName = aName.first;

        bool isUserDefinedSubName = isUserDefined || aDefaultName != aSubName;
        myNames[aSub] = EntityName(aSubName,
            (isUserDefinedSubName ? aSubName : std::string()), !isUserDefinedSubName);
      }
    }
  }
}

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_Document>& theDoc,
                                  const std::string& theFileName)
{
  // dump top level document feature
  static const std::string aDocName("partSet");
  myNames[theDoc] = EntityName(aDocName, std::string(), true);
  *this << aDocName << " = model.moduleDocument()" << std::endl;

  // dump subfeatures and store result to file
  return process(theDoc) && exportTo(theFileName);
}

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_Document>& theDoc)
{
  bool isOk = true;
  std::list<ObjectPtr> anObjects = theDoc->allObjects();
  std::list<ObjectPtr>::const_iterator anObjIt = anObjects.begin();
  // firstly, dump all parameters
  for (; anObjIt != anObjects.end(); ++ anObjIt) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIt);
    if (aFeature)
      dumpParameter(aFeature);
  }
  // dump all other features
  for (anObjIt = anObjects.begin(); anObjIt != anObjects.end(); ++anObjIt) {
    CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*anObjIt);
    if (aCompFeat) // iteratively process composite features
      isOk = process(aCompFeat) && isOk;
    else if (!isDumped(EntityPtr(*anObjIt))) {
      // dump folder
      FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(*anObjIt);
      if (aFolder)
        dumpFolder(aFolder);
      else {
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIt);
        if (aFeature) // dump common feature
          dumpFeature(aFeature);
      }
    }
  }
  return isOk;
}

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite,
                                  bool isForce)
{
  // increase composite features stack
  ++gCompositeStackDepth;
  // dump composite itself
  if (!isDumped(EntityPtr(theComposite)) || isForce)
    dumpFeature(FeaturePtr(theComposite), isForce);

  // sub-part is processed independently, because it provides separate document
  if (theComposite->getKind() == PartSetPlugin_Part::ID()) {
    // dump name of the part if it is different from default
    if (!myEntitiesStack.empty())
      dumpEntitySetName();

    // decrease composite features stack because we run into separate document
    --gCompositeStackDepth;

    ResultPartPtr aPartResult =
        std::dynamic_pointer_cast<ModelAPI_ResultPart>(theComposite->lastResult());
    if (!aPartResult)
      return false;
    DocumentPtr aSubDoc = aPartResult->partDoc();
    if (!aSubDoc)
      return false;
    // set name of document
    const std::string& aPartName = myNames[theComposite].myCurrentName;
    std::string aDocName = aPartName + "_doc";
    myNames[aSubDoc] = EntityName(aDocName, std::string(), true);

    // dump document in a separate line
    *this << aDocName << " = " << aPartName << ".document()" << std::endl;
    // dump features in the document
    bool aRes = process(aSubDoc);
    *this << "model.do()" << std::endl;
    return aRes;
  }

  // dump sub-features
  bool isOk = processSubs(theComposite);
  // decrease composite features stack
  --gCompositeStackDepth;

  return isOk;
}

bool ModelHighAPI_Dumper::processSubs(
  const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite,
  bool theDumpModelDo)
{
  bool isOk = true;
  // dump all sub-features;
  bool isSubDumped = false;
  int aNbSubs = theComposite->numberOfSubs();
  for (int anIndex = 0; anIndex < aNbSubs; ++anIndex) {
    FeaturePtr aFeature = theComposite->subFeature(anIndex);
    if (isDumped(EntityPtr(aFeature)))
      continue;

    isSubDumped = true;
    CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
    if (aCompFeat) // iteratively process composite features
      isOk = process(aCompFeat) && isOk;
    else
      dumpFeature(aFeature, true);
  }

  bool isDumpSetName = !myEntitiesStack.empty() &&
      myEntitiesStack.top().myEntity == EntityPtr(theComposite);
  bool isForceModelDo = isSubDumped && isDumpSetName &&
      (myEntitiesStack.top().myUserName || !myEntitiesStack.top().myResults.empty());
  // It is necessary for the sketch to create its result when complete (command "model.do()").
  // This option is set by flat theDumpModelDo.
  // However, nested sketches are rebuilt by parent feature, so, they do not need
  // explicit call of "model.do()". This will be controlled by the depth of the stack.
  if (isForceModelDo || (theDumpModelDo && gCompositeStackDepth <= 1))
    *this << "model.do()" << std::endl;

  // dump "setName" for composite feature
  if (isDumpSetName)
    dumpEntitySetName();
  // dump folders if any
  dumpPostponed();
  return isOk;
}

void ModelHighAPI_Dumper::postpone(const EntityPtr& theEntity)
{
  // keep the name
  name(theEntity, false);
  myPostponed.push_back(theEntity);
}

void ModelHighAPI_Dumper::dumpPostponed()
{
  if (myDumpPostponedInProgress)
    return;

  myDumpPostponedInProgress = true;
  // make a copy of postponed entities, because the list will be updated
  // if some features are not able to be dumped
  std::list<EntityPtr> aPostponedCopy = myPostponed;
  myPostponed.clear();

  // iterate over postponed entities and try to dump them
  std::list<EntityPtr>::const_iterator anIt = aPostponedCopy.begin();
  for (; anIt != aPostponedCopy.end(); ++anIt) {
    FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(*anIt);
    if (aFolder)
      dumpFolder(aFolder);
    else {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
      if (aFeature)
        dumpFeature(aFeature);
    }
  }
  myDumpPostponedInProgress = false;
}

void ModelHighAPI_Dumper::dumpSubFeatureNameAndColor(const std::string theSubFeatureGet,
                                                     const FeaturePtr& theSubFeature)
{
  name(theSubFeature, false);
  myNames[theSubFeature] = EntityName(theSubFeatureGet, theSubFeature->name(), false);

  // store results if they have user-defined names or colors
  std::list<ResultPtr> aResultsWithNameOrColor;
  const std::list<ResultPtr>& aResults = theSubFeature->results();
  std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
  for (; aResIt != aResults.end(); ++aResIt) {
    std::string aResName = (*aResIt)->data()->name();
    myNames[*aResIt] = EntityName(aResName, aResName, false);
    aResultsWithNameOrColor.push_back(*aResIt);
  }

  // store just dumped entity to stack
  myEntitiesStack.push(LastDumpedEntity(theSubFeature, true, aResultsWithNameOrColor));

  dumpEntitySetName();
}

bool ModelHighAPI_Dumper::exportTo(const std::string& theFileName)
{
  std::ofstream aFile;
  OSD_OpenStream(aFile, theFileName.c_str(), std::ofstream::out);
  if (!aFile.is_open())
    return false;

  // standard header
  for (ModulesMap::const_iterator aModIt = myModules.begin();
       aModIt != myModules.end(); ++aModIt) {
    aFile << "from " << aModIt->first << " import ";
    if (aModIt->second.empty() ||
        aModIt->second.find(std::string()) != aModIt->second.end())
      aFile << "*"; // import whole module
    else {
      // import specific features
      std::set<std::string>::const_iterator anObjIt = aModIt->second.begin();
      aFile << *anObjIt;
      for (++anObjIt; anObjIt != aModIt->second.end(); ++anObjIt)
        aFile << ", " << *anObjIt;
    }
    aFile << std::endl;
  }
  if (!myModules.empty())
    aFile << std::endl;

  aFile << "from salome.shaper import model" << std::endl << std::endl;
  aFile << "model.begin()" << std::endl;

  // dump collected data
  aFile << myFullDump.str();
  aFile << myDumpBuffer.str();

  // standard footer
  aFile << "model.end()" << std::endl;

  aFile.close();
  clear();

  return true;
}

void ModelHighAPI_Dumper::importModule(const std::string& theModuleName,
                                       const std::string& theObject)
{
  myModules[theModuleName].insert(theObject);
}

void ModelHighAPI_Dumper::dumpEntitySetName()
{
  const LastDumpedEntity& aLastDumped = myEntitiesStack.top();

  // dump "setName" for the entity
  if (aLastDumped.myUserName) {
    EntityName& anEntityNames = myNames[aLastDumped.myEntity];
    if (!anEntityNames.myIsDefault)
      myDumpBuffer << anEntityNames.myCurrentName << ".setName(\""
                   << anEntityNames.myUserName << "\")" << std::endl;
    // don't dump "setName" for the entity twice
    anEntityNames.myUserName.clear();
    anEntityNames.myIsDefault = true;
  }
  // dump "setName" for results
  std::list<ResultPtr>::const_iterator aResIt = aLastDumped.myResults.begin();
  std::list<ResultPtr>::const_iterator aResEnd = aLastDumped.myResults.end();
  for (; aResIt != aResEnd; ++aResIt) {
    // set result name
    EntityName& anEntityNames = myNames[*aResIt];
    if (!anEntityNames.myIsDefault) {
      *this << *aResIt;
      myDumpBuffer << ".setName(\"" << anEntityNames.myUserName << "\")" << std::endl;
      // don't dump "setName" for the entity twice
      anEntityNames.myUserName.clear();
      anEntityNames.myIsDefault = true;
    }
    // set result color
    if (!isDefaultColor(*aResIt)) {
      AttributeIntArrayPtr aColor = (*aResIt)->data()->intArray(ModelAPI_Result::COLOR_ID());
      if (aColor && aColor->isInitialized()) {
        *this << *aResIt;
        myDumpBuffer << ".setColor(" << aColor->value(0) << ", " << aColor->value(1)
                     << ", " << aColor->value(2) << ")" << std::endl;
      }
    }
    // set result deflection
    if (!isDefaultDeflection(*aResIt)) {
      AttributeDoublePtr aDeflectionAttr =
        (*aResIt)->data()->real(ModelAPI_Result::DEFLECTION_ID());
      if(aDeflectionAttr.get() && aDeflectionAttr->isInitialized()) {
        *this << *aResIt;
        myDumpBuffer << ".setDeflection(" << aDeflectionAttr->value() << ")" << std::endl;
      }
    }
    // set result transparency
    if (!isDefaultTransparency(*aResIt)) {
      AttributeDoublePtr aTransparencyAttr =
        (*aResIt)->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
      if(aTransparencyAttr.get() && aTransparencyAttr->isInitialized()) {
        *this << *aResIt;
        myDumpBuffer << ".setTransparency(" << aTransparencyAttr->value() << ")" << std::endl;
      }
    }
  }

  myNames[aLastDumped.myEntity].myIsDumped = true;
  myEntitiesStack.pop();
}

bool ModelHighAPI_Dumper::isDumped(const EntityPtr& theEntity) const
{
  EntityNameMap::const_iterator aFound = myNames.find(theEntity);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theEntity);
  return (aFound != myNames.end() && aFound->second.myIsDumped) ||
         myFeaturesToSkip.find(aFeature) != myFeaturesToSkip.end();
}

bool ModelHighAPI_Dumper::isDumped(const AttributeRefAttrPtr& theRefAttr) const
{
  FeaturePtr aFeature;
  if (theRefAttr->isObject())
    aFeature = ModelAPI_Feature::feature(theRefAttr->object());
  else
    aFeature = ModelAPI_Feature::feature(theRefAttr->attr()->owner());
  return aFeature && isDumped(EntityPtr(aFeature));
}

bool ModelHighAPI_Dumper::isDumped(const AttributeRefListPtr& theRefList) const
{
  std::list<ObjectPtr> aRefs = theRefList->list();
  std::list<ObjectPtr>::iterator anIt = aRefs.begin();
  for (; anIt != aRefs.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    if (aFeature && !isDumped(EntityPtr(aFeature)))
      return false;
  }
  return true;
}

bool ModelHighAPI_Dumper::isDefaultColor(const ResultPtr& theResult) const
{
  AttributeIntArrayPtr aColor = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
  if (!aColor || !aColor->isInitialized())
    return true;

  std::string aSection, aName, aDefault;
  theResult->colorConfigInfo(aSection, aName, aDefault);

  // dump current color
  std::ostringstream aColorInfo;
  aColorInfo << aColor->value(0) << "," << aColor->value(1) << "," << aColor->value(2);

  return aDefault == aColorInfo.str();
}

bool ModelHighAPI_Dumper::isDefaultDeflection(const ResultPtr& theResult) const
{
  AttributeDoublePtr aDeflectionAttr = theResult->data()->real(ModelAPI_Result::DEFLECTION_ID());
  if(!aDeflectionAttr || !aDeflectionAttr->isInitialized()) {
    return true;
  }

  double aCurrent = aDeflectionAttr->value();
  double aDefault = -1;

  bool isConstruction = false;
  std::string aResultGroup = theResult->groupName();
  if (aResultGroup == ModelAPI_ResultConstruction::group())
    isConstruction = true;
  else if (aResultGroup == ModelAPI_ResultBody::group()) {
    GeomShapePtr aGeomShape = theResult->shape();
    if (aGeomShape.get()) {
      // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
      // correction of deviation for them should not influence to the application performance
      GeomAPI_ShapeExplorer anExp(aGeomShape, GeomAPI_Shape::FACE);
      isConstruction = !anExp.more();
    }
  }
  if (isConstruction)
    aDefault = Config_PropManager::real("Visualization", "construction_deflection");
  else
    aDefault = Config_PropManager::real("Visualization", "body_deflection");

  return fabs(aCurrent - aDefault) < 1.e-12;
}

bool ModelHighAPI_Dumper::isDefaultTransparency(const ResultPtr& theResult) const
{
  AttributeDoublePtr anAttribute = theResult->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
  if(!anAttribute || !anAttribute->isInitialized()) {
    return true;
  }
  return fabs(anAttribute->value()) < 1.e-12;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const char theChar)
{
  myDumpBuffer << theChar;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const char* theString)
{
  myDumpBuffer << theString;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const std::string& theString)
{
  myDumpBuffer << theString;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const bool theValue)
{
  myDumpBuffer << (theValue ? "True" : "False");
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const int theValue)
{
  myDumpBuffer << theValue;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const double theValue)
{
  myDumpBuffer << theValue;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  importModule("GeomAPI", "GeomAPI_Pnt");
  myDumpBuffer << "GeomAPI_Pnt(" << thePoint->x() << ", "
               << thePoint->y() << ", " << thePoint->z() << ")";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const std::shared_ptr<GeomAPI_Dir>& theDir)
{
  importModule("GeomAPI", "GeomAPI_Dir");
  myDumpBuffer << "GeomAPI_Dir(" << theDir->x() << ", "
               << theDir->y() << ", " << theDir->z() << ")";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<GeomDataAPI_Dir>& theDir)
{
  myDumpBuffer << theDir->x() << ", " << theDir->y() << ", " << theDir->z();
  return *this;
}

static void dumpArray(std::ostringstream& theOutput, int theSize,
                      double* theValues, std::string* theTexts)
{
  for (int i = 0; i < theSize; ++i) {
    if (i > 0)
      theOutput << ", ";
    if (theTexts[i].empty())
      theOutput << theValues[i];
    else
      theOutput << "\"" << theTexts[i] << "\"";
  }
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<GeomDataAPI_Point>& thePoint)
{
  static const int aSize = 3;
  double aValues[aSize] = {thePoint->x(), thePoint->y(), thePoint->z()};
  std::string aTexts[aSize] = {thePoint->textX(), thePoint->textY(), thePoint->textZ()};
  dumpArray(myDumpBuffer, aSize, aValues, aTexts);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<GeomDataAPI_Point2D>& thePoint)
{
  static const int aSize = 2;
  double aValues[aSize] = {thePoint->x(), thePoint->y()};
  std::string aTexts[aSize] = {thePoint->textX(), thePoint->textY()};
  dumpArray(myDumpBuffer, aSize, aValues, aTexts);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeBoolean>& theAttrBool)
{
  myDumpBuffer << (theAttrBool->value() ? "True" : "False");
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeInteger>& theAttrInt)
{
  std::string aText = theAttrInt->text();
  if (aText.empty())
    myDumpBuffer << theAttrInt->value();
  else
    myDumpBuffer << "\"" << aText << "\"";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeDouble>& theAttrReal)
{
  std::string aText = theAttrReal->text();
  if (aText.empty())
    myDumpBuffer << theAttrReal->value();
  else
    myDumpBuffer << "\"" << aText << "\"";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeString>& theAttrStr)
{
  myDumpBuffer << "\"" << theAttrStr->value() << "\"";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const FolderPtr& theFolder)
{
  myDumpBuffer << name(theFolder);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const FeaturePtr& theEntity)
{
  myDumpBuffer << name(theEntity);

  if (!myNames[theEntity].myIsDumped) {
    bool isUserDefinedName = !myNames[theEntity].myIsDefault;
    // store results if they have user-defined names or colors
    std::list<ResultPtr> aResultsWithNameOrColor;
    const std::list<ResultPtr>& aResults = theEntity->results();
    std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
    for (; aResIt != aResults.end(); ++aResIt) {
      if (!myNames[*aResIt].myIsDefault || !isDefaultColor(*aResIt) ||
          !isDefaultDeflection(*aResIt) || !isDefaultTransparency(*aResIt))
        aResultsWithNameOrColor.push_back(*aResIt);

      ResultCompSolidPtr aCompSolid =
          std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aResIt);
      if (aCompSolid) {
        int aNbSubs = aCompSolid->numberOfSubs();
        for (int i = 0; i < aNbSubs; ++i) {
          ResultPtr aCurRes = aCompSolid->subResult(i);
          if (!myNames[aCurRes].myIsDefault || !isDefaultColor(aCurRes) ||
              !isDefaultDeflection(aCurRes) || !isDefaultTransparency(aCurRes))
            aResultsWithNameOrColor.push_back(aCurRes);
        }
      }
    }
    // store just dumped entity to stack
    if (myEntitiesStack.empty() || myEntitiesStack.top().myEntity != theEntity)
      myEntitiesStack.push(
          LastDumpedEntity(theEntity, isUserDefinedName, aResultsWithNameOrColor));
  }

  // remove entity from the list of not dumped items
  myNotDumpedEntities.erase(theEntity);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const ResultPtr& theResult)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theResult);
  int anIndex = 0;
  int aSubIndex = -1;
  std::list<ResultPtr> aResults = aFeature->results();
  for(std::list<ResultPtr>::const_iterator
      anIt = aResults.cbegin(); anIt != aResults.cend(); ++anIt, ++anIndex) {
    if(theResult->isSame(*anIt)) {
      break;
    }

    ResultCompSolidPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*anIt);
    if (aCompSolid) {
      int aNbSubs = aCompSolid->numberOfSubs();
      for (aSubIndex = 0; aSubIndex < aNbSubs; ++aSubIndex)
        if (theResult->isSame(aCompSolid->subResult(aSubIndex)))
          break;
      if (aSubIndex < aNbSubs)
        break;
      aSubIndex = -1;
    }
  }

  myDumpBuffer << name(aFeature);
  if(anIndex == 0) {
    myDumpBuffer << ".result()";
  } else {
    myDumpBuffer << ".results()[" << anIndex << "]";
  }
  if (aSubIndex >= 0) {
    myDumpBuffer << ".subResult(" << aSubIndex << ")";
  }
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const ObjectPtr& theObject)
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if(aFeature.get()) {
    myDumpBuffer << name(aFeature);
    return *this;
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if(aResult.get()) {
    *this << aResult;
    return *this;
  }

  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const AttributePtr& theAttr)
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttr->owner());

  std::string aWrapperPrefix, aWrapperSuffix;
  // Check the attribute belongs to copied (in multi-translation or multi-rotation) feature.
  // In this case we need to cast explicitly feature to appropriate type.
  AttributeBooleanPtr isCopy = anOwner->boolean("Copy");
  if (isCopy.get() && isCopy->value()) {
    aWrapperPrefix = featureWrapper(anOwner) + "(";
    aWrapperSuffix = ")";
    importModule("SketchAPI");
  }

  myDumpBuffer << aWrapperPrefix << name(anOwner) << aWrapperSuffix
               << "." << attributeGetter(anOwner, theAttr->id()) << "()";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeRefAttr>& theRefAttr)
{
  if (theRefAttr->isObject())
    *this << theRefAttr->object();
  else
    *this << theRefAttr->attr();
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeRefAttrList>& theRefAttrList)
{
  myDumpBuffer << "[";
  std::list<std::pair<ObjectPtr, AttributePtr> > aList = theRefAttrList->list();
  bool isAdded = false;
  std::list<std::pair<ObjectPtr, AttributePtr> >::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (isAdded)
      myDumpBuffer << ", ";
    else
      isAdded = true;
    if (anIt->first)
      *this << anIt->first;
    else if (anIt->second)
      * this << anIt->second;
  }
  myDumpBuffer << "]";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeReference>& theReference)
{
  *this << theReference->value();
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeRefList>& theRefList)
{
  static const int aThreshold = 2;
  // if number of elements in the list if greater than a threshold,
  // dump it in a separate line with specific name
  std::string aDumped = myDumpBuffer.str();
  if (aDumped.empty() || theRefList->size() <= aThreshold) {
    myDumpBuffer << "[";
    std::list<ObjectPtr> aList = theRefList->list();
    bool isAdded = false;
    std::list<ObjectPtr>::const_iterator anIt = aList.begin();
    for (; anIt != aList.end(); ++anIt) {
      if (isAdded)
        myDumpBuffer << ", ";
      else
        isAdded = true;

      *this << *anIt;
    }
    myDumpBuffer << "]";
  } else {
    // clear buffer and store list "as is"
    myDumpBuffer.str("");
    *this << theRefList;
    // save buffer and clear it again
    std::string aDumpedList = myDumpBuffer.str();
    myDumpBuffer.str("");
    // obtain name of list
    FeaturePtr anOwner = ModelAPI_Feature::feature(theRefList->owner());
    std::string aListName = name(anOwner) + "_objects";
    // store all previous data
    myDumpBuffer << aListName << " = " << aDumpedList << std::endl
                 << aDumped << aListName;
  }
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeSelection>& theAttrSelect)
{
  myDumpBuffer << "model.selection(";

  if(!theAttrSelect->isInitialized()) {
    myDumpBuffer << ")";
    return *this;
  }

  GeomShapePtr aShape = theAttrSelect->value();
  if(!aShape.get()) {
    aShape = theAttrSelect->context()->shape();
  }

  if(!aShape.get()) {
    myDumpBuffer << ")";
    return *this;
  }

  myDumpBuffer << "\"" << aShape->shapeTypeStr() << "\", \"" <<
    theAttrSelect->namingName() << "\")";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeSelectionList>& theAttrSelList)
{
  myDumpBuffer << "[";

  GeomShapePtr aShape;
  std::string aShapeTypeStr;

  bool isAdded = false;

  for(int anIndex = 0; anIndex < theAttrSelList->size(); ++anIndex) {
    AttributeSelectionPtr anAttribute = theAttrSelList->value(anIndex);
    aShape = anAttribute->value();
    if(!aShape.get()) {
      ResultPtr aContext = anAttribute->context();
      if (aContext.get())
        aShape = aContext->shape();
    }

    if(!aShape.get()) {
      continue;
    }

    if(isAdded) {
      myDumpBuffer << ", ";
    } else {
      isAdded = true;
    }
    myDumpBuffer << "model.selection(\"" <<
      aShape->shapeTypeStr() << "\", \"" << anAttribute->namingName() << "\")";
  }

  myDumpBuffer << "]";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
  const std::shared_ptr<ModelAPI_AttributeStringArray>& theArray)
{
  myDumpBuffer<<"[";
  for(int anIndex = 0; anIndex < theArray->size(); ++anIndex) {
    if (anIndex != 0)
      myDumpBuffer<<", ";

    myDumpBuffer<<"\""<<theArray->value(anIndex)<<"\"";
  }

  myDumpBuffer<<"]";
  return *this;
}

/// Dump std::endl
ModelHighAPI_Dumper& operator<<(ModelHighAPI_Dumper& theDumper,
                                std::basic_ostream<char>& (*theEndl)(std::basic_ostream<char>&))
{
  theDumper.myDumpBuffer << theEndl;

  if (!theDumper.myEntitiesStack.empty()) {
    bool isCopy;
    // all copies have been stored into stack, pop them all
    do {
      isCopy = false;
      // Name for composite feature is dumped when all sub-entities are dumped
      // (see method ModelHighAPI_Dumper::processSubs).
      const ModelHighAPI_Dumper::LastDumpedEntity& aLastDumped = theDumper.myEntitiesStack.top();
      CompositeFeaturePtr aComposite =
          std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aLastDumped.myEntity);
      if (!aComposite) {
        theDumper.dumpEntitySetName();
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aLastDumped.myEntity);
        if (aFeature) {
          AttributeBooleanPtr aCopyAttr = aFeature->boolean("Copy");
          isCopy = aCopyAttr.get() && aCopyAttr->value();
        }
      }
    } while (isCopy && !theDumper.myEntitiesStack.empty());
  }

  // store all not-dumped entities first
  std::set<EntityPtr> aNotDumped = theDumper.myNotDumpedEntities;
  std::string aBufCopy = theDumper.myDumpBuffer.str();
  theDumper.clear(true);
  std::set<EntityPtr>::const_iterator anIt = aNotDumped.begin();
  for (; anIt != aNotDumped.end(); ++anIt) {
    // if the feature is composite, dump it with all subs
    CompositeFeaturePtr aCompFeat =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*anIt);
    if (aCompFeat)
      theDumper.process(aCompFeat, true);
    else {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
      theDumper.dumpFeature(aFeature, true);
      // dump the Projection feature which produces this "Copy" entity
      AttributeBooleanPtr aCopyAttr = aFeature->boolean("Copy");
      if (aCopyAttr.get() && aCopyAttr->value())
      {
        const std::set<AttributePtr>& aRefs = aFeature->data()->refsToMe();
        std::set<AttributePtr>::iterator aRefIt = aRefs.begin();
        for (; aRefIt != aRefs.end(); ++aRefIt)
          if ((*aRefIt)->id() == "ProjectedFeature")
          { // process projection only
            FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
            if (anOwner && !theDumper.isDumped(EntityPtr(anOwner)))
              theDumper.dumpFeature(anOwner, true);
          }
      }
    }
  }

  // avoid multiple empty lines
  size_t anInd = std::string::npos;
  while ((anInd = aBufCopy.find("\n\n\n")) != std::string::npos)
    aBufCopy.erase(anInd, 1);
  // then store currently dumped string
  theDumper.myFullDump << aBufCopy;

  // now, store all postponed features
  theDumper.dumpPostponed();

  return theDumper;
}
