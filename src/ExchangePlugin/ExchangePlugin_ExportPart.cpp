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

#include <ExchangePlugin_ExportPart.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ConstructionPlugin_Axis.h>
#include <ConstructionPlugin_Plane.h>
#include <ConstructionPlugin_Point.h>

// Obtain all features to be exported to get the list of selected results.
static void collectFeatures(AttributeSelectionListPtr theSelected,
                            std::list<FeaturePtr>& theExport);
// Obtain all constuction elements of the document.
static void collectConstructions(DocumentPtr theDocument, std::list<FeaturePtr>& theExport);


ExchangePlugin_ExportPart::ExchangePlugin_ExportPart()
{
}

void ExchangePlugin_ExportPart::initAttributes()
{
  data()->addAttribute(FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(FILE_FORMAT_ID(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FILE_FORMAT_ID());
  data()->addAttribute(SELECTION_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SELECTION_LIST_ID());
}

void ExchangePlugin_ExportPart::execute()
{
  AttributeStringPtr aFilePathAttr = string(FILE_PATH_ID());
  std::string aFilename = aFilePathAttr->value();
  if (aFilename.empty()) {
    setError("File path is empty.");
    return;
  }

  std::list<FeaturePtr> aFeaturesToExport;

  SessionPtr aSession = ModelAPI_Session::get();
  AttributeSelectionListPtr aSelected = selectionList(SELECTION_LIST_ID());
  DocumentPtr anExportDoc;
  if (aSelected && aSelected->size() == 0 &&
      aSession->activeDocument() == aSession->moduleDocument()) {
    // no result is selected, thus have to export all features of the current document,
    // but the document is a PartSet; and it is forbidden to copy results of Parts,
    // thus copy construction elements only
    collectConstructions(aSession->moduleDocument(), aFeaturesToExport);
  }
  else
    collectFeatures(aSelected, aFeaturesToExport);

  if (!aFeaturesToExport.empty()) {
    // remove 'ExportPart' feature is any
    if (aFeaturesToExport.back()->getKind() == ExchangePlugin_ExportPart::ID())
      aFeaturesToExport.pop_back();
    // save the document
    if (!aSession->activeDocument()->save(aFilename.c_str(), aFeaturesToExport))
      setError("Cannot save the document.");
  }
}


// ================================     Auxiliary functions     ===================================

static void allReferencedFeatures(std::set<FeaturePtr>& theFeatures)
{
  std::set<FeaturePtr> aReferences;
  for (std::set<FeaturePtr>::iterator anIt = theFeatures.begin();
       anIt != theFeatures.end(); ++anIt) {
    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    (*anIt)->data()->referencesToObjects(aRefs);

    for (std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRIt = aRefs.begin();
         aRIt != aRefs.end(); ++aRIt) {
      for (std::list<ObjectPtr>::iterator anObjIt = aRIt->second.begin();
           anObjIt != aRIt->second.end(); ++anObjIt) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(*anObjIt);
        if (aFeature)
          aReferences.insert(aFeature);
      }
    }
  }

  if (!aReferences.empty()) {
    allReferencedFeatures(aReferences);
    theFeatures.insert(aReferences.begin(), aReferences.end());
  }
}

void collectFeatures(AttributeSelectionListPtr theSelected, std::list<FeaturePtr>& theExport)
{
  theExport = ModelAPI_Session::get()->activeDocument()->allFeatures();

  if (!theSelected || theSelected->size() == 0) {
    // nothing is selected, return all features of the document
    return;
  }

  // collect initial list of features basing on the selected results
  std::set<FeaturePtr> aFeaturesToExport;
  for (int anIndex = 0, aSize = theSelected->size(); anIndex < aSize; ++anIndex) {
    AttributeSelectionPtr aCurrent = theSelected->value(anIndex);
    FeaturePtr aCurrentFeature = ModelAPI_Feature::feature(aCurrent->context());
    if (aCurrentFeature)
      aFeaturesToExport.insert(aCurrentFeature);
  }
  // recursively collect all features used for the selected results
  allReferencedFeatures(aFeaturesToExport);

  // remove the features which are not affect the selected results
  std::list<FeaturePtr>::iterator anIt = theExport.begin();
  while (anIt != theExport.end()) {
    if (aFeaturesToExport.find(*anIt) == aFeaturesToExport.end()) {
      std::list<FeaturePtr>::iterator aRemoveIt = anIt++;
      theExport.erase(aRemoveIt);
    }
    else
      ++anIt;
  }
}

void collectConstructions(DocumentPtr theDocument, std::list<FeaturePtr>& theExport)
{
  theExport = theDocument->allFeatures();
  // keep constructions only
  std::list<FeaturePtr>::iterator anIt = theExport.begin();
  while (anIt != theExport.end()) {
    FeaturePtr aCurFeature = *anIt;
    ResultPtr aCurResult = aCurFeature->lastResult();

    bool isApplicable =
        (!aCurResult || aCurResult->groupName() == ModelAPI_ResultConstruction::group());

    if (isApplicable && !aCurFeature->isInHistory()) {
      isApplicable = aCurFeature->getKind() != ConstructionPlugin_Point::ID() &&
                     aCurFeature->getKind() != ConstructionPlugin_Axis::ID() &&
                     aCurFeature->getKind() != ConstructionPlugin_Plane::ID();
    }

    if (isApplicable)
      ++anIt;
    else {
      std::list<FeaturePtr>::iterator aRemoveIt = anIt++;
      theExport.erase(aRemoveIt);
    }
  }
}
