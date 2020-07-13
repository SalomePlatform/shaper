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

#include <ExchangePlugin_ExportPart.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ConstructionPlugin_Axis.h>
#include <ConstructionPlugin_Plane.h>
#include <ConstructionPlugin_Point.h>

#include <Events_InfoMessage.h>

#include <PartSetPlugin_Part.h>

#include <sstream>

// Obtain all features to be exported to get the list of selected results.
static void collectFeatures(DocumentPtr theDocument,
                            AttributeSelectionListPtr theSelected,
                            std::list<FeaturePtr>& theExport);
// Obtain all constuction elements of the document.
static void collectConstructions(DocumentPtr theDocument, std::list<FeaturePtr>& theExport);
// Check features could be exported. The following features cannot be exported:
// * non-construction result (Part) when exporting the PartSet;
// * features, which refer to objects from another document.
// Returns true if all features can be exported.
static bool verifyExport(const std::list<FeaturePtr>& theFeatures,
                         std::list<FeaturePtr>& theExternalReferences,
                         std::list<FeaturePtr>& theExportedParts,
                         std::list<FeaturePtr>& theReferredParts);
// Collect names of features as a single string
static std::wstring namesOfFeatures(const std::list<FeaturePtr>& theFeatures);


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
    setError("File name is empty.");
    return;
  }

  std::list<FeaturePtr> aFeaturesToExport;

  DocumentPtr anExportDoc = document();
  DocumentPtr aPartSetDoc = ModelAPI_Session::get()->moduleDocument();
  AttributeSelectionListPtr aSelected = selectionList(SELECTION_LIST_ID());
  if (aSelected && aSelected->size() == 0 && anExportDoc == aPartSetDoc) {
    // no result is selected, thus have to export all features of the current document,
    // but the document is a PartSet; and it is forbidden to copy results of Parts,
    // thus copy construction elements only
    collectConstructions(anExportDoc, aFeaturesToExport);
  }
  else
    collectFeatures(anExportDoc, aSelected, aFeaturesToExport);

  if (aFeaturesToExport.empty()) {
    Events_InfoMessage(getKind(), "Selected features cannot be exported from the document.").send();
    return;
  }

  // remove 'ExportPart' feature if any
  if (aFeaturesToExport.back()->getKind() == ExchangePlugin_ExportPart::ID())
    aFeaturesToExport.pop_back();

  std::list<FeaturePtr> anExternalLinks, anExportedParts, aReferredParts;
  if (!verifyExport(aFeaturesToExport, anExternalLinks, anExportedParts, aReferredParts)) {
    if (!anExternalLinks.empty()) {
      std::wstring aListOfFeatures = namesOfFeatures(anExternalLinks);

      std::string aMessage = "The selected results were created using external references "
                             "outside of this Part from features %1. "
                             "Please, remove these references or select another "
                             "sub-set of results to be able to export.";
      Events_InfoMessage(getKind(), aMessage).arg(aListOfFeatures).send();
    }
    if (!aReferredParts.empty()) {
      std::wstring aListOfParts = namesOfFeatures(aReferredParts);

      std::string aMessage = "The selected results were created using references "
                             "to the results of Parts: %1. Please, remove these references "
                             "or select another sub-set of results to be able to export.";
      Events_InfoMessage(getKind(), aMessage).arg(aListOfParts).send();
    }
    if (!anExportedParts.empty()) {
      std::wstring aListOfParts = namesOfFeatures(anExportedParts);

      std::string aMessage = "The export of Part's result is forbidden (%1).";
      Events_InfoMessage(getKind(), aMessage).arg(aListOfParts).send();
    }
    // should not export anything
    aFeaturesToExport.clear();
  }

  if (!aFeaturesToExport.empty()) {
    // save the document
    if (!anExportDoc->save(aFilename.c_str(), aFeaturesToExport))
      setError("Cannot save the document.");
  }
}


// ================================     Auxiliary functions     ===================================

static bool isCoordinate(FeaturePtr theFeature)
{
  return !theFeature->isInHistory() &&
          (theFeature->getKind() == ConstructionPlugin_Point::ID() ||
           theFeature->getKind() == ConstructionPlugin_Axis::ID() ||
           theFeature->getKind() == ConstructionPlugin_Plane::ID());
}

static void allReferencedFeatures(const std::set<FeaturePtr>& theFeatures,
                                  std::set<FeaturePtr>& theReferencedFeatures)
{
  std::set<FeaturePtr> aReferences;
  for (std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin();
       anIt != theFeatures.end(); ++anIt) {
    theReferencedFeatures.insert(*anIt);

    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    (*anIt)->data()->referencesToObjects(aRefs);

    for (std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRIt = aRefs.begin();
         aRIt != aRefs.end(); ++aRIt) {
      for (std::list<ObjectPtr>::iterator anObjIt = aRIt->second.begin();
           anObjIt != aRIt->second.end(); ++anObjIt) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(*anObjIt);
        if (aFeature && !isCoordinate(aFeature) &&
            theReferencedFeatures.find(aFeature) == theReferencedFeatures.end())
          aReferences.insert(aFeature);
      }
    }
  }

  if (!aReferences.empty())
    allReferencedFeatures(aReferences, theReferencedFeatures);
}

void collectFeatures(DocumentPtr theDocument,
                     AttributeSelectionListPtr theSelected,
                     std::list<FeaturePtr>& theExport)
{
  theExport = theDocument->allFeatures();

  // remove all features after the current one
  FeaturePtr aCurrentFeature = theDocument->currentFeature(false);
  std::list<FeaturePtr>::iterator anIt = theExport.begin();
  for (; anIt != theExport.end(); ++anIt)
    if (*anIt == aCurrentFeature) {
      theExport.erase(++anIt, theExport.end());
      break;
    }

  if (!theSelected || theSelected->size() == 0) {
    // nothing is selected, return all features of the document
    return;
  }

  // collect initial list of features basing on the selected results
  std::set<FeaturePtr> aFeaturesToExport;
  for (int anIndex = 0, aSize = theSelected->size(); anIndex < aSize; ++anIndex) {
    AttributeSelectionPtr aCurrent = theSelected->value(anIndex);
    FeaturePtr aCurFeature = ModelAPI_Feature::feature(aCurrent->context());
    if (aCurFeature)
      aFeaturesToExport.insert(aCurFeature);
  }
  // recursively collect all features used for the selected results
  allReferencedFeatures(aFeaturesToExport, aFeaturesToExport);

  // remove the features which are not affect the selected results
  anIt = theExport.begin();
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
        (!aCurResult || aCurResult->groupName() == ModelAPI_ResultConstruction::group()) &&
        !isCoordinate(aCurFeature);

    if (isApplicable)
      ++anIt;
    else {
      std::list<FeaturePtr>::iterator aRemoveIt = anIt++;
      theExport.erase(aRemoveIt);
    }
  }
}

bool verifyExport(const std::list<FeaturePtr>& theFeatures,
                  std::list<FeaturePtr>& theExternalReferences,
                  std::list<FeaturePtr>& theExportedParts,
                  std::list<FeaturePtr>& theReferredParts)
{
  for (std::list<FeaturePtr>::const_iterator anIt = theFeatures.begin();
       anIt != theFeatures.end(); ++anIt) {
    // full part should not be exported
    if ((*anIt)->getKind() == PartSetPlugin_Part::ID())
      theExportedParts.push_back(*anIt);

    DocumentPtr aDoc = (*anIt)->document();

    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    (*anIt)->data()->referencesToObjects(aRefs);
    std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRIt = aRefs.begin();
    for (;  aRIt != aRefs.end(); ++aRIt) {
      for (std::list<ObjectPtr>::iterator anObjIt = aRIt->second.begin();
           anObjIt != aRIt->second.end(); ++anObjIt) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(*anObjIt);
        if (aFeature) {
          // feature refers to external entity,
          // which is neither the Origin nor coordinate axis or plane
          if (aFeature->document() != aDoc && !isCoordinate(aFeature))
            theExternalReferences.push_back(*anIt);
          // feature refers to result of a part
          if (aFeature->getKind() == PartSetPlugin_Part::ID())
            theReferredParts.push_back(*anIt);
        }
      }
    }
  }

  return theExternalReferences.empty() && theExportedParts.empty() && theReferredParts.empty();
}

std::wstring namesOfFeatures(const std::list<FeaturePtr>& theFeatures)
{
  std::wostringstream aListOfFeatures;
  for (std::list<FeaturePtr>::const_iterator anIt = theFeatures.begin();
       anIt != theFeatures.end(); ++anIt) {
    if (anIt != theFeatures.begin())
      aListOfFeatures << ", ";
    aListOfFeatures << "'" << (*anIt)->name() << "'";
  }
  return aListOfFeatures.str();
}
