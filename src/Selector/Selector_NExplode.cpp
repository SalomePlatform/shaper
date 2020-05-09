// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "Selector_NExplode.h"

#include <GeomAPI_Pnt.h>

#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <NCollection_DataMap.hxx>
#include <Precision.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>

#include <algorithm>

static void dummy(gp_Pnt& thePoint)
{
  // do nothing (new approach to order shapes)
}

static void pointToDouble(gp_Pnt& thePoint)
{
  // old approach to order shapes
  double dMidXYZ = thePoint.X() * 999.0 + thePoint.Y() * 99.0 + thePoint.Z() * 0.9;
  thePoint.SetCoord(dMidXYZ, 0.0, 0.0);
}

static std::pair<gp_Pnt, double> ShapeToDouble (const TopoDS_Shape& S,
                                                void (*convertPoint)(gp_Pnt&))
{
  // Computing of CentreOfMass
  gp_Pnt GPoint;
  double Len;

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
  return std::make_pair(GPoint, Len);
}

/*!
* \brief Sort shapes in the list by their coordinates.
*/
struct CompareShapes : public std::binary_function<TopoDS_Shape, TopoDS_Shape, bool>
{
  typedef NCollection_DataMap<TopoDS_Shape, std::pair<gp_Pnt, double> > DataMapOfShapeDouble;

  CompareShapes(DataMapOfShapeDouble* theCashMap, void (*convertPoint)(gp_Pnt&))
    : myMap(theCashMap), myConvertPoint(convertPoint) {}

  bool operator() (const TopoDS_Shape& lhs, const TopoDS_Shape& rhs);

  DataMapOfShapeDouble* myMap;
  void (*myConvertPoint)(gp_Pnt&);
};

bool CompareShapes::operator() (const TopoDS_Shape& theShape1,
  const TopoDS_Shape& theShape2)
{
  if (!myMap->IsBound(theShape1)) {
    myMap->Bind(theShape1, ShapeToDouble(theShape1, myConvertPoint));
  }

  if (!myMap->IsBound(theShape2)) {
    myMap->Bind(theShape2, ShapeToDouble(theShape2, myConvertPoint));
  }

  const std::pair<gp_Pnt, double>& val1 = myMap->Find(theShape1);
  const std::pair<gp_Pnt, double>& val2 = myMap->Find(theShape2);

  double tol = 10.0 * Precision::Confusion();
  bool exchange = Standard_False;

  // compare coordinates of center points
  GeomPointPtr aPnt1(new GeomAPI_Pnt(val1.first.X(), val1.first.Y(), val1.first.Z()));
  GeomPointPtr aPnt2(new GeomAPI_Pnt(val2.first.X(), val2.first.Y(), val2.first.Z()));
  if (aPnt2->isLess(aPnt1, tol)) {
    exchange = Standard_True;
  }
  else if (!aPnt1->isLess(aPnt2, tol)) {
    double dLength = val1.second - val2.second;
    if (dLength >= tol) {
      exchange = Standard_True;
    }
    else if (Abs(dLength) < tol && theShape1.ShapeType() <= TopAbs_FACE) {
      // equal values possible on shapes such as two halves of a sphere and
      // a membrane inside the sphere
// LCOV_EXCL_START
      // this part of code is taken from GEOM module, but can not reproduce in SHAPER
      Bnd_Box box1,box2;
      BRepBndLib::Add(theShape1, box1);
      if (!box1.IsVoid()) {
        BRepBndLib::Add(theShape2, box2);
        Standard_Real dSquareExtent = box1.SquareExtent() - box2.SquareExtent();
        if (dSquareExtent >= tol) {
          exchange = Standard_True;
        }
        else if (Abs(dSquareExtent) < tol) {
          Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, val1, val2;
          box1.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          val1 = (aXmin+aXmax)*999.0 + (aYmin+aYmax)*99.0 + (aZmin+aZmax)*0.9;
          box2.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          val2 = (aXmin+aXmax)*999.0 + (aYmin+aYmax)*99.0 + (aZmin+aZmax)*0.9;
          if ((val1 - val2) >= tol) {
            exchange = Standard_True;
          }
        }
      }
// LCOV_EXCL_STOP
    } else // compare addresses if shapes are geometrically equal
      return theShape1.TShape().get() > theShape2.TShape().get();
  }

  //return val1 < val2;
  return !exchange;
}

Selector_NExplode::Selector_NExplode(const TopoDS_ListOfShape& theShapes, const bool theOldOrder)
  : myToBeReordered(theOldOrder)
{
  for(TopoDS_ListOfShape::Iterator anIter(theShapes); anIter.More(); anIter.Next()) {
    mySorted.push_back(anIter.Value());
  }

  CompareShapes::DataMapOfShapeDouble aCash;
  CompareShapes shComp(&aCash, theOldOrder ? pointToDouble : dummy);
  std::stable_sort(mySorted.begin(), mySorted.end(), shComp);
}

Selector_NExplode::Selector_NExplode(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType,
                                     const bool theOldOrder)
  : myToBeReordered(theOldOrder)
{
  TopTools_MapOfShape anAdded; // to avoid same shapes duplication
  for(TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next()) {
    if (anAdded.Add(anExp.Current()))
      mySorted.push_back(anExp.Current());
  }

  CompareShapes::DataMapOfShapeDouble aCash;
  CompareShapes shComp(&aCash, theOldOrder ? pointToDouble : dummy);
  std::stable_sort(mySorted.begin(), mySorted.end(), shComp);
}


int Selector_NExplode::index(const TopoDS_Shape& theSubShape)
{
  // reorder if necessary
  reorder();

  std::vector<TopoDS_Shape>::iterator anIter = mySorted.begin();
  for(int anIndex = 1; anIter != mySorted.end(); anIter++, anIndex++) {
    if ((*anIter).IsSame(theSubShape))
      return anIndex;
  }
  return -1; // not found
}

TopoDS_Shape Selector_NExplode::shape(int& theIndex)
{
  std::vector<TopoDS_Shape>::iterator anIter = mySorted.begin();
  for(int anIndex = 1; anIter != mySorted.end(); anIter++, anIndex++) {
    if (anIndex == theIndex) {
      TopoDS_Shape aShape = *anIter;
      if (myToBeReordered)
        theIndex = index(aShape);
      return aShape;
    }
  }
  return TopoDS_Shape(); // not found
}

void Selector_NExplode::reorder()
{
  if (!myToBeReordered)
    return;

  myToBeReordered = false;
  CompareShapes::DataMapOfShapeDouble aCash;
  CompareShapes shComp(&aCash, dummy);
  std::stable_sort(mySorted.begin(), mySorted.end(), shComp);
}
