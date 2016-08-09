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
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
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

#include <algorithm>
#include <fstream>

//#define DUMP_USER_DEFINED_NAMES

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
  myDumpBuffer = std::ostringstream();
  myDumpBuffer << std::setprecision(16);

  clearNotDumped();

  if (!bufferOnly) {
    myFullDump = std::ostringstream();
    myFullDump << std::setprecision(16);

    myNames.clear();
    myModules.clear();
    myLastEntityWithName = EntityPtr();
  }
}

void ModelHighAPI_Dumper::clearNotDumped()
{
  myNotDumpedEntities.clear();
}

const std::string& ModelHighAPI_Dumper::name(const EntityPtr& theEntity)
{
  EntityNameMap::const_iterator aFound = myNames.find(theEntity);
  if (aFound != myNames.end())
    return aFound->second.first;

  // entity is not found, store it
  std::string aName;
  bool isNameDefined = false;
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theEntity);
  if (aFeature) {
    aName = aFeature->name();
    isNameDefined = !aName.empty();

    if (!isNameDefined) {
      static long anIndex = 0;
      // set default name: feature ID + index
      std::ostringstream aConverter;
      aConverter << aFeature->getKind() << "_" << ++anIndex;
      aName = aConverter.str();
      std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
    }
  }

  myNames[theEntity] = std::pair<std::string, bool>(aName, isNameDefined);
  myNotDumpedEntities.insert(theEntity);
  return myNames[theEntity].first;
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

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_Document>& theDoc,
                                  const std::string& theFileName)
{
  // dump top level document feature
  static const std::string aDocName("partSet");
  myNames[theDoc] = std::pair<std::string, bool>(aDocName, false);
  *this << aDocName << " = model.moduleDocument()" << std::endl;

  // dump subfeatures and store result to file
  return process(theDoc) && exportTo(theFileName);
}

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_Document>& theDoc)
{
  bool isOk = true;
  // dump all features
  std::list<FeaturePtr> aFeatures = theDoc->allFeatures();
  std::list<FeaturePtr>::const_iterator aFeatIt = aFeatures.begin();
  for (; aFeatIt != aFeatures.end(); ++aFeatIt) {
    if (!isDumped(*aFeatIt))
      dumpFeature(*aFeatIt);

    // iteratively process composite features
    CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aFeatIt);
    if (!aCompFeat)
      continue;

    // sub-part is processed independently, because it provides separate document
    if ((*aFeatIt)->getKind() == PartSetPlugin_Part::ID()) {
      ResultPartPtr aPartResult =
          std::dynamic_pointer_cast<ModelAPI_ResultPart>((*aFeatIt)->lastResult());
      if (!aPartResult)
        continue;
      DocumentPtr aSubDoc = aPartResult->partDoc();
      // set name of document equal to part name
      myNames[aSubDoc] = myNames[*aFeatIt];

      isOk = process(aSubDoc) && isOk;
    } else
      isOk = process(aCompFeat) && isOk;
  }
  return isOk;
}

bool ModelHighAPI_Dumper::process(const std::shared_ptr<ModelAPI_CompositeFeature>& theComposite)
{
  // dump all sub-features;
  int aNbSubs = theComposite->numberOfSubs();
  for (int anIndex = 0; anIndex < aNbSubs; ++anIndex) {
    FeaturePtr aFeature = theComposite->subFeature(anIndex);
    if (!isDumped(aFeature))
      dumpFeature(aFeature, true);
  }
  // dump command to update model
  myDumpBuffer << "model.do()" << std::endl;
  return true;
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
  if (!myLastEntityWithName)
    return;

#ifdef DUMP_USER_DEFINED_NAMES
  const std::string& aName = name(myLastEntityWithName);
  myDumpBuffer << aName;
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(myLastEntityWithName);
  if (aFeature)
    myDumpBuffer << ".feature()";
  myDumpBuffer << ".data().setName(\"" << aName << "\")" << std::endl;
#endif
  myLastEntityWithName = EntityPtr();
}

bool ModelHighAPI_Dumper::isDumped(const EntityPtr& theEntity) const
{
  EntityNameMap::const_iterator aFound = myNames.find(theEntity);
  return aFound != myNames.end();
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

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const EntityPtr& theEntity)
{
  myDumpBuffer << name(theEntity);
  if (myNames[theEntity].second)
    myLastEntityWithName = theEntity;
  myNotDumpedEntities.erase(theEntity);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeRefAttr>& theRefAttr)
{
  if (theRefAttr->isObject()) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theRefAttr->object());
    myDumpBuffer << name(aFeature);
  } else {
    AttributePtr anAttr = theRefAttr->attr();
    FeaturePtr anOwner = ModelAPI_Feature::feature(anAttr->owner());
    myDumpBuffer << name(anOwner) << "." << attributeGetter(anOwner, anAttr->id()) << "()";
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
  theDumper.dumpEntitySetName();

  // store all not-dumped entities first
  std::set<EntityPtr> aNotDumped = theDumper.myNotDumpedEntities;
  std::string aBufCopy = theDumper.myDumpBuffer.str();
  theDumper.clear(true);
  std::set<EntityPtr>::const_iterator anIt = aNotDumped.begin();
  for (; anIt != aNotDumped.end(); ++anIt) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    theDumper.dumpFeature(aFeature, true);
  }

  // then store currently dumped string
  theDumper.myFullDump << aBufCopy;

  return theDumper;
}
