// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ModelHighAPI_Dumper.h"

#include <Config_PropManager.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAlgoAPI_NExplode.h>

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
#include <ModelAPI_FiltersFeature.h>
#include <ModelAPI_Folder.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <ModelGeomAlgo_Shape.h>

#include <PartSetPlugin_Part.h>

#include <OSD_OpenFile.hxx>

#include <fstream>

// ===========    Implementation of storage of dumped data    ===========
static const int THE_DUMP_PRECISION = 16;

class ModelHighAPI_Dumper::DumpStorageBuffer : public ModelHighAPI_Dumper::DumpStorage
{
public:
  void addStorage(const ModelHighAPI_Dumper::DumpStoragePtr& theStorage)
  { myStorageArray.push_back(theStorage); }

  void clear() { myStorageArray.clear(); }

  bool isBufferEmpty()
  {
    return myStorageArray.empty() || myStorageArray.front()->buffer().str().empty();
  }

  void mergeBuffer()
  {
    std::list<ModelHighAPI_Dumper::DumpStoragePtr>::iterator anIt = myStorageArray.begin();
    for (; anIt != myStorageArray.end(); ++anIt) {
      // avoid multiple empty lines
      std::string aBuf = (*anIt)->buffer().str();
      size_t anInd = std::string::npos;
      while ((anInd = aBuf.find("\n\n\n")) != std::string::npos)
        aBuf.erase(anInd, 1);

      (*anIt)->fullDump() << aBuf;
      (*anIt)->buffer().str("");
    }
  }

  void write(const std::string& theValue)
  {
    if (myStorageArray.empty())
      addStorage(DumpStoragePtr(new DumpStorage));

    std::list<ModelHighAPI_Dumper::DumpStoragePtr>::iterator anIt = myStorageArray.begin();
    for (; anIt != myStorageArray.end(); ++anIt)
      (*anIt)->buffer() << theValue;
  }

  DumpStorageBuffer& operator<<(const char theChar)
  {
    std::ostringstream out;
    out << theChar;
    write(out.str());
    return *this;
  }

  DumpStorageBuffer& operator<<(const char* theString)
  {
    write(theString);
    return *this;
  }

  DumpStorageBuffer& operator<<(const std::string& theString)
  {
    write(theString);
    return *this;
  }

  DumpStorageBuffer& operator<<(const bool theValue)
  {
    std::ostringstream out;
    out << theValue;
    write(out.str());
    return *this;
  }

  DumpStorageBuffer& operator<<(const int theValue)
  {
    std::ostringstream out;
    out << theValue;
    write(out.str());
    return *this;
  }

  DumpStorageBuffer& operator<<(const double theValue)
  {
    std::ostringstream out;
    out << std::setprecision(THE_DUMP_PRECISION) << theValue;
    write(out.str());
    return *this;
  }
  /// Dump std::endl
  friend
  DumpStorageBuffer& operator<<(DumpStorageBuffer& theBuffer,
                                std::basic_ostream<char>& (*theEndl)(std::basic_ostream<char>&))
  {
    theBuffer.write("\n");
    return theBuffer;
  }

  void dumpArray(int theSize, double* theValues, std::string* theTexts)
  {
    std::ostringstream anOutput;
    anOutput << std::setprecision(THE_DUMP_PRECISION);
    for (int i = 0; i < theSize; ++i) {
      if (i > 0)
        anOutput << ", ";
      if (theTexts[i].empty())
        anOutput << theValues[i];
      else
        anOutput << "\"" << theTexts[i] << "\"";
    }
    write(anOutput.str());
  }

  virtual void write(const std::shared_ptr<ModelAPI_AttributeSelection>& theAttrSelect)
  {
    if (myStorageArray.empty())
      addStorage(DumpStoragePtr(new DumpStorage));

    std::list<ModelHighAPI_Dumper::DumpStoragePtr>::iterator anIt = myStorageArray.begin();
    for (; anIt != myStorageArray.end(); ++anIt)
      (*anIt)->write(theAttrSelect);
  }

  virtual void reserveBuffer()
  {
    std::list<ModelHighAPI_Dumper::DumpStoragePtr>::iterator anIt = myStorageArray.begin();
    for (; anIt != myStorageArray.end(); ++anIt)
      (*anIt)->reserveBuffer();
  }

  virtual void restoreReservedBuffer()
  {
    std::list<ModelHighAPI_Dumper::DumpStoragePtr>::iterator anIt = myStorageArray.begin();
    for (; anIt != myStorageArray.end(); ++anIt)
      (*anIt)->restoreReservedBuffer();
  }

  virtual bool exportTo(const std::string& theFilename, const ModulesSet& theUsedModules)
  {
    static const std::string THE_EXT = ".py";
    std::string aFilenameBase = theFilename;
    if (aFilenameBase.rfind(THE_EXT) == aFilenameBase.size() - THE_EXT.size())
      aFilenameBase = aFilenameBase.substr(0, aFilenameBase.size() - THE_EXT.size());

    bool isOk = true;
    std::list<ModelHighAPI_Dumper::DumpStoragePtr>::iterator anIt = myStorageArray.begin();
    for (; anIt != myStorageArray.end(); ++anIt) {
      std::string aFilename = aFilenameBase + (*anIt)->myFilenameSuffix + THE_EXT;
      isOk = (*anIt)->exportTo(aFilename, theUsedModules) && isOk;
    }
    return isOk;
  }

private:
  std::list<ModelHighAPI_Dumper::DumpStoragePtr> myStorageArray;
};


ModelHighAPI_Dumper::DumpStorage::DumpStorage(const DumpStorage& theOther)
  : myFilenameSuffix(theOther.myFilenameSuffix),
    myDumpBufferHideout(theOther.myDumpBufferHideout)
{
  myFullDump.str(theOther.myFullDump.str());
  myDumpBuffer.str(theOther.myDumpBuffer.str());
}

const ModelHighAPI_Dumper::DumpStorage&
ModelHighAPI_Dumper::DumpStorage::operator=(const ModelHighAPI_Dumper::DumpStorage& theOther)
{
  myFilenameSuffix = theOther.myFilenameSuffix;
  myFullDump.str(theOther.myFullDump.str());
  myDumpBuffer.str(theOther.myDumpBuffer.str());
  myDumpBufferHideout = theOther.myDumpBufferHideout;
  return *this;
}

void ModelHighAPI_Dumper::DumpStorage::reserveBuffer()
{
  myDumpBufferHideout.push(myDumpBuffer.str());
  myDumpBuffer.str("");
}

void ModelHighAPI_Dumper::DumpStorage::restoreReservedBuffer()
{
  myDumpBuffer << myDumpBufferHideout.top();
  myDumpBufferHideout.pop();
}

bool ModelHighAPI_Dumper::DumpStorage::exportTo(const std::string& theFilename,
                                                const ModulesSet& theUsedModules)
{
  std::ofstream aFile;
  OSD_OpenStream(aFile, theFilename.c_str(), std::ofstream::out);
  if (!aFile.is_open())
    return false;

  // standard header imported modules
  for (ModulesSet::const_iterator aModIt = theUsedModules.begin();
    aModIt != theUsedModules.end(); ++aModIt) {
    aFile << "from " << *aModIt << " import *" << std::endl;
  }
  if (!theUsedModules.empty())
    aFile << std::endl;

  aFile << "from salome.shaper import model" << std::endl << std::endl;
  aFile << "model.begin()" << std::endl;

  // dump collected data
  aFile << myFullDump.str();
  aFile << myDumpBuffer.str();

  // standard footer
  aFile << "model.end()" << std::endl;
  aFile.close();

  return true;
}

static void getShapeAndContext(const AttributeSelectionPtr& theAttrSelect,
                               GeomShapePtr& theShape, ResultPtr& theContext)
{
  if (theAttrSelect->isInitialized()) {
    theShape = theAttrSelect->value();
    theContext = theAttrSelect->context();
    if (!theShape.get())
      theShape = theContext->shape();

    if (theAttrSelect->isGeometricalSelection() &&
        theShape.get() && theShape->shapeType() == GeomAPI_Shape::COMPOUND &&
        theContext.get() && !theShape->isEqual(theContext->shape()) &&
        theContext->groupName() != ModelAPI_ResultPart::group()) {
      GeomAPI_ShapeIterator anIt(theShape);
      theShape = anIt.current();
    }
  }
}

void ModelHighAPI_Dumper::DumpStorage::write(const AttributeSelectionPtr& theAttrSelect)
{
  myDumpBuffer << "model.selection(";

  GeomShapePtr aShape;
  ResultPtr aContext;
  getShapeAndContext(theAttrSelect, aShape, aContext);

  if (aShape.get()) {
    myDumpBuffer << "\"" << aShape->shapeTypeStr() << "\", \""
                 << theAttrSelect->namingName() << "\"";
  }

  myDumpBuffer << ")";
}

static int possibleSelectionsByPoint(const GeomPointPtr& thePoint,
                                     const ResultPtr& theResult,
                                     const GeomShapePtr& theShape,
                                     const FeaturePtr& theStartFeature,
                                     const FeaturePtr& theEndFeature)
{
  DocumentPtr aDoc1 = theStartFeature->document();
  DocumentPtr aDoc2 = theEndFeature->document();

  std::list<FeaturePtr> aFeatures = aDoc1->allFeatures();
  if (aDoc1 != aDoc2) {
    std::list<FeaturePtr> anAdditionalFeatures = aDoc2->allFeatures();
    aFeatures.insert(aFeatures.end(), anAdditionalFeatures.begin(), anAdditionalFeatures.end());
  }

  CompositeFeaturePtr aLastCompositeFeature;

  std::list<FeaturePtr>::const_iterator aFIt = aFeatures.begin();
  while (aFIt != aFeatures.end() && *aFIt != theStartFeature) {
    CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aFIt);
    if (aCompFeat)
      aLastCompositeFeature = aCompFeat;
    ++aFIt;
  }

  // collect the list of composite features, containing the last feature;
  // these features should be excluded from searching,
  // because the feature cannot select sub-shapes from its parent
  std::set<FeaturePtr> aEndFeatureParents = ModelAPI_Tools::getParents(theEndFeature);

  int aNbPossibleSelections = 0;
  for (; aFIt != aFeatures.end() && *aFIt != theEndFeature; ++aFIt) {
    bool isSkipFeature = false;
    if (aLastCompositeFeature && aLastCompositeFeature->isSub(*aFIt))
      isSkipFeature = true;
    CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aFIt);
    if (aCompFeat) {
      ResultPartPtr aPartRes =
          std::dynamic_pointer_cast<ModelAPI_ResultPart>(aCompFeat->firstResult());
      if (!aPartRes)
        aLastCompositeFeature = aCompFeat;
      if (aEndFeatureParents.find(aCompFeat) != aEndFeatureParents.end()) {
        // do not process the parent for the last feature,
        // because it cannot select objects from its parent
        isSkipFeature = true;
      }
    }
    if (isSkipFeature)
      continue;

    std::list<ModelGeomAlgo_Shape::SubshapeOfResult> anApproproate;
    if (ModelGeomAlgo_Shape::findSubshapeByPoint(*aFIt, thePoint, theShape->shapeType(),
                                                 anApproproate)) {
      std::list<ModelGeomAlgo_Shape::SubshapeOfResult>::iterator anApIt = anApproproate.begin();
      for (; anApIt != anApproproate.end(); ++anApIt) {
        ++aNbPossibleSelections;

        // stop if the target shape and result are found
        GeomShapePtr aCurShape = anApIt->mySubshape;
        if (!aCurShape)
          aCurShape = anApIt->myResult->shape();

        if (anApIt->myResult->isSame(theResult) && aCurShape->isSame(theShape))
          break;
      }
    }
  }
  return aNbPossibleSelections;
}

void ModelHighAPI_Dumper::DumpStorageGeom::write(const AttributeSelectionPtr& theAttrSelect)
{
  GeomShapePtr aShape;
  ResultPtr aContext;
  getShapeAndContext(theAttrSelect, aShape, aContext);

  // how to dump selection: construction features are dumped by name always
  FeaturePtr aSelectedFeature;
  FeaturePtr aFeature = theAttrSelect->contextFeature();
  if (aShape && aContext && !aFeature)
    aSelectedFeature = ModelAPI_Feature::feature(aContext->data()->owner());
  bool isDumpByGeom = aSelectedFeature && aSelectedFeature->isInHistory();

  if (isDumpByGeom) {
    myDumpBuffer << "model.selection(\"" << aShape->shapeTypeStr();
    // check the selected item is a ResultPart;
    // in this case it is necessary to get shape with full transformation
    // for correct calculation of the middle point
    ResultPartPtr aResPart =
      std::dynamic_pointer_cast<ModelAPI_ResultPart>(theAttrSelect->context());
    if (aResPart && aShape->shapeType() == GeomAPI_Shape::COMPOUND)
      aShape = aResPart->shape();
    GeomPointPtr aMiddlePoint = aShape->middlePoint();
    // calculate number of features, which could be selected by the same point
    FeaturePtr anOwner = ModelAPI_Feature::feature(theAttrSelect->owner());
    int aNbPossibleSelections = possibleSelectionsByPoint(aMiddlePoint,
        theAttrSelect->context(), aShape, aSelectedFeature, anOwner);

    // produce the index if the number of applicable features is greater than 1
    std::string anIndex;
    if (aNbPossibleSelections > 1) {
      std::ostringstream anOutput;
      anOutput << "_" << aNbPossibleSelections;
      anIndex = anOutput.str();
    }

    myDumpBuffer << std::setprecision(THE_DUMP_PRECISION)
                 << anIndex << "\", ("
                 << aMiddlePoint->x() << ", "
                 << aMiddlePoint->y() << ", "
                 << aMiddlePoint->z() << ")";
    myDumpBuffer << ")";
  }
  else
    DumpStorage::write(theAttrSelect);
}

void ModelHighAPI_Dumper::DumpStorageWeak::write(const AttributeSelectionPtr& theAttrSelect)
{
  GeomShapePtr aShape;
  ResultPtr aContext;
  getShapeAndContext(theAttrSelect, aShape, aContext);

  bool aStandardDump = true;
  if (aShape.get() && aContext.get() &&
      aShape != aContext->shape()) { // weak naming for local selection only
    GeomAlgoAPI_NExplode aNExplode(aContext->shape(), aShape->shapeType());
    int anIndex = aNExplode.index(aShape);
    if (anIndex != 0) { // found a week-naming index, so, export it
      myDumpBuffer << "model.selection(\"" << aShape->shapeTypeStr() << "\", \""
                   << theAttrSelect->contextName(aContext) << "\", " << anIndex << ")";
      aStandardDump = false;
    }
  }
  if (aStandardDump)
    DumpStorage::write(theAttrSelect);
}
// ======================================================================


static int gCompositeStackDepth = 0;

ModelHighAPI_Dumper* ModelHighAPI_Dumper::mySelf = 0;

ModelHighAPI_Dumper::ModelHighAPI_Dumper()
  : myDumpStorage(new DumpStorageBuffer),
    myDumpPostponedInProgress(false)
{
}

ModelHighAPI_Dumper::~ModelHighAPI_Dumper()
{
  delete myDumpStorage;
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

void ModelHighAPI_Dumper::addCustomStorage(const ModelHighAPI_Dumper::DumpStoragePtr& theStorage)
{
  myDumpStorage->addStorage(theStorage);
}

void ModelHighAPI_Dumper::clearCustomStorage()
{
  myDumpStorage->clear();

  myNames.clear();
  myModules.clear();
  myFeatureCount.clear();
  myPostponed.clear();
  while (!myEntitiesStack.empty())
    myEntitiesStack.pop();
  clearNotDumped();
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
    std::pair<int, int>& aNbFeatures = myFeatureCount[aDoc][aKind];
    aNbFeatures.first += 1;

    size_t anIndex = aName.find(aKind);
    if (anIndex == 0 && aName[aKind.length()] == '_') { // name starts with "FeatureKind_"
      std::string anIdStr = aName.substr(aKind.length() + 1);
      int anId = toInt(anIdStr);

      // Check number of already registered objects of such kind. Index of current object
      // should be the same to identify feature's name as automatically generated.
      if (aNbFeatures.first == anId && aNbFeatures.second < anId) {
        // name is not user-defined
        isDefaultName = true;

        // check there are postponed features of this kind,
        // dump their names, because the sequence of features may be changed
        for (std::list<EntityPtr>::const_iterator aPpIt = myPostponed.begin();
            aPpIt != myPostponed.end(); ++aPpIt) {
          FeaturePtr aCurFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*aPpIt);
          if (aCurFeature && aCurFeature->getKind() == aKind) {
            myNames[*aPpIt].myIsDefault = false;
            isDefaultName = false;
          }
        }
      }

      if (anId > aNbFeatures.second)
        aNbFeatures.second = anId;
    }

    // obtain default name for the feature
    if (theUseEntityName)
      aDefaultName << aName;
    else {
      int aFullIndex = 0;
      NbFeaturesMap::const_iterator aFIt = myFeatureCount.begin();
      for (; aFIt != myFeatureCount.end(); ++aFIt) {
        std::map<std::string, std::pair<int, int> >::const_iterator aFound =
          aFIt->second.find(aKind);
        if (aFound != aFIt->second.end())
          aFullIndex += aFound->second.first;
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
  std::list<ResultPtr> allRes;
  ModelAPI_Tools::allResults(theFeature, allRes);
  for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
    std::pair<std::string, bool> aName = ModelAPI_Tools::getDefaultName(*aRes);
    std::string aDefaultName = aName.first;
    std::string aResName = (*aRes)->data()->name();
    bool isUserDefined = !(isFeatureDefaultName && aDefaultName == aResName);
    myNames[*aRes] =
      EntityName(aResName, (isUserDefined ? aResName : std::string()), !isUserDefined);
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
  bool isOk = process(theDoc) && myDumpStorage->exportTo(theFileName, myModules);
  clearCustomStorage();
  return isOk;
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
  // dump folders if any
  dumpPostponed(true);
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
  return isOk;
}

void ModelHighAPI_Dumper::postpone(const EntityPtr& theEntity)
{
  // keep the name
  name(theEntity, false);
  myPostponed.push_back(theEntity);
}

void ModelHighAPI_Dumper::dumpPostponed(bool theDumpFolders)
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
    if (aFolder) {
      if (theDumpFolders)
        dumpFolder(aFolder);
      else
        myPostponed.push_back(*anIt);
    }
    else {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
      if (aFeature)
        dumpFeature(aFeature, true);
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

void ModelHighAPI_Dumper::importModule(const std::string& theModuleName)
{
  myModules.insert(theModuleName);
}

void ModelHighAPI_Dumper::dumpEntitySetName()
{
  const LastDumpedEntity& aLastDumped = myEntitiesStack.top();
  bool isBufferEmpty = myDumpStorage->isBufferEmpty();

  // dump "setName" for the entity
  if (aLastDumped.myUserName) {
    EntityName& anEntityNames = myNames[aLastDumped.myEntity];
    if (!anEntityNames.myIsDefault)
      *myDumpStorage << anEntityNames.myCurrentName << ".setName(\""
                     << anEntityNames.myUserName << "\")\n";
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
      *myDumpStorage << ".setName(\"" << anEntityNames.myUserName << "\")\n";
      // don't dump "setName" for the entity twice
      anEntityNames.myUserName.clear();
      anEntityNames.myIsDefault = true;
    }
    // set result color
    if (!isDefaultColor(*aResIt)) {
      AttributeIntArrayPtr aColor = (*aResIt)->data()->intArray(ModelAPI_Result::COLOR_ID());
      if (aColor && aColor->isInitialized()) {
        *this << *aResIt;
        *myDumpStorage << ".setColor(" << aColor->value(0) << ", " << aColor->value(1)
                       << ", " << aColor->value(2) << ")\n";
      }
    }
    // set result deflection
    if (!isDefaultDeflection(*aResIt)) {
      AttributeDoublePtr aDeflectionAttr =
        (*aResIt)->data()->real(ModelAPI_Result::DEFLECTION_ID());
      if(aDeflectionAttr.get() && aDeflectionAttr->isInitialized()) {
        *this << *aResIt;
        *myDumpStorage << ".setDeflection(" << aDeflectionAttr->value() << ")\n";
      }
    }
    // set result transparency
    if (!isDefaultTransparency(*aResIt)) {
      AttributeDoublePtr aTransparencyAttr =
        (*aResIt)->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
      if(aTransparencyAttr.get() && aTransparencyAttr->isInitialized()) {
        *this << *aResIt;
        *myDumpStorage << ".setTransparency(" << aTransparencyAttr->value() << ")\n";
      }
    }
  }

  myNames[aLastDumped.myEntity].myIsDumped = true;
  myEntitiesStack.pop();

  // clean buffer if it was clear before
  if (isBufferEmpty)
    myDumpStorage->mergeBuffer();
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

static bool isSketchSub(const FeaturePtr& theFeature)
{
  static const std::string SKETCH("Sketch");
  CompositeFeaturePtr anOwner = ModelAPI_Tools::compositeOwner(theFeature);
  return anOwner && anOwner->getKind() == SKETCH;
}

bool ModelHighAPI_Dumper::isDefaultColor(const ResultPtr& theResult) const
{
  AttributeIntArrayPtr aColor = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
  if (!aColor || !aColor->isInitialized())
    return true;

  // check the result belongs to sketch entity, do not dump color in this way
  ResultConstructionPtr aResConstr =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theResult);
  if (aResConstr) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theResult->data()->owner());
    if (isSketchSub(aFeature))
      return true;
  }

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
  *myDumpStorage << theChar;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const char* theString)
{
  *myDumpStorage << theString;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const std::string& theString)
{
  *myDumpStorage << theString;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const bool theValue)
{
  *myDumpStorage << (theValue ? "True" : "False");
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const int theValue)
{
  *myDumpStorage << theValue;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const double theValue)
{
  *myDumpStorage << theValue;
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  importModule("GeomAPI");
  *myDumpStorage << "GeomAPI_Pnt(" << thePoint->x() << ", "
                 << thePoint->y() << ", " << thePoint->z() << ")";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const std::shared_ptr<GeomAPI_Dir>& theDir)
{
  importModule("GeomAPI");
  *myDumpStorage << "GeomAPI_Dir(" << theDir->x() << ", "
                 << theDir->y() << ", " << theDir->z() << ")";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<GeomDataAPI_Dir>& theDir)
{
  *myDumpStorage << theDir->x() << ", " << theDir->y() << ", " << theDir->z();
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<GeomDataAPI_Point>& thePoint)
{
  static const int aSize = 3;
  double aValues[aSize] = {thePoint->x(), thePoint->y(), thePoint->z()};
  std::string aTexts[aSize] = {thePoint->textX(), thePoint->textY(), thePoint->textZ()};
  myDumpStorage->dumpArray(aSize, aValues, aTexts);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<GeomDataAPI_Point2D>& thePoint)
{
  static const int aSize = 2;
  double aValues[aSize] = {thePoint->x(), thePoint->y()};
  std::string aTexts[aSize] = {thePoint->textX(), thePoint->textY()};
  myDumpStorage->dumpArray(aSize, aValues, aTexts);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeBoolean>& theAttrBool)
{
  *myDumpStorage << (theAttrBool->value() ? "True" : "False");
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeInteger>& theAttrInt)
{
  std::string aText = theAttrInt->text();
  if (aText.empty())
    *myDumpStorage << theAttrInt->value();
  else
    *myDumpStorage << "\"" << aText << "\"";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeDouble>& theAttrReal)
{
  std::string aText = theAttrReal->text();
  if (aText.empty())
    *myDumpStorage << theAttrReal->value();
  else
    *myDumpStorage << "\"" << aText << "\"";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeString>& theAttrStr)
{
  *myDumpStorage << "\"" << theAttrStr->value() << "\"";
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const FolderPtr& theFolder)
{
  *myDumpStorage << name(theFolder);

  // add dumped folder to a stack
  if (!myNames[theFolder].myIsDumped &&
     (myEntitiesStack.empty() || myEntitiesStack.top().myEntity != theFolder))
    myEntitiesStack.push(LastDumpedEntity(theFolder, !myNames[theFolder].myIsDefault));

  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const FeaturePtr& theEntity)
{
  *myDumpStorage << name(theEntity);

  if (!myNames[theEntity].myIsDumped) {
    bool isUserDefinedName = !myNames[theEntity].myIsDefault;
    // store results if they have user-defined names or colors
    std::list<ResultPtr> aResultsWithNameOrColor;
    std::list<ResultPtr> allRes;
    ModelAPI_Tools::allResults(theEntity, allRes);
    for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
      if(!myNames[*aRes].myIsDefault || !isDefaultColor(*aRes) ||
         !isDefaultDeflection(*aRes) || !isDefaultTransparency(*aRes))
        aResultsWithNameOrColor.push_back(*aRes);
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
  // iterate in the structure of sub-results to the parent
  ResultPtr aCurRes = theResult;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theResult);
  std::list<int> anIndices; // indexes of results in the parent result, starting from topmost
  while(aCurRes.get()) {
    ResultBodyPtr aParent = ModelAPI_Tools::bodyOwner(aCurRes);
    if (aParent) {
      anIndices.push_front(ModelAPI_Tools::bodyIndex(aCurRes));
    } else { // index of the result in the feature
      std::list<ResultPtr>::const_iterator aRes = aFeature->results().cbegin();
      for(int anIndex = 0; aRes != aFeature->results().cend(); aRes++, anIndex++) {
        if (*aRes == aCurRes) {
          anIndices.push_front(anIndex);
          break;
        }
      }
    }
    aCurRes = aParent;
  }

  *myDumpStorage << name(aFeature);
  for (std::list<int>::iterator anI = anIndices.begin(); anI != anIndices.end(); anI++) {
    if (anI == anIndices.begin()) {
      if(*anI == 0) {
        *myDumpStorage << ".result()";
      }
      else {
        *myDumpStorage << ".results()[" << *anI << "]";
      }
    } else {
      *myDumpStorage << ".subResult(" << *anI << ")";
    }
  }

  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(const ObjectPtr& theObject)
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if(aFeature.get()) {
    *myDumpStorage << name(aFeature);
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

  *myDumpStorage << aWrapperPrefix << name(anOwner) << aWrapperSuffix
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
  *myDumpStorage << "[";
  std::list<std::pair<ObjectPtr, AttributePtr> > aList = theRefAttrList->list();
  bool isAdded = false;
  std::list<std::pair<ObjectPtr, AttributePtr> >::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (isAdded)
      *myDumpStorage << ", ";
    else
      isAdded = true;
    if (anIt->first)
      *this << anIt->first;
    else if (anIt->second)
      * this << anIt->second;
  }
  *myDumpStorage << "]";
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
  static bool aDumpAsIs = false;
  // if number of elements in the list if greater than a threshold,
  // dump it in a separate line with specific name
  if (aDumpAsIs || theRefList->size() <= aThreshold) {
    *myDumpStorage << "[";
    std::list<ObjectPtr> aList = theRefList->list();
    bool isAdded = false;
    std::list<ObjectPtr>::const_iterator anIt = aList.begin();
    for (; anIt != aList.end(); ++anIt) {
      if (isAdded)
        *myDumpStorage << ", ";
      else
        isAdded = true;

      *this << *anIt;
    }
    *myDumpStorage << "]";
  } else {
    // name of list
    FeaturePtr anOwner = ModelAPI_Feature::feature(theRefList->owner());
    std::string aListName = name(anOwner) + "_objects";
    // reserve dumped buffer and store list "as is"
    myDumpStorage->reserveBuffer();
    aDumpAsIs = true;
    *this << aListName << " = " << theRefList << "\n";
    aDumpAsIs = false;
    // append reserved data to the end of the current buffer
    myDumpStorage->restoreReservedBuffer();
    *myDumpStorage << aListName;
  }
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeSelection>& theAttrSelect)
{
  myDumpStorage->write(theAttrSelect);
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
    const std::shared_ptr<ModelAPI_AttributeSelectionList>& theAttrSelList)
{
  static const int aThreshold = 2;
  static bool aDumpAsIs = false;
  // if number of elements in the list if greater than a threshold,
  // dump it in a separate line with specific name
  if (aDumpAsIs || theAttrSelList->size() <= aThreshold) {
    *myDumpStorage << "[";

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
        *myDumpStorage << ", ";
      } else {
        isAdded = true;
      }
      *this << anAttribute;
    }

    // check selection list is obtained by filters
    FiltersFeaturePtr aFilters = theAttrSelList->filters();
    if (aFilters) {
      *myDumpStorage << ", ";
      dumpFeature(aFilters, true);
    }

    *myDumpStorage << "]";
  } else {
    // obtain name of list (the feature may contain several selection lists)
    FeaturePtr anOwner = ModelAPI_Feature::feature(theAttrSelList->owner());
    std::string aListName = name(anOwner) + "_objects";
    std::list<AttributePtr> aSelLists =
        anOwner->data()->attributes(ModelAPI_AttributeSelectionList::typeId());
    if (aSelLists.size() > 1) {
      int anIndex = 1;
      for (std::list<AttributePtr>::iterator aSIt = aSelLists.begin();
           aSIt != aSelLists.end(); ++aSIt, ++anIndex)
        if ((*aSIt).get() == theAttrSelList.get())
          break;
      std::ostringstream aSStream;
      aSStream << aListName << "_" << anIndex;
      aListName = aSStream.str();
    }
    // reserve dumped buffer and store list "as is"
    myDumpStorage->reserveBuffer();
    aDumpAsIs = true;
    *this << aListName << " = " << theAttrSelList << "\n";
    aDumpAsIs = false;
    // append reserved data to the end of the current buffer
    myDumpStorage->restoreReservedBuffer();
    *myDumpStorage << aListName;
  }
  return *this;
}

ModelHighAPI_Dumper& ModelHighAPI_Dumper::operator<<(
  const std::shared_ptr<ModelAPI_AttributeStringArray>& theArray)
{
  std::ostringstream aBuffer;
  aBuffer << "[";
  for(int anIndex = 0; anIndex < theArray->size(); ++anIndex) {
    if (anIndex != 0)
      aBuffer << ", ";

    aBuffer << "\"" << theArray->value(anIndex) << "\"";
  }
  aBuffer << "]";

  myDumpStorage->write(aBuffer.str());
  return *this;
}

/// Dump std::endl
ModelHighAPI_Dumper& operator<<(ModelHighAPI_Dumper& theDumper,
                                std::basic_ostream<char>& (*theEndl)(std::basic_ostream<char>&))
{
  *theDumper.myDumpStorage << theEndl;

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
  theDumper.myDumpStorage->reserveBuffer();
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

  // then store the reserved data
  theDumper.myDumpStorage->restoreReservedBuffer();
  theDumper.myDumpStorage->mergeBuffer();

  // now, store all postponed features
  theDumper.dumpPostponed();

  return theDumper;
}
