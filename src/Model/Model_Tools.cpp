// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <Model_Tools.h>
#include <Model_Data.h>

#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Session.h>

#include <ConstructionPlugin_Axis.h>
#include <ConstructionPlugin_Plane.h>
#include <ConstructionPlugin_Point.h>

#include <Standard_GUID.hxx>

#include <TDataStd_Comment.hxx>
#include <TDataStd_AsciiString.hxx>

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

void Model_Tools::copyLabels(TDF_Label theSource, TDF_Label theDestination,
                             Handle(TDF_RelocationTable) theRelocTable)
{
  theRelocTable->SetRelocation(theSource, theDestination);
  // copy the sub-labels hierarchy
  TDF_ChildIterator aSubLabsIter(theSource);
  for (; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyLabels(aSubLabsIter.Value(),
               theDestination.FindChild(aSubLabsIter.Value().Tag()),
               theRelocTable);
  }
}

void Model_Tools::copyAttrs(TDF_Label theSource, TDF_Label theDestination,
                            Handle(TDF_RelocationTable) theRelocTable)
{
  TDF_AttributeIterator anAttrIter(theSource);
  for(; anAttrIter.More(); anAttrIter.Next()) {
    Handle(TDF_Attribute) aTargetAttr;
    if (!theDestination.FindAttribute(anAttrIter.Value()->ID(), aTargetAttr)) {
      // create a new attribute if not yet exists in the destination
	    aTargetAttr = anAttrIter.Value()->NewEmpty();
      if (aTargetAttr->ID() != anAttrIter.Value()->ID())
        aTargetAttr->SetID(anAttrIter.Value()->ID());
      theDestination.AddAttribute(aTargetAttr);
    }
    // no special relocation, empty map, but self-relocation is on: copy references w/o changes
    Handle(TDF_RelocationTable) aRelocTable =
        theRelocTable.IsNull() ? new TDF_RelocationTable(Standard_True) : theRelocTable;
    anAttrIter.Value()->Paste(aTargetAttr, aRelocTable);
    // an exception: if a source reference refers itself, a copy must also refer itself
    if (aTargetAttr->ID() == TDF_Reference::GetID()) {
      Handle(TDF_Reference) aTargetRef = Handle(TDF_Reference)::DownCast(aTargetAttr);
      if (aTargetRef->Get().IsEqual(anAttrIter.Value()->Label()))
        aTargetRef->Set(aTargetRef->Label());
    }
  }
  // copy the sub-labels content
  TDF_ChildIterator aSubLabsIter(theSource);
  for(; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyAttrs(aSubLabsIter.Value(),
              theDestination.FindChild(aSubLabsIter.Value().Tag()),
              theRelocTable);
  }
}

static TCollection_AsciiString labelToString(TDF_Label theLabel)
{
  TCollection_AsciiString aLabString;
  TDF_Tool::Entry(theLabel, aLabString);
  return aLabString;
}

static void makeExternalReference(TDF_Label theDestination, TDF_Label theReferred)
{
  Handle(TDF_Attribute) aReference, aComment, aString;
  theDestination.FindAttribute(TDF_Reference::GetID(), aReference);
  // create new attributes if not yet exists in the destination
  if (!theDestination.FindAttribute(TDataStd_Comment::GetID(), aComment)) {
    aComment = new TDataStd_Comment;
    theDestination.AddAttribute(aComment);
  }
  if (!theDestination.FindAttribute(TDataStd_AsciiString::GetID(), aString)) {
    aString = new TDataStd_AsciiString;
    theDestination.AddAttribute(aString);
  }
  // reference to itself
  Handle(TDF_Reference)::DownCast(aReference)->Set(theDestination, theDestination);
  // ID of the document
  std::ostringstream aDocIdStr;
  aDocIdStr << ModelAPI_Session::get()->moduleDocument()->id();
  Handle(TDataStd_Comment)::DownCast(aComment)->Set(aDocIdStr.str().c_str());
  // value of referred label
  Handle(TDataStd_AsciiString)::DownCast(aString)->Set(labelToString(theReferred));
}

void Model_Tools::copyAttrsAndKeepRefsToCoordinates(
    TDF_Label theSource,
    TDF_Label theDestination,
    const std::set<TCollection_AsciiString>& theCoordinateLabels,
    Handle(TDF_RelocationTable) theRelocTable)
{
  TDF_AttributeIterator anAttrIter(theSource);
  for(; anAttrIter.More(); anAttrIter.Next()) {
    Handle(TDF_Attribute) aTargetAttr;
    if (!theDestination.FindAttribute(anAttrIter.Value()->ID(), aTargetAttr)) {
      // create a new attribute if not yet exists in the destination
	    aTargetAttr = anAttrIter.Value()->NewEmpty();
      if (aTargetAttr->ID() != anAttrIter.Value()->ID())
        aTargetAttr->SetID(anAttrIter.Value()->ID());
      theDestination.AddAttribute(aTargetAttr);
    }
    anAttrIter.Value()->Paste(aTargetAttr, theRelocTable);
    if (aTargetAttr->ID() == TDF_Reference::GetID()) {
      Handle(TDF_Reference) aTargetRef = Handle(TDF_Reference)::DownCast(aTargetAttr);
      if (aTargetRef->Get().IsNull()) {
        // may be refer to a cartesian coordinate entity
        Handle(TDF_Reference) aSourceRef = Handle(TDF_Reference)::DownCast(anAttrIter.Value());
        if (!aSourceRef.IsNull() && !aSourceRef->Get().IsNull()) {
          std::set<TCollection_AsciiString>::const_iterator aFound =
              theCoordinateLabels.find(labelToString(aSourceRef->Get()));
          if (aFound != theCoordinateLabels.end())
            makeExternalReference(theDestination, aSourceRef->Get());
        }
      }
      else if (aTargetRef->Get().IsEqual(anAttrIter.Value()->Label())) {
        // a source reference refers itself, a copy must also refer itself
        aTargetRef->Set(aTargetRef->Label());
      }
    }
  }
  // copy the sub-labels content
  TDF_ChildIterator aSubLabsIter(theSource);
  for(; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyAttrsAndKeepRefsToCoordinates(
        aSubLabsIter.Value(), theDestination.FindChild(aSubLabsIter.Value().Tag()),
        theCoordinateLabels, theRelocTable);
  }
}

void Model_Tools::labelsOfCoordinates(std::set<TCollection_AsciiString>& theCoordinateLabels,
                                      Handle(TDF_RelocationTable) theRelocTable)
{
  DocumentPtr aPartSet = ModelAPI_Session::get()->moduleDocument();
  std::list<FeaturePtr> aFeatures = aPartSet->allFeatures();
  for (std::list<FeaturePtr>::iterator aFIt = aFeatures.begin(); aFIt != aFeatures.end(); ++aFIt) {
    FeaturePtr aCurFeat = *aFIt;
    if (!aCurFeat->isInHistory() &&
        (aCurFeat->getKind() == ConstructionPlugin_Point::ID() ||
         aCurFeat->getKind() == ConstructionPlugin_Axis::ID() ||
         aCurFeat->getKind() == ConstructionPlugin_Plane::ID())) {
      ResultPtr aResult = aCurFeat->lastResult();
      if (aResult) {
        std::shared_ptr<Model_Data> aResData =
            std::dynamic_pointer_cast<Model_Data>(aResult->data());
        TDF_Label aLab = aResData->label().Father();
        theCoordinateLabels.insert(labelToString(aLab));
        // set relocation to empty, references will be set correctly while copying attributes
        theRelocTable->SetRelocation(aLab, TDF_Label());
      }
    }
  }
}
