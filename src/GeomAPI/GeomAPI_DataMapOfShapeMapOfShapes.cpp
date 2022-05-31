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

#include <GeomAPI_DataMapOfShapeMapOfShapes.h>

#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>

typedef NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape> > MAP;
#define MY_MAP implPtr<MAP>()

//=================================================================================================
GeomAPI_DataMapOfShapeMapOfShapes::GeomAPI_DataMapOfShapeMapOfShapes()
: GeomAPI_Interface(new NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape> >)
{}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::bind(const std::shared_ptr<GeomAPI_Shape> theKey,
                                             const ListOfShape& theItems)
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  if(MY_MAP->IsBound(aKey)) {
    MY_MAP->ChangeFind(aKey).Clear();
  }
  for(ListOfShape::const_iterator anIt = theItems.cbegin(); anIt != theItems.cend(); anIt++) {
    const TopoDS_Shape& anItem = (*anIt)->impl<TopoDS_Shape>();
    if(MY_MAP->IsBound(aKey)) {
      MY_MAP->ChangeFind(aKey).Add(anItem);
    } else {
      NCollection_Map<TopoDS_Shape> anItems;
      anItems.Add(anItem);
      MY_MAP->Bind(aKey, anItems);
    }
  }

  return true;
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::add(const std::shared_ptr<GeomAPI_Shape> theKey,
                                            const std::shared_ptr<GeomAPI_Shape> theItem)
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  const TopoDS_Shape& anItem = theItem->impl<TopoDS_Shape>();
  if(MY_MAP->IsBound(aKey)) {
    return MY_MAP->ChangeFind(aKey).Add(anItem) == Standard_True;
  } else {
    NCollection_Map<TopoDS_Shape> anItems;
    anItems.Add(anItem);
    return MY_MAP->Bind(aKey, anItems) == Standard_True;
  }
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::isBound(const std::shared_ptr<GeomAPI_Shape> theKey) const
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  return MY_MAP->IsBound(aKey) == Standard_True;
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::find(const std::shared_ptr<GeomAPI_Shape> theKey,
                                             ListOfShape& theItems) const
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();

  if(MY_MAP->IsBound(aKey) == Standard_False) {
    return false;
  }

  const NCollection_Map<TopoDS_Shape>& aMap = MY_MAP->Find(aKey);
  for(NCollection_Map<TopoDS_Shape>::Iterator anIt(aMap); anIt.More(); anIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
    aShape->setImpl(new TopoDS_Shape(anIt.Value()));
    theItems.push_back(aShape);
  }

  return true;
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::unBind(const std::shared_ptr<GeomAPI_Shape> theKey)
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  return MY_MAP->UnBind(aKey) == Standard_True;
}

//=================================================================================================
void GeomAPI_DataMapOfShapeMapOfShapes::clear()
{
  return MY_MAP->Clear();
}

//=================================================================================================
int GeomAPI_DataMapOfShapeMapOfShapes::size() const
{
  return MY_MAP->Size();
}

// LCOV_EXCL_START

//=================================================================================================
//   iterator implementation
//=================================================================================================

class IteratorImpl : public GeomAPI_DataMapOfShapeMapOfShapes::iterator
{
public:
  IteratorImpl() {}

  IteratorImpl(const MAP::Iterator& theIterator)
    : myIterator(theIterator)
  {}

  IteratorImpl(const std::shared_ptr<IteratorImpl>& theIterator)
  {
    mySelf = theIterator;
  }

  bool operator==(const std::shared_ptr<IteratorImpl>& theOther) const
  {
    if (theOther)
      return theOther->myIterator.IsEqual(myIterator);

    // theOther is end iterator => check the current iterator is not at the end
    return !myIterator.More();
  }

  virtual iterator& operator++()
  {
    myIterator.Next();
    return *this;
  }
  virtual iterator operator++(int)
  {
    std::shared_ptr<IteratorImpl> aSelf = std::dynamic_pointer_cast<IteratorImpl>(mySelf);
    std::shared_ptr<IteratorImpl> aCopy(new IteratorImpl(aSelf->myIterator));
    myIterator.Next();
    return IteratorImpl(aCopy);
  }

  virtual key_type first() const
  {
    if (mySelf)
      return iterator::first();

    GeomShapePtr aShape(new GeomAPI_Shape);
    aShape->setImpl(new TopoDS_Shape(myIterator.Key()));
    return aShape;
  }

  virtual mapped_type second() const
  {
    if (mySelf)
      return iterator::second();

    ListOfShape anItems;
    const NCollection_Map<TopoDS_Shape>& aMap = myIterator.Value();
    for(NCollection_Map<TopoDS_Shape>::Iterator anIt(aMap); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      anItems.push_back(aShape);
    }
    return anItems;
  }

private:
  MAP::Iterator myIterator;
};


GeomAPI_DataMapOfShapeMapOfShapes::iterator::iterator()
{
}

GeomAPI_DataMapOfShapeMapOfShapes::iterator::iterator(
    const GeomAPI_DataMapOfShapeMapOfShapes::iterator& theOther)
  : mySelf(theOther.mySelf)
{
}

const GeomAPI_DataMapOfShapeMapOfShapes::iterator&
    GeomAPI_DataMapOfShapeMapOfShapes::iterator::operator=(
      const GeomAPI_DataMapOfShapeMapOfShapes::iterator& theOther)
{
  mySelf = theOther.mySelf;
  return *this;
}

bool GeomAPI_DataMapOfShapeMapOfShapes::iterator::operator==(const iterator& theOther) const
{
  std::shared_ptr<IteratorImpl> aSelf = std::dynamic_pointer_cast<IteratorImpl>(mySelf);
  std::shared_ptr<IteratorImpl> aSelfOther =
      std::dynamic_pointer_cast<IteratorImpl>(theOther.mySelf);

  return aSelf ? aSelf->operator==(aSelfOther)
                : (aSelfOther ? aSelfOther->operator==(aSelf) : true);
}

bool GeomAPI_DataMapOfShapeMapOfShapes::iterator::operator!=(const iterator& theOther) const
{
  return !operator==(theOther);
}

GeomAPI_DataMapOfShapeMapOfShapes::iterator&
    GeomAPI_DataMapOfShapeMapOfShapes::iterator::operator++()
{
  mySelf->operator++();
  return *this;
}

GeomAPI_DataMapOfShapeMapOfShapes::iterator
    GeomAPI_DataMapOfShapeMapOfShapes::iterator::operator++(int)
{
  return ++(*mySelf);
}

GeomAPI_DataMapOfShapeMapOfShapes::iterator::key_type
    GeomAPI_DataMapOfShapeMapOfShapes::iterator::first() const
{
  return mySelf->first();
}

GeomAPI_DataMapOfShapeMapOfShapes::iterator::mapped_type
    GeomAPI_DataMapOfShapeMapOfShapes::iterator::second() const
{
  return mySelf->second();
}



GeomAPI_DataMapOfShapeMapOfShapes::iterator GeomAPI_DataMapOfShapeMapOfShapes::begin()
{
  MAP::Iterator anIt(*MY_MAP);
  std::shared_ptr<IteratorImpl> anIter(new IteratorImpl(anIt));
  return IteratorImpl(anIter);
}

GeomAPI_DataMapOfShapeMapOfShapes::const_iterator GeomAPI_DataMapOfShapeMapOfShapes::begin() const
{
  MAP::Iterator anIt(*MY_MAP);
  std::shared_ptr<IteratorImpl> anIter(new IteratorImpl(anIt));
  return IteratorImpl(anIter);
}

GeomAPI_DataMapOfShapeMapOfShapes::iterator GeomAPI_DataMapOfShapeMapOfShapes::end()
{
  return IteratorImpl(std::shared_ptr<IteratorImpl>());
}

GeomAPI_DataMapOfShapeMapOfShapes::const_iterator GeomAPI_DataMapOfShapeMapOfShapes::end() const
{
  return IteratorImpl(std::shared_ptr<IteratorImpl>());
}
// LCOV_EXCL_STOP
