// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Selector_Container.h>

#include <Selector_NameGenerator.h>
#include <Selector_NExplode.h>

#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_ChildIterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>

Selector_Container::Selector_Container() : Selector_AlgoWithSubs()
{}

bool Selector_Container::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  myShapeType = theValue.ShapeType();

  // iterate all sub-shapes and select them on sublabels
  for(TopoDS_Iterator aSubIter(theValue); aSubIter.More(); aSubIter.Next()) {
    Selector_Algo* aSubAlgo = Selector_Algo::select(theContext, aSubIter.Value(),
      newSubLabel(), baseDocument(),
      false, useNeighbors(), useIntersections()); //for subs no geometrical naming allowed
    if (!append(aSubAlgo))
      return false;
  }
  return true;
}

void Selector_Container::store()
{
  storeType(Selector_Algo::SELTYPE_CONTAINER);
  // store all sub-selectors
  TDataStd_Integer::Set(label(), shapeTypeID(), (int)myShapeType);
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    (*aSubSel)->store();
  }
  TDataStd_Integer::Set(label(), shapeTypeID(), (int)myShapeType);
}

bool Selector_Container::restore()
{
  Handle(TDataStd_Integer) aShapeTypeAttr;
  if (!label().FindAttribute(shapeTypeID(), aShapeTypeAttr))
    return false;
  myShapeType = TopAbs_ShapeEnum(aShapeTypeAttr->Get());
  // restore sub-selectors
  bool aSubResult = true;
  for(TDF_ChildIterator aSub(label(), false); aSub.More(); aSub.Next()) {
    Selector_Algo* aSubSel = restoreByLab(aSub.Value(), baseDocument());
    if (!append(aSubSel, false)) {
      break; // some empty label left in the end
    }
  }
  return true;
}

TDF_Label Selector_Container::restoreByName(std::string theName,
  const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator)
{
  myShapeType = theShapeType;
  TDF_Label aContext;
  for(size_t aStart = 0; aStart != std::string::npos; aStart = theName.find('[', aStart + 1)) {
    size_t anEndPos = theName.find(']', aStart + 1);
    if (anEndPos != std::string::npos) {
      // there could be sub-intersections, so, [[...]] case; searching for other open-bracket
      size_t aNextStart = theName.find('[', aStart + 1);
      while(aNextStart != std::string::npos && aNextStart < anEndPos) {
        anEndPos = theName.find(']', anEndPos + 1);
        if (anEndPos == std::string::npos) {
          return TDF_Label(); // invalid parentheses
        }
        aNextStart = theName.find('[', aNextStart + 1);
      }
      if (anEndPos == std::string::npos)
        return TDF_Label(); // invalid parentheses
      std::string aSubStr = theName.substr(aStart + 1, anEndPos - aStart - 1);
      TopAbs_ShapeEnum aSubShapeType = TopAbs_FACE;
      switch (myShapeType) {
      case TopAbs_COMPSOLID: aSubShapeType = TopAbs_SOLID; break;
      case TopAbs_WIRE: aSubShapeType = TopAbs_EDGE; break;
      default:;
      }
      TDF_Label aSubContext;
      Selector_Algo* aSubSel =
        Selector_Algo::restoreByName(
          newSubLabel(), baseDocument(), aSubStr, aSubShapeType, theNameGenerator, aSubContext);
      if (!append(aSubSel))
        return TDF_Label();

      if (aSubContext.IsNull()) {
        delete aSubSel;
        clearSubAlgos();
        return TDF_Label();
      }
      if (!aContext.IsNull() && !aContext.IsEqual(aSubContext)) {
        if (!theNameGenerator->isLater(aContext, aSubContext))
          aContext = aSubContext;
      } else {
        aContext = aSubContext;
      }
    } else
      return TDF_Label(); // invalid parentheses
    aStart = anEndPos; // for recursive parenthesis set start on the current end
  }
  return aContext;
}

bool Selector_Container::solve(const TopoDS_Shape& theContext)
{
  TopoDS_Shape aResult;

  TopoDS_Builder aBuilder;
  switch(myShapeType) {
  case TopAbs_COMPOUND: {
    TopoDS_Compound aComp;
    aBuilder.MakeCompound(aComp);
    aResult = aComp;
    break;
  }
  case TopAbs_COMPSOLID: {
    TopoDS_CompSolid aComp;
    aBuilder.MakeCompSolid(aComp);
    aResult = aComp;
    break;
  }
  case TopAbs_SHELL: {
    TopoDS_Shell aShell;
    aBuilder.MakeShell(aShell);
    aResult = aShell;
    break;
  }
  case TopAbs_WIRE: {
    TopoDS_Wire aWire;
    aBuilder.MakeWire(aWire);
    aResult = aWire;
    break;
  }
  }
  TopoDS_ListOfShape aSubSelectorShapes;
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    if (!(*aSubSel)->solve(theContext)) {
      return false;
    }
    aBuilder.Add(aResult, (*aSubSel)->value());
  }
  if (!aResult.IsNull()) {
    Selector_Algo::store(aResult);
    return true;
  }
  return false;
}

std::string Selector_Container::name(Selector_NameGenerator* theNameGenerator)
{
  std::string aResult;
  // add names of sub-components one by one in "[]"
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    aResult += '[';
    aResult += (*aSubSel)->name(theNameGenerator);
    aResult += ']';
  }
  return aResult;
}
