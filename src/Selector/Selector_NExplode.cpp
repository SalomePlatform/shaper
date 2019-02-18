// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include <vector>
#include <algorithm>

static std::pair<double, double> ShapeToDouble (const TopoDS_Shape& S)
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

  double dMidXYZ = GPoint.X() * 999.0 + GPoint.Y() * 99.0 + GPoint.Z() * 0.9;
  return std::make_pair(dMidXYZ, Len);
}

/*!
* \brief Sort shapes in the list by their coordinates.
*/
struct CompareShapes : public std::binary_function<TopoDS_Shape, TopoDS_Shape, bool>
{
  typedef NCollection_DataMap<TopoDS_Shape, std::pair<double, double> > DataMapOfShapeDouble;

  CompareShapes(DataMapOfShapeDouble* theCashMap) : myMap(theCashMap) {}

  bool operator() (const TopoDS_Shape& lhs, const TopoDS_Shape& rhs);

  DataMapOfShapeDouble* myMap;
};

bool CompareShapes::operator() (const TopoDS_Shape& theShape1,
  const TopoDS_Shape& theShape2)
{
  if (!myMap->IsBound(theShape1)) {
    myMap->Bind(theShape1, ShapeToDouble(theShape1));
  }

  if (!myMap->IsBound(theShape2)) {
    myMap->Bind(theShape2, ShapeToDouble(theShape2));
  }

  std::pair<double, double> val1 = myMap->Find(theShape1);
  std::pair<double, double> val2 = myMap->Find(theShape2);

  double tol = Precision::Confusion();
  bool exchange = Standard_False;

  double dMidXYZ = val1.first - val2.first;
  if (dMidXYZ >= tol) {
    exchange = Standard_True;
  }
  else if (Abs(dMidXYZ) < tol) {
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

Selector_NExplode::Selector_NExplode(const TopoDS_ListOfShape& theShapes)
{
  std::vector<TopoDS_Shape> aShapesVec;

  for(TopoDS_ListOfShape::Iterator anIter(theShapes); anIter.More(); anIter.Next()) {
      aShapesVec.push_back(anIter.Value());
  }

  CompareShapes::DataMapOfShapeDouble aCash;
  CompareShapes shComp(&aCash);
  std::stable_sort(aShapesVec.begin(), aShapesVec.end(), shComp);

  std::vector<TopoDS_Shape>::const_iterator anIter = aShapesVec.begin();
  for (; anIter != aShapesVec.end(); ++anIter) {
    mySorted.Append(*anIter);
  }
}

Selector_NExplode::Selector_NExplode(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  std::vector<TopoDS_Shape> aShapesVec;
  TopTools_MapOfShape anAdded; // to avoid same shapes duplication
  for(TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next()) {
    if (anAdded.Add(anExp.Current()))
     aShapesVec.push_back(anExp.Current());
  }

  CompareShapes::DataMapOfShapeDouble aCash;
  CompareShapes shComp(&aCash);
  std::stable_sort(aShapesVec.begin(), aShapesVec.end(), shComp);

  std::vector<TopoDS_Shape>::const_iterator anIter = aShapesVec.begin();
  for (; anIter != aShapesVec.end(); ++anIter) {
    mySorted.Append(*anIter);
  }
}


int Selector_NExplode::index(const TopoDS_Shape& theSubShape)
{
  TopoDS_ListOfShape::Iterator anIter(mySorted);
  for(int anIndex = 1; anIter.More(); anIter.Next(), anIndex++) {
    if (anIter.Value().IsSame(theSubShape))
      return anIndex;
  }
  return -1; // not found
}

TopoDS_Shape Selector_NExplode::shape(const int theIndex)
{
  TopoDS_ListOfShape::Iterator anIter(mySorted);
  for(int anIndex = 1; anIter.More(); anIter.Next(), anIndex++) {
    if (anIndex == theIndex)
      return anIter.Value();
  }
  return TopoDS_Shape(); // not found
}
