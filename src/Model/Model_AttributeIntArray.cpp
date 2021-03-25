// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <Model_AttributeIntArray.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>

#include <string>

int Model_AttributeIntArray::size()
{
  if (myArray.IsNull() || !myArray->IsValid()) {
    // this could be on undo and then redo creation of the attribute
    // in result creation it may be uninitialized
    myIsInitialized = myLab.FindAttribute(TDataStd_IntegerArray::GetID(), myArray) == Standard_True;
  }
  // checking the validity because attribute (as a field) may be presented,
  // but without label: it is undone
  return (myArray.IsNull() || !myArray->IsValid()) ? 0 : myArray->Length();
}

void Model_AttributeIntArray::setSize(const int theSize, bool sendUpdated)
{
  if (myArray.IsNull() || !myArray->IsValid()) { // create array if it is not done yet
    if (theSize != 0) { // if size is zero, nothing to do (null array means there is no array)
      myArray = TDataStd_IntegerArray::Set(myLab, 0, theSize - 1);
      if (sendUpdated)
        owner()->data()->sendAttributeUpdated(this);
    }
  } else { // reset the old array
    if (theSize) {
      if (theSize != myArray->Length()) { // old data is not kept, a new array is created
        Handle(TColStd_HArray1OfInteger) aNewArray = new TColStd_HArray1OfInteger(0, theSize - 1);
        myArray->ChangeArray(aNewArray);
        if (sendUpdated)
          owner()->data()->sendAttributeUpdated(this);
      }
    } else { // size is zero => array must be erased
      if (!myArray.IsNull()) {
        myArray.Nullify();
        myLab.ForgetAttribute(TDataStd_IntegerArray::GetID());
        if (sendUpdated)
          owner()->data()->sendAttributeUpdated(this);
      }
    }
  }
}

void Model_AttributeIntArray::setValue(const int theIndex,
                                       const int theValue,
                                       bool sendUpdated)
{
  if (!isInitialized() || myArray->Value(theIndex) != theValue) {
    setInitialized();
    myArray->SetValue(theIndex, theValue);
    if (sendUpdated)
      owner()->data()->sendAttributeUpdated(this);
  }
}

int Model_AttributeIntArray::value(const int theIndex)
{
  return myArray->Value(theIndex);
}

Model_AttributeIntArray::Model_AttributeIntArray(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeIntArray::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized =
    myLab.FindAttribute(TDataStd_IntegerArray::GetID(), myArray) == Standard_True;
}
