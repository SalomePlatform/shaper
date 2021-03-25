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

#include "GeomAPI_ShapeIterator.h"

#include <Standard_NoMoreObject.hxx>
#include <TopoDS_Iterator.hxx>

#define MY_ITERATOR implPtr<TopoDS_Iterator>()

//==================================================================================================
GeomAPI_ShapeIterator::GeomAPI_ShapeIterator()
: GeomAPI_Interface(new TopoDS_Iterator())
{
}

//==================================================================================================
GeomAPI_ShapeIterator::GeomAPI_ShapeIterator(const std::shared_ptr<GeomAPI_Shape> theShape)
: GeomAPI_Interface(new TopoDS_Iterator())
{
  init(theShape);
}

//==================================================================================================
void GeomAPI_ShapeIterator::init(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  if(!theShape.get()) {
    return;
  }
  MY_ITERATOR->Initialize(theShape->impl<TopoDS_Shape>());
}

//==================================================================================================
bool GeomAPI_ShapeIterator::more() const
{
  return MY_ITERATOR->More() == Standard_True;
}

//==================================================================================================
void GeomAPI_ShapeIterator::next()
{
  try {
    MY_ITERATOR->Next();
  } catch (Standard_NoMoreObject) {
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAPI_ShapeIterator::current()
{
  try {
    const TopoDS_Shape& aShape = MY_ITERATOR->Value();
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(aShape));
    return aGeomShape;
  } catch (Standard_NoMoreObject) {
    return std::shared_ptr<GeomAPI_Shape>();
  }
}
