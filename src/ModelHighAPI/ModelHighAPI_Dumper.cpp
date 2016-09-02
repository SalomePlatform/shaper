// Copyright (C) 2016-20xx CEA/DEN, EDF R&D -->

// File:        ModelHighAPI_Dumper.cpp
// Created:     1 August 2016
// Author:      Artem ZHIDKOV

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Dumper.h"

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

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
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Entity.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultPart.h>

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
  }
}

void ModelHighAPI_Dumper::clearNotDumped()
{
  myNotDumpedEntities.clear();
}

const std::string& ModelHighAPI_Dumper::name(const EntityPtr& theEntity,
                                             bool theSaveNotDumped,
                                             bool theUseEntityName)
{
  EntityNameMap::const_iterator aFound = myNames.find(theEntity);
  if (aFound != myNames.end())
    return aFound->second.myCurrentName;

  // entity is not found, store it
  std::string aName;
  bool isDefaultName = false;
  std::ostringstream aDefaultName;
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theEntity);
  if (aFeature) {
    aName = aFeature->name();
    const std::string& aKind = aFeature->getKind();
    DocumentPtr aDoc = aFeature->document();
    int& aNbFeatures = myFeatureCount[aDoc][aKind];
    aNbFeatures += 1;

    size_t anIndex = aName.find(aKind);
    if (anIndex == 0 && aName[aKind.length()] == '_') { // name starts with "FeatureKind_"
      std::string anIdStr = aName.substr(aKind.length() + 1);
      int anId = std::stoi(anIdStr);

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
  if (theSaveNotDumped)
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
  const std::string& aKind = theFeature->getKind();
  DocumentPtr aDoc = theFeature->document();
  int aNbFeatures = myFeatureCount[aDoc][aKind];
  std::ostringstream aNameStream;
  aNameStream << aKind << "_" << aNbFeatures;
  std::string aFeatureName = aNameStream.str();

  // Save only names of results which is not correspond to default feature name
  const std::list<ResultPtr>& aResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
  for (int i = 1; aResIt != aResults.end(); ++aResIt, ++i) {
    bool isUserDefined = true;
    std::string aResName = (*aResIt)->data()->name();
    size_t anIndex = aResName.find(aFeatureName);
    if (anIndex == 0) {
      std::string aSuffix = aResName.substr(aFeatureName.length());
      if (aSuffix.empty() && i == 1) // first result may not constain index in the name
        isUserDefined = false;
      else {
        if (aSuffix[0] == '_' && std::stoi(aSuffix.substr(1)) == i)
          isUserDefined = false;
      }
    }

    myNames[*aResIt] = EntityName(aResName,
        (isUserDefined ? aResName : std::string()), !isUserDefined);
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
  std::list<FeaturePtr> aFeatures = theDoc->allFeatures();
  std::list<FeaturePtr>::const_iterator aFeatIt = aFeatures.begin();
  // firstly, dump all parameters
  for (; aFeatIt != aFeatures.end(); ++ aFeatIt)
    dumpParameter(*aFeatIt);
  // dump all other features
  for (aFeatIt = aFeatures.begin(); aFeatIt != aFeatures.end(); ++aFeatIt) {
    CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aFeatIt);
    if (aCompFeat) // iteratively process composite features
      isOk = process(aCompFeat) && isOk;
    else if (!isDumped(*aFeatIt)) // dump common feature 
      dumpFeature(*aFeatIt);
  }
  return isOk;
}

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite, bool isForce)
{
  // increase composite features stack
  ++gCompositeStackDepth;
  // dump composite itself
  if (!isDumped(theComposite) || isForce)
    dumpFeature(FeaturePtr(theComposite), isForce);

  // sub-part is processed independently, because it provides separate document
  if (theComposite->getKind() == PartSetPlugin_Part::ID()) {
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
    return process(aSubDoc);
  }

  // dump sub-features
  bool isOk = processSubs(theComposite);
  // decrease composite features stack
  --gCompositeStackDepth;

  return isOk;
}

bool ModelHighAPI_Dumper::processSubs(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite,
                                      bool theDumpModelDo)
{
  bool isOk = true;
  // dump all sub-features;
  bool isSubDumped = false;
  int aNbSubs = theComposite->numberOfSubs();
  for (int anIndex = 0; anIndex < aNbSubs; ++anIndex) {
    FeaturePtr aFeature = theComposite->subFeature(anIndex);
    if (isDumped(aFeature))
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
  return isOk;
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

  aFile << "import model" << std::endl << std::endl;
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
  }

  myEntitiesStack.pop();
}

bool ModelHighAPI_Dumper::isDumped(const EntityPtr& theEntity) const
{
  EntityNameMap::const_iterator aFound = myNames.find(theEntity);
  return aFound != myNames.end();
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

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const FeaturePtr& theEntity)
{
  myDumpBuffer << name(theEntity);

  bool isUserDefinedName = !myNames[theEntity].myIsDefault;
  // store results if they have user-defined names or colors
  std::list<ResultPtr> aResultsWithNameOrColor;
  const std::list<ResultPtr>& aResults = theEntity->results();
  std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
  for (; aResIt != aResults.end(); ++aResIt)
    if (!myNames[*aResIt].myIsDefault || !isDefaultColor(*aResIt))
      aResultsWithNameOrColor.push_back(*aResIt);
  // store just dumped entity to stack
  myEntitiesStack.push(LastDumpedEntity(theEntity, isUserDefinedName, aResultsWithNameOrColor));

  // remove entity from the list of not dumped items
  myNotDumpedEntities.erase(theEntity);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const ResultPtr& theResult)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theResult);
  int anIndex = 0;
  std::list<ResultPtr> aResults = aFeature->results();
  for(std::list<ResultPtr>::const_iterator anIt = aResults.cbegin(); anIt != aResults.cend(); ++anIt, ++anIndex) {
    if(theResult->isSame(*anIt)) {
      break;
    }
  }
  myDumpBuffer << name(aFeature) << ".result()[" << anIndex << "]";
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

  myDumpBuffer << "\"" << aShape->shapeTypeStr() << "\", \"" << theAttrSelect->namingName() << "\")";
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
      aShape = anAttribute->context()->shape();
    }

    if(!aShape.get()) {
      continue;
    }

    if(isAdded) {
      myDumpBuffer << ", ";
    } else {
      isAdded = true;
    }
    myDumpBuffer << "model.selection(\"" << aShape->shapeTypeStr() << "\", \"" << anAttribute->namingName() << "\")";
  }

  myDumpBuffer << "]";
  return *this;
}

/// Dump std::endl
MODELHIGHAPI_EXPORT
ModelHighAPI_Dumper& operator<<(ModelHighAPI_Dumper& theDumper,
                                std::basic_ostream<char>& (*theEndl)(std::basic_ostream<char>&))
{
  theDumper.myDumpBuffer << theEndl;

  if (!theDumper.myEntitiesStack.empty()) {
    // Name for composite feature is dumped when all sub-entities are dumped
    // (see method ModelHighAPI_Dumper::processSubs).
    const ModelHighAPI_Dumper::LastDumpedEntity& aLastDumped = theDumper.myEntitiesStack.top();
    CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aLastDumped.myEntity);
    if (!aComposite)
      theDumper.dumpEntitySetName();
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
    }
  }

  // avoid multiple empty lines
  size_t anInd = std::string::npos;
  while ((anInd = aBufCopy.find("\n\n\n")) != std::string::npos)
    aBufCopy.erase(anInd, 1);
  // then store currently dumped string
  theDumper.myFullDump << aBufCopy;

  return theDumper;
}
