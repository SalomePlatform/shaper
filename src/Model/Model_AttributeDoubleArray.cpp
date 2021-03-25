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

#include "Model_AttributeDoubleArray.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <TColStd_HArray1OfReal.hxx>

#include <string>

//==================================================================================================
int Model_AttributeDoubleArray::size()
{
  if (myArray.IsNull() || !myArray->IsValid()) {
    // this could be on undo and then redo creation of the attribute
    // in result creation it may be uninitialized
    myIsInitialized = myLab.FindAttribute(TDataStd_RealArray::GetID(), myArray) == Standard_True;
  }
  // checking the validity because attribute (as a field) may be presented,
  // but without label: it is undoed
  return (myArray.IsNull() || !myArray->IsValid()) ? 0 : myArray->Length();
}

//==================================================================================================
void Model_AttributeDoubleArray::setSize(const int theSize)
{
  if (myArray.IsNull() || !myArray->IsValid()) { // create array if it is not done yet
    if (theSize != 0) { // if size is zero, nothing to do (null array means there is no array)
      myArray = TDataStd_RealArray::Set(myLab, 0, theSize - 1);
      owner()->data()->sendAttributeUpdated(this);
    }
  } else { // reset the old array
    if (theSize) {
      if (theSize != myArray->Length()) { // old data is not keept, a new array is created
        Handle(TColStd_HArray1OfReal) aNewArray = new TColStd_HArray1OfReal(0, theSize - 1);
        myArray->ChangeArray(aNewArray);
        owner()->data()->sendAttributeUpdated(this);
      }
    } else { // size is zero => array must be erased
      if (!myArray.IsNull()) {
        myArray.Nullify();
        myLab.ForgetAttribute(TDataStd_RealArray::GetID());
        owner()->data()->sendAttributeUpdated(this);
      }
    }
  }
}

//==================================================================================================
void Model_AttributeDoubleArray::setValue(const int theIndex,
                                          const double theValue)
{
  if (myArray->Value(theIndex) != theValue) {
    myArray->SetValue(theIndex, theValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

//==================================================================================================
double Model_AttributeDoubleArray::value(const int theIndex)
{
  return myArray->Value(theIndex);
}

//==================================================================================================
Model_AttributeDoubleArray::Model_AttributeDoubleArray(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeDoubleArray::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized =
    myLab.FindAttribute(TDataStd_RealArray::GetID(), myArray) == Standard_True;
}
