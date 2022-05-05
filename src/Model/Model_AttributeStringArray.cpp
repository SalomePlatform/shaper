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

#include "Model_AttributeStringArray.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <TColStd_HArray1OfExtendedString.hxx>

//==================================================================================================
int Model_AttributeStringArray::size()
{
  if (myArray.IsNull() || !myArray->IsValid()) {
    // this could be on undo and then redo creation of the attribute
    // in result creation it may be uninitialized
    myIsInitialized =
      myLab.FindAttribute(TDataStd_ExtStringArray::GetID(), myArray) == Standard_True;
  }
  // checking the validity because attribute (as a field) may be presented,
  // but without label: it is undoed
  return (myArray.IsNull() || !myArray->IsValid()) ? 0 : myArray->Length();
}

//==================================================================================================
void Model_AttributeStringArray::setSize(const int theSize)
{
  if (myArray.IsNull() || !myArray->IsValid()) { // create array if it is not done yet
    if (theSize != 0) { // if size is zero, nothing to do (null array means there is no array)
      myArray = TDataStd_ExtStringArray::Set(myLab, 0, theSize - 1);
      owner()->data()->sendAttributeUpdated(this);
    }
  } else { // reset the old array
    if (theSize) {
      if (theSize != myArray->Length()) { // old data is not keept, a new array is created
        Handle(TColStd_HArray1OfExtendedString) aNewArray =
          new TColStd_HArray1OfExtendedString(0, theSize - 1);
        myArray->ChangeArray(aNewArray);
        owner()->data()->sendAttributeUpdated(this);
      }
    } else { // size is zero => array must be erased
      if (!myArray.IsNull()) {
        myArray.Nullify();
        myLab.ForgetAttribute(TDataStd_ExtStringArray::GetID());
        owner()->data()->sendAttributeUpdated(this);
      }
    }
  }
}

//==================================================================================================
void Model_AttributeStringArray::setValue(const int theIndex,
                                          const std::string theValue)
{
  if (!myArray->Value(theIndex).IsEqual(theValue.c_str())) {
    myArray->SetValue(theIndex, theValue.c_str());
    owner()->data()->sendAttributeUpdated(this);
  }
}

//==================================================================================================
std::string Model_AttributeStringArray::value(const int theIndex)
{
  return TCollection_AsciiString(myArray->Value(theIndex)).ToCString();
}

//==================================================================================================
Model_AttributeStringArray::Model_AttributeStringArray(TDF_Label& theLabel)
{
  myLab = theLabel;
  myIsInitialized =
    myLab.FindAttribute(TDataStd_ExtStringArray::GetID(), myArray) == Standard_True;
}
