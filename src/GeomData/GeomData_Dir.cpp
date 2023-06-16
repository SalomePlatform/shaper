// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "GeomData_Dir.h"
#include <GeomAPI_Dir.h>
#include <GeomAPI_XYZ.h>
#include <gp_Dir.hxx>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

void GeomData_Dir::setValue(const double theX, const double theY, const double theZ)
{
  if (!myIsInitialized || myCoords->Value(0) != theX || myCoords->Value(1) != theY
      || myCoords->Value(2) != theZ) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    myCoords->SetValue(2, theZ);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Dir::setValue(const std::shared_ptr<GeomAPI_Dir>& theDir)
{
  setValue(theDir->x(), theDir->y(), theDir->z());
}

double GeomData_Dir::x() const
{
  return myCoords->Value(0);
}

double GeomData_Dir::y() const
{
  return myCoords->Value(1);
}

double GeomData_Dir::z() const
{
  return myCoords->Value(2);
}

std::shared_ptr<GeomAPI_Dir> GeomData_Dir::dir()
{
  return std::shared_ptr<GeomAPI_Dir>(
      new GeomAPI_Dir(myCoords->Value(0), myCoords->Value(1), myCoords->Value(2)));
}

std::shared_ptr<GeomAPI_XYZ> GeomData_Dir::xyz()
{
  return std::shared_ptr<GeomAPI_XYZ>(
      new GeomAPI_XYZ(myCoords->Value(0), myCoords->Value(1), myCoords->Value(2)));
}

GeomData_Dir::GeomData_Dir(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void GeomData_Dir::reinit()
{
  myIsInitialized = myLab.FindAttribute(TDataStd_RealArray::GetID(), myCoords) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(myLab, 0, 2);
    myIsInitialized = true;
  }
}

void GeomData_Dir::reset()
{
  if (myLab.FindAttribute(TDataStd_RealArray::GetID(), myCoords)) {
    // invalidate values of the direction
    myCoords->SetValue(0, 0.0);
    myCoords->SetValue(1, 0.0);
    myCoords->SetValue(2, 0.0);
  }
  GeomDataAPI_Dir::reset();
}

bool GeomData_Dir::isInitialized()
{
  // Check once again the direction is initialized.
  // Use case (bos #18905): draw a sketch, click "Change sketch plane", then abort it.
  //                        myIsInitialized value is dropped to false, thus recheck.
  // Additionally verify the values are valid.
  // Use case (bos #24206): start a sketch, highlight a coordinate plane, then click anywhere
  //                        in empty space. Sketch features in menu appears and can be selected.
  if (!myIsInitialized) {
    myIsInitialized = myLab.FindAttribute(TDataStd_RealArray::GetID(), myCoords) == Standard_True
        && myCoords->Value(0) != 0.0 && myCoords->Value(1) != 0.0 && myCoords->Value(2) != 0.0;
  }
  return ModelAPI_Attribute::isInitialized();
}
