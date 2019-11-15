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

#include <ExchangePlugin_ImportPart.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

#include <PartSetPlugin_Part.h>

#include <map>
#include <sstream>

// Update names of imported features/results concurent with existing objects.
static void correntNonUniqueNames(DocumentPtr theDocument, std::list<FeaturePtr>& theImported);

ExchangePlugin_ImportPart::ExchangePlugin_ImportPart()
{
}

void ExchangePlugin_ImportPart::initAttributes()
{
  data()->addAttribute(FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
}

void ExchangePlugin_ImportPart::execute()
{
  AttributeStringPtr aFilePathAttr = string(FILE_PATH_ID());
  std::string aFilename = aFilePathAttr->value();
  if (aFilename.empty()) {
    setError("File name is empty.");
    return;
  }

  // load the file into the active document
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDoc = document();
  bool isPartSet = aDoc == aSession->moduleDocument();
  std::list<FeaturePtr> anImportedFeatures;
  bool isOk = aDoc->import(aFilename.c_str(), anImportedFeatures, isPartSet);
  if (!isOk && isPartSet) {
    // there are features not appropriate for PartSet,
    // create new part and load there
    FeaturePtr aPartFeature = aDoc->addFeature(PartSetPlugin_Part::ID());
    ResultPartPtr aPartResult;
    if (aPartFeature) {
      aPartFeature->execute();
      aPartResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->lastResult());
    }
    if (aPartResult) {
      aDoc = aPartResult->partDoc();
      isOk = aDoc->import(aFilename.c_str(), anImportedFeatures);
    }
  }
  if (isOk)
    correntNonUniqueNames(aDoc, anImportedFeatures);
  else
    setError("Cannot import the document.");
}


// ================================     Auxiliary functions     ===================================

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

void addIndexedName(const std::string& theName,
                    std::map<std::string, std::set<int> >& theIndexedNames)
{
  std::string aName = theName;
  int anIndex = 0;
  bool isIndexed = splitName(aName, anIndex);
  std::set<int>& anIndices = theIndexedNames[aName];
  if (isIndexed)
    anIndices.insert(anIndex);
}

// Collect names of features and results in the document before the import.
// The name of indexed feature/result will be split to the name and the index. For example ,
// 'Point_1', 'Point_2' will be placed at the same key with the set of corrsponding indices:
// 'Point_1', 'Point_2' => {'Point', [1, 2]}.
// Thus, the new point should have index 3 and therefore the name 'Point_3'.
static void collectOldNames(DocumentPtr theDocument, std::list<FeaturePtr>& theAvoided,
                            std::map<std::string, std::set<int> >& theIndexedNames)
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
    addIndexedName((*aFIt)->data()->name(), theIndexedNames);
    // store names of results
    const std::list<ResultPtr>& aResults = (*aFIt)->results();
    for (std::list<ResultPtr>::const_iterator aRIt = aResults.begin();
         aRIt != aResults.end(); ++aRIt)
      addIndexedName((*aRIt)->data()->name(), theIndexedNames);
  }
}

static std::string uniqueName(const std::string& theName,
                              std::map<std::string, std::set<int> >& theExistingNames)
{
  std::string aName = theName;
  int anIndex = 1;
  splitName(aName, anIndex);

  std::map<std::string, std::set<int> >::iterator aFound = theExistingNames.find(aName);
  bool isUnique = false;
  if (aFound == theExistingNames.end())
    isUnique = true;
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

  if (isUnique) {
    // name is unique
    aName = theName;
    addIndexedName(theName, theExistingNames);
  }
  return aName;
}

void correntNonUniqueNames(DocumentPtr theDocument, std::list<FeaturePtr>& theImported)
{
  std::map<std::string, std::set<int> > aNames;
  collectOldNames(theDocument, theImported, aNames);

  for (std::list<FeaturePtr>::iterator anIt = theImported.begin();
       anIt != theImported.end(); ++anIt) {
    // update name of feature
    std::string aNewName = uniqueName((*anIt)->data()->name(), aNames);
    (*anIt)->data()->setName(aNewName);
    // update names of results
    const std::list<ResultPtr>& aResults = (*anIt)->results();
    for (std::list<ResultPtr>::const_iterator aRIt = aResults.begin();
         aRIt != aResults.end(); ++aRIt) {
      aNewName = uniqueName((*aRIt)->data()->name(), aNames);
      (*aRIt)->data()->setName(aNewName);
    }
  }
}
