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

#include <Selector_Selector.h>

#include <Selector_NameGenerator.h>
#include <Selector_Algo.h>

#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TNaming_NamedShape.hxx>

Selector_Selector::Selector_Selector(TDF_Label theLab, TDF_Label theBaseDocLab) :
  myLab(theLab), myBaseDocumentLab(theBaseDocLab), myAlgo(NULL)
{}

Selector_Selector::~Selector_Selector()
{
  if (myAlgo)
    delete myAlgo;
}

bool Selector_Selector::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
  const bool theGeometricalNaming)
{
  if (theValue.IsNull() || theContext.IsNull())
    return false;

  myAlgo = Selector_Algo::select(theContext, theValue, myLab, myBaseDocumentLab,
    theGeometricalNaming, true, true);

  return myAlgo != NULL;
}

bool Selector_Selector::store(const TopoDS_Shape theContext)
{
  myAlgo->store();
  return myAlgo->solve(theContext); // to update the selection shape on the label
}

bool Selector_Selector::restore(const TopoDS_Shape theContext)
{
  myAlgo = Selector_Algo::restoreByLab(myLab, myBaseDocumentLab);
  if (myAlgo) {
    return myAlgo->solve(theContext); // to update the selection shape on the label
  }
  return false;
}

TopoDS_Shape Selector_Selector::value()
{
  Handle(TNaming_NamedShape) aNS;
  if (myLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
    return aNS->Get();
  return TopoDS_Shape(); // empty, error shape
}

std::wstring Selector_Selector::name(Selector_NameGenerator* theNameGenerator) {
  return myAlgo->name(theNameGenerator);
}

TDF_Label Selector_Selector::restoreByName(
  std::wstring theName, const TopAbs_ShapeEnum theShapeType,
  Selector_NameGenerator* theNameGenerator, const bool theGeometricalNaming)
{
  TDF_Label aResult;
  myAlgo = Selector_Algo::restoreByName(myLab, myBaseDocumentLab, theName, theShapeType,
    theGeometricalNaming, theNameGenerator, aResult);
  if (myAlgo) {
    return aResult;
  }
  return TDF_Label();
}

void Selector_Selector::combineGeometrical(const TopoDS_Shape theContext)
{
  TopoDS_Shape aValue = value();
  if (aValue.IsNull() || aValue.ShapeType() == TopAbs_COMPOUND)
    return;

  Selector_Algo* aNewAlgo = Selector_Algo::relesectWithAllGeometry(myAlgo, theContext);
  if (aNewAlgo) {
    aNewAlgo->store();
    aNewAlgo->solve(theContext);
    delete myAlgo;
    myAlgo = aNewAlgo;
  }
}

bool Selector_Selector::solve(const TopoDS_Shape theContext)
{
  return myAlgo->solve(theContext);
}
