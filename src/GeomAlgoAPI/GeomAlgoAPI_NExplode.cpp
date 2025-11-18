// Copyright (C) 2017-2025  CEA, EDF
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

#include "GeomAlgoAPI_NExplode.h"

#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <algorithm>
#include <set>
#include <unordered_map>
#include <functional>

namespace NExplodeTools
{
  void dummy(gp_Pnt&)
  {
    // do nothing (new approach to order shapes)
  }

  void pointToDouble(gp_Pnt& thePoint)
  {
    // old approach to order shapes
    double dMidXYZ = thePoint.X() * 999.0 + thePoint.Y() * 99.0 + thePoint.Z() * 0.9;
    thePoint.SetCoord(dMidXYZ, 0.0, 0.0);
  }

  std::pair<GeomPointPtr, double> ShapeToDouble(const GeomShapePtr& theShape,
                                                void(*convertPoint)(gp_Pnt&))
  {
    // Computing of CentreOfMass
    gp_Pnt GPoint;
    double Len;

    TopoDS_Shape S = theShape->impl<TopoDS_Shape>();
    if (S.ShapeType() == TopAbs_VERTEX) {
      GPoint = BRep_Tool::Pnt(TopoDS::Vertex(S));
      Len = (double)S.Orientation();
    }
    else {
      GProp_GProps GPr;
      if (S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE) {
        BRepGProp::LinearProperties(S, GPr);
      }
      else if (S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL) {
        BRepGProp::SurfaceProperties(S, GPr);
      }
      else {
        BRepGProp::VolumeProperties(S, GPr);
      }
      GPoint = GPr.CentreOfMass();
      Len = GPr.Mass();
    }

    (*convertPoint)(GPoint);
    GeomPointPtr aMidPoint(new GeomAPI_Pnt(GPoint.X(), GPoint.Y(), GPoint.Z()));
    return std::make_pair(aMidPoint, Len);
  }

  /*!
  * \brief Sort shapes in the list by their coordinates.
  */
  struct CompareShapes : public std::function<bool(GeomShapePtr&, GeomShapePtr&)>
  {
    typedef std::unordered_map<GeomShapePtr,
                               std::pair<GeomPointPtr, double>,
                               GeomAPI_Shape::Hash,
                               GeomAPI_Shape::Equal > DataMapOfShapeDouble;

    CompareShapes(void(*convertPoint)(gp_Pnt&))
      : myConvertPoint(convertPoint) {}

    bool operator() (const GeomShapePtr& lhs, const GeomShapePtr& rhs);

    DataMapOfShapeDouble myMap;
    void(*myConvertPoint)(gp_Pnt&);
  };
}

bool NExplodeTools::CompareShapes::operator() (const GeomShapePtr& lhs, const GeomShapePtr& rhs)
{
  if (myMap.find(lhs) == myMap.end()) {
    myMap[lhs] = ShapeToDouble(lhs, myConvertPoint);
  }

  if (myMap.find(rhs) == myMap.end()) {
    myMap[rhs] = ShapeToDouble(rhs, myConvertPoint);
  }

  const std::pair<GeomPointPtr, double>& val1 = myMap.at(lhs);
  const std::pair<GeomPointPtr, double>& val2 = myMap.at(rhs);

  double tol = 10.0 * Precision::Confusion();
  bool exchange = Standard_False;

  // compare coordinates of center points
  if (val2.first->isLess(val1.first, tol)) {
    exchange = Standard_True;
  }
  else if (!val1.first->isLess(val2.first, tol)) {
    double dLength = val1.second - val2.second;
    if (dLength >= tol) {
      exchange = Standard_True;
    }
    else if (Abs(dLength) < tol && lhs->shapeType() <= GeomAPI_Shape::FACE) {
      // equal values possible on shapes such as two halves of a sphere and
      // a membrane inside the sphere
      Bnd_Box box1, box2;
      BRepBndLib::Add(lhs->impl<TopoDS_Shape>(), box1);
      if (!box1.IsVoid()) {
        BRepBndLib::Add(rhs->impl<TopoDS_Shape>(), box2);
        Standard_Real dSquareExtent = box1.SquareExtent() - box2.SquareExtent();
        if (dSquareExtent >= tol) {
          exchange = Standard_True;
        }
        else if (Abs(dSquareExtent) < tol) {
          Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, value1, value2;
          box1.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          value1 = (aXmin + aXmax)*999.0 + (aYmin + aYmax)*99.0 + (aZmin + aZmax)*0.9;
          box2.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          value2 = (aXmin + aXmax)*999.0 + (aYmin + aYmax)*99.0 + (aZmin + aZmax)*0.9;
          if (value1 - value2 >= tol) {
            exchange = Standard_True;
          }
          else { // compare addresses if shapes are geometrically equal
            exchange = lhs->impl<TopoDS_Shape>().TShape().get() >
                       rhs->impl<TopoDS_Shape>().TShape().get();
          }
        }
      }
    }
    else { // compare addresses if shapes are geometrically equal
      exchange = lhs->impl<TopoDS_Shape>().TShape().get() >
                 rhs->impl<TopoDS_Shape>().TShape().get();
     }
  }

  //return val1 < val2;
  return !exchange;
}

GeomAlgoAPI_NExplode::GeomAlgoAPI_NExplode(const GeomShapePtr theContext,
                                           const GeomAPI_Shape::ShapeType theShapeType,
                                           const ShapeOrder theOrder)
{
  std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aMapShape;
  GeomAPI_ShapeExplorer anExp(theContext, theShapeType);
  for (; anExp.more(); anExp.next()) {
    GeomShapePtr aCurrent = anExp.current();
    if (aMapShape.find(aCurrent) == aMapShape.end()) {
      mySorted.push_back(aCurrent);
      aMapShape.insert(aCurrent);
    }
  }
  reorder(theOrder);
}

GeomAlgoAPI_NExplode::GeomAlgoAPI_NExplode(const ListOfShape& theShapes,
                                           const ShapeOrder theOrder)
  : mySorted(theShapes.begin(), theShapes.end())
{
  reorder(theOrder);
}

int GeomAlgoAPI_NExplode::index(const GeomShapePtr theSubShape)
{
  std::vector<GeomShapePtr>::iterator anIter = mySorted.begin();
  for(int anIndex = 1; anIter != mySorted.end(); anIter++, anIndex++) {
    if ((*anIter)->isSame(theSubShape))
      return anIndex;
  }
  return 0; // not found
}

GeomShapePtr GeomAlgoAPI_NExplode::shape(const int theIndex)
{
  std::vector<GeomShapePtr>::iterator anIter = mySorted.begin();
  for(int anIndex = 1; anIter != mySorted.end(); anIter++, anIndex++) {
    if (anIndex == theIndex)
      return *anIter;
  }
  return GeomShapePtr(); // not found
}

void GeomAlgoAPI_NExplode::reorder(const ShapeOrder theNewOrder)
{
  NExplodeTools::CompareShapes shComp(
      theNewOrder == ORDER_BY_HASH_VALUE ? NExplodeTools::pointToDouble : NExplodeTools::dummy);
  std::stable_sort(mySorted.begin(), mySorted.end(), shComp);
}
