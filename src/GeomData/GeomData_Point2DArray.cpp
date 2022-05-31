// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#include "GeomData_Point2DArray.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Expression.h>
#include <ModelAPI_Feature.h>

#include <cassert>

GeomData_Point2DArray::GeomData_Point2DArray(TDF_Label& theLabel)
  : myLab(theLabel)
{
  reinit();
}

void GeomData_Point2DArray::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = myLab.FindAttribute(TDataStd_RealArray::GetID(), myArray) == Standard_True;
}

bool GeomData_Point2DArray::assign(std::shared_ptr<GeomDataAPI_Point2DArray> theOther)
{
  std::shared_ptr<GeomData_Point2DArray> anOther =
      std::dynamic_pointer_cast<GeomData_Point2DArray>(theOther);
  if (!anOther)
    return false;

  setSize(anOther->size());
  myArray->ChangeArray(anOther->myArray->Array(), false);
  owner()->data()->sendAttributeUpdated(this);

  return true;
}

int GeomData_Point2DArray::size()
{
  if (myArray.IsNull() || !myArray->IsValid()) {
    // this could be on undo and then redo creation of the attribute
    // in result creation it may be uninitialized
    myIsInitialized = myLab.FindAttribute(TDataStd_RealArray::GetID(), myArray) == Standard_True;
  }
  // checking the validity because attribute (as a field) may be presented,
  // but without label: it is undoed
  return (myArray.IsNull() || !myArray->IsValid()) ? 0 : myArray->Length() / 2;
}

void GeomData_Point2DArray::setSize(const int theSize)
{
  int aValuesSize = 2 * theSize;
  if (myArray.IsNull() || !myArray->IsValid()) { // create array if it is not done yet
    if (aValuesSize != 0) { // if size is zero, nothing to do (null array means there is no array)
      myArray = TDataStd_RealArray::Set(myLab, 0, aValuesSize - 1);
      owner()->data()->sendAttributeUpdated(this);
    }
  }
  else { // reset the old array
    if (aValuesSize) {
      if (aValuesSize != myArray->Length()) { // old data is kept in the new array
        Handle(TColStd_HArray1OfReal) aNewArray = new TColStd_HArray1OfReal(0, aValuesSize - 1);
        for (int anIndex = 0; anIndex < aValuesSize && anIndex <= myArray->Upper(); ++anIndex)
          aNewArray->SetValue(anIndex, myArray->Value(anIndex));
        myArray->ChangeArray(aNewArray);
        owner()->data()->sendAttributeUpdated(this);
      }
    }
    else { // size is zero => array must be erased
      if (!myArray.IsNull()) {
        myArray.Nullify();
        myLab.ForgetAttribute(TDataStd_RealArray::GetID());
        owner()->data()->sendAttributeUpdated(this);
      }
    }
  }
}

void GeomData_Point2DArray::setPnt(const int theIndex,
                                   const double theX,
                                   const double theY)
{
  if (myArray->Value(2 * theIndex) != theX || myArray->Value(2 * theIndex + 1) != theY) {
    myArray->SetValue(2 * theIndex, theX);
    myArray->SetValue(2 * theIndex + 1, theY);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Point2DArray::setPnt(const int theIndex, const GeomPnt2dPtr& thePoint)
{
  setPnt(theIndex, thePoint->x(), thePoint->y());
}

GeomPnt2dPtr GeomData_Point2DArray::pnt(const int theIndex)
{
  GeomPnt2dPtr aPoint;
  if (theIndex >= 0 && theIndex * 2 < myArray->Length())
    aPoint.reset(new GeomAPI_Pnt2d(myArray->Value(2 * theIndex), myArray->Value(2 * theIndex + 1)));
  return aPoint;
}
