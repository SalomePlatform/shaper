// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <ExchangePlugin_ImportPart.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <PartSetPlugin_Part.h>

#include <map>
#include <sstream>

static const std::string THE_NEW_PART_STR("New Part");
static const std::string THE_PART_SET_STR("PartSet");

// Update names of imported features/results concurent with existing objects.
static void correntNonUniqueNames(DocumentPtr theDocument, std::list<FeaturePtr>& theImported);
// Find the document according to its name or create the new one.
static DocumentPtr findDocument(DocumentPtr thePartSetDoc, const std::string& thePartName);

ExchangePlugin_ImportPart::ExchangePlugin_ImportPart()
{
}

void ExchangePlugin_ImportPart::initAttributes()
{
  data()->addAttribute(FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TARGET_PART_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(TARGET_PARTS_LIST_ID(), ModelAPI_AttributeStringArray::typeId());
}


void ExchangePlugin_ImportPart::execute()
{
  AttributeStringPtr aFilePathAttr = string(FILE_PATH_ID());
  std::string aFilename = aFilePathAttr->value();
  if (aFilename.empty()) {
    setError("File name is empty.");
    return;
  }

  // get the document where to import
  AttributeStringArrayPtr aPartsAttr = stringArray(TARGET_PARTS_LIST_ID());
  AttributeIntegerPtr aTargetAttr = integer(TARGET_PART_ID());
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDoc =
      findDocument(aSession->moduleDocument(), aPartsAttr->value(aTargetAttr->value()));

  // load the file into the document
  std::list<FeaturePtr> anImportedFeatures;
  if (aDoc && aDoc->importPart(aFilename.c_str(), anImportedFeatures))
    correntNonUniqueNames(aDoc, anImportedFeatures);
  else
    setError("Cannot import the document.");
}

void ExchangePlugin_ImportPart::attributeChanged(const std::string& theID)
{
  if (theID == FILE_PATH_ID()) {
    AttributeStringPtr aFilePathAttr = string(FILE_PATH_ID());
    if (aFilePathAttr->value().empty())
      return;

    AttributeStringArrayPtr aPartsAttr = stringArray(TARGET_PARTS_LIST_ID());
    AttributeIntegerPtr aTargetAttr = integer(TARGET_PART_ID());

    // update the list of target parts
    SessionPtr aSession = ModelAPI_Session::get();
    DocumentPtr aDoc = document();
    bool isPartSet = aDoc == aSession->moduleDocument();
    if (isPartSet) {
      std::list<std::string> anAcceptedValues;
      anAcceptedValues.push_back(THE_NEW_PART_STR);

      std::list<FeaturePtr> anImportedFeatures;
      if (aDoc->importPart(aFilePathAttr->value().c_str(), anImportedFeatures, isPartSet))
        anAcceptedValues.push_back(THE_PART_SET_STR);

      // append names of all parts
      std::list<FeaturePtr> aSubFeatures = aDoc->allFeatures();
      for (std::list<FeaturePtr>::iterator aFIt = aSubFeatures.begin();
           aFIt != aSubFeatures.end(); ++aFIt) {
        if ((*aFIt)->getKind() == PartSetPlugin_Part::ID())
          anAcceptedValues.push_back((*aFIt)->name());
      }

      if ((size_t)aPartsAttr->size() != anAcceptedValues.size())
        aTargetAttr->setValue(0);

      aPartsAttr->setSize((int)anAcceptedValues.size());
      std::list<std::string>::iterator anIt = anAcceptedValues.begin();
      for (int anInd = 0; anIt != anAcceptedValues.end(); ++anIt, ++anInd)
        aPartsAttr->setValue(anInd, *anIt);
    }
    else {
      // keep only the name of the current part
      if (aPartsAttr->size() == 0) {
        FeaturePtr aPartFeature = ModelAPI_Tools::findPartFeature(aSession->moduleDocument(), aDoc);

        aPartsAttr->setSize(1);
        aPartsAttr->setValue(0, aPartFeature->name());
        aTargetAttr->setValue(0);
      }
    }
  }
}


// ================================     Auxiliary functions     ===================================

DocumentPtr findDocument(DocumentPtr thePartSetDoc, const std::string& thePartName)
{
  DocumentPtr aDoc;
  if (thePartName == THE_PART_SET_STR)
    aDoc = thePartSetDoc;
  else {
    FeaturePtr aPartFeature;
    if (thePartName == THE_NEW_PART_STR) {
      // create new part
      aPartFeature = thePartSetDoc->addFeature(PartSetPlugin_Part::ID());
      if (aPartFeature)
        aPartFeature->execute();
    }
    else {
      // find existing part by its name
      std::list<FeaturePtr> aSubFeatures = thePartSetDoc->allFeatures();
      for (std::list<FeaturePtr>::iterator aFIt = aSubFeatures.begin();
           aFIt != aSubFeatures.end(); ++aFIt) {
        if ((*aFIt)->getKind() == PartSetPlugin_Part::ID() && (*aFIt)->name() == thePartName) {
          aPartFeature = *aFIt;
          break;
        }
      }
    }

    if (aPartFeature) {
      ResultPartPtr aPartResult =
          std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->lastResult());
      if (aPartResult)
        aDoc = aPartResult->partDoc();
    }
  }
  return aDoc;
}

typedef std::map<std::string, std::map<std::string, std::set<int> > > ObjectNameMap;

bool splitName(std::string& theName, int& theIndex)
{
  size_t aLastUndercore = theName.find_last_of('_');
  bool isOk = aLastUndercore != std::string::npos;
  if (isOk) {
    char* isNumber;
    std::string anIndexStr = theName.substr(aLastUndercore + 1);
    theIndex = std::strtol(anIndexStr.c_str(), &isNumber, 10);
    isOk = isNumber != 0;
    if (isOk)
      theName.erase(aLastUndercore);
  }
  return isOk;
}

void addIndexedName(const ObjectPtr& theObject, ObjectNameMap& theIndexedNames)
{
  std::string aName = theObject->data()->name();
  std::string aGroup = theObject->groupName();
  int anIndex = 0;
  bool isIndexed = splitName(aName, anIndex);
  std::set<int>& anIndices = theIndexedNames[aGroup][aName];
  if (isIndexed)
    anIndices.insert(anIndex);
}

// Collect names of features and results in the document before the import.
// The name of indexed feature/result will be split to the name and the index. For example ,
// 'Point_1', 'Point_2' will be placed at the same key with the set of corrsponding indices:
// 'Point_1', 'Point_2' => {'Point', [1, 2]}.
// Thus, the new point should have index 3 and therefore the name 'Point_3'.
static void collectOldNames(DocumentPtr theDocument, std::list<FeaturePtr>& theAvoided,
                            ObjectNameMap& theIndexedNames)
{
  std::list<FeaturePtr> anAllFeatures = theDocument->allFeatures();
  std::list<FeaturePtr>::iterator aFIt = anAllFeatures.begin();
  std::list<FeaturePtr>::iterator anAvoidIt = theAvoided.begin();
  for (; aFIt != anAllFeatures.end(); ++aFIt) {
    if (anAvoidIt != theAvoided.end() && *aFIt == *anAvoidIt) {
      // skip this feature
      ++anAvoidIt;
      continue;
    }

    // store name of feature
    addIndexedName(*aFIt, theIndexedNames);
    // store names of results
    const std::list<ResultPtr>& aResults = (*aFIt)->results();
    for (std::list<ResultPtr>::const_iterator aRIt = aResults.begin();
         aRIt != aResults.end(); ++aRIt)
      addIndexedName(*aRIt, theIndexedNames);
  }
}

static std::string uniqueName(const ObjectPtr& theObject, ObjectNameMap& theExistingNames)
{
  std::string aName = theObject->data()->name();
  std::string aGroup = theObject->groupName();
  int anIndex = 1;
  splitName(aName, anIndex);

  ObjectNameMap::iterator aFoundGroup = theExistingNames.find(aGroup);
  bool isUnique = aFoundGroup == theExistingNames.end();

  std::map<std::string, std::set<int> >::iterator aFound;
  if (!isUnique) {
    aFound = aFoundGroup->second.find(aName);
    isUnique = aFound == aFoundGroup->second.end();
  }

  if (isUnique) {
    // name is unique
    aName = theObject->data()->name();
    addIndexedName(theObject, theExistingNames);
  }
  else {
    // search the appropriate index
    std::set<int>::iterator aFoundIndex = aFound->second.find(anIndex);
    for (; aFoundIndex != aFound->second.end(); ++aFoundIndex, ++anIndex)
      if (anIndex != *aFoundIndex)
        break;
    // compose the new name
    std::ostringstream aNewName;
    aNewName << aName << "_" << anIndex;
    aName = aNewName.str();
    // add new index
    aFound->second.insert(anIndex);
  }

  return aName;
}

void correntNonUniqueNames(DocumentPtr theDocument, std::list<FeaturePtr>& theImported)
{
  ObjectNameMap aNames;
  collectOldNames(theDocument, theImported, aNames);

  for (std::list<FeaturePtr>::iterator anIt = theImported.begin();
       anIt != theImported.end(); ++anIt) {
    // update name of feature
    std::string aNewName = uniqueName(*anIt, aNames);
    (*anIt)->data()->setName(aNewName);
    // update names of results
    const std::list<ResultPtr>& aResults = (*anIt)->results();
    for (std::list<ResultPtr>::const_iterator aRIt = aResults.begin();
         aRIt != aResults.end(); ++aRIt) {
      aNewName = uniqueName(*aRIt, aNames);
      (*aRIt)->data()->setName(aNewName);
    }
  }
}
