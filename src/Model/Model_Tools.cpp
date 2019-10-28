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

#include <Model_Tools.h>

#include <Standard_GUID.hxx>

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>

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
