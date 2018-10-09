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

#include <Selector_Selector.h>

#include <TDF_ChildIDIterator.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Builder.hxx>
#include <TopTools_MapOfShape.hxx>

#include <TDataStd_Integer.hxx>
#include <TDataStd_ReferenceArray.hxx>

#include <list>

/// type of the selection, integerm keeps the Selector_Type value
static const Standard_GUID kSEL_TYPE("db174d59-c2e3-4a90-955e-55544df090d6");
/// type of the shape, stored in case it is intersection or container
static const Standard_GUID kSHAPE_TYPE("864b3267-cb9d-4107-bf58-c3ce1775b171");
//  reference attribute that contains the reference to labels where the "from" or "base" shapes
// of selection are located
static const Standard_GUID kBASE_ARRAY("7c515b1a-9549-493d-9946-a4933a22f45f");

Selector_Selector::Selector_Selector(TDF_Label theLab) : myLab(theLab)
{
}

TDF_Label Selector_Selector::label()
{
  return myLab;
}

// adds to theResult all labels that contain initial shapes for theValue located in theFinal
static void findBases(Handle(TNaming_NamedShape) theFinal, const TopoDS_Shape& theValue,
  TDF_LabelList& theResult)
{
  for(TNaming_Iterator aThisIter(theFinal); aThisIter.More(); aThisIter.Next()) {
    if (aThisIter.NewShape().IsSame(theValue)) {
      const TopoDS_Shape& anOldShape = aThisIter.OldShape();
      // searching for all old shapes in this sequence of modification
      TNaming_OldShapeIterator anOldIter(anOldShape, theFinal->Label());
      for(; anOldIter.More(); anOldIter.Next()) {
        TNaming_Evolution anEvolution = anOldIter.NamedShape()->Evolution();
        if (anEvolution == TNaming_PRIMITIVE) { // found a good candidate, a base shape
          // check that this is not in the results already
          const TDF_Label aResult = anOldIter.NamedShape()->Label();
          TDF_LabelList::Iterator aResIter(theResult);
          for(; aResIter.More(); aResIter.Next()) {
            if (aResIter.Value().IsEqual(aResult))
              break;
          }
          if (!aResIter.More()) // not found, so add this new
            theResult.Append(aResult);
        } else if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
          // continue recursively
          findBases(anOldIter.NamedShape(), anOldShape, theResult);
        }
      }
    }
  }
}

bool Selector_Selector::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  if (theValue.IsNull() || theContext.IsNull())
    return false;
  // check the value shape can be named as it is, or it is needed to construct it from the
  // higher level shapes (like a box vertex by faces that form this vertex)
  if (!TNaming_Tool::HasLabel(myLab, theValue)) {
    TopAbs_ShapeEnum aSelectionType = theValue.ShapeType();
    myShapeType = aSelectionType;
    if (aSelectionType == TopAbs_COMPOUND || aSelectionType == TopAbs_COMPSOLID ||
        aSelectionType == TopAbs_SHELL || aSelectionType == TopAbs_WIRE)
    { // iterate all sub-shapes and select them on sublabels
      for(TopoDS_Iterator aSubIter(theValue); aSubIter.More(); aSubIter.Next()) {
        if (!selectBySubSelector(theContext, aSubIter.Value())) {
          return false; // if some selector is failed, everything is failed
        }
      }
      myType = SELTYPE_CONTAINER;
      return true;
    }

    // try to find the shape of the higher level type in the context shape
    while(aSelectionType != TopAbs_FACE) {
      if (aSelectionType == TopAbs_VERTEX) aSelectionType = TopAbs_EDGE;
      else if (aSelectionType == TopAbs_EDGE) aSelectionType = TopAbs_FACE;
      TopTools_MapOfShape anIntersectors; // shapes of aSelectionType that contain theValue
      for(TopExp_Explorer aSelExp(theContext, aSelectionType); aSelExp.More(); aSelExp.Next()) {
        TopExp_Explorer aSubExp(aSelExp.Current(), theValue.ShapeType());
        for(; aSubExp.More(); aSubExp.Next()) {
          if (aSubExp.Current().IsSame(theValue)) {
            anIntersectors.Add(aSelExp.Current());
            break;
          }
        }
      }
      //TODO: check if intersectors produce several subs, try to remove one of the intersector

      //TODO: check that solution is only one

      // name the intersectors
      std::list<Selector_Selector> aSubSelList;
      TopTools_MapOfShape::Iterator anInt(anIntersectors);
      for (; anInt.More(); anInt.Next()) {
        if (!selectBySubSelector(theContext, anInt.Value())) {
          break; // if some selector is failed, stop and search another solution
        }
      }
      if (!anInt.More()) { // all intersectors were correctly named
        myType = SELTYPE_INTERSECT;
        return true;
      }
    }

    // TODO: searching by neighbours
    return false;
  }
  // searching for the base shapes of the value
  Handle(TNaming_NamedShape) aPrimitiveNS;
  NCollection_List<Handle(TNaming_NamedShape)> aModifList;
  for(TNaming_SameShapeIterator aShapes(theValue, myLab); aShapes.More(); aShapes.Next())
  {
    Handle(TNaming_NamedShape) aNS;
    if (aShapes.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      TNaming_Evolution anEvolution = aNS->Evolution();
      if (anEvolution == TNaming_PRIMITIVE) { // the value shape is declared as PRIMITIVE
        aPrimitiveNS = aNS;
        break;
      } else if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
        // check this is a new shape
        TNaming_Iterator aNSIter(aNS);
        for(; aNSIter.More(); aNSIter.Next())
          if (aNSIter.NewShape().IsSame(theValue))
            break;
        if (aNSIter.More()) // new was found
          aModifList.Append(aNS);
      }
    }
  }

  if (!aPrimitiveNS.IsNull()) {
    myType = SELTYPE_PRIMITIVE;
    myFinal = aPrimitiveNS->Label();
    return true;
  }

  if (aModifList.Extent() > 1) { // searching for the best modification result: by context
    Handle(TNaming_NamedShape) aCandidate;
    NCollection_List<Handle(TNaming_NamedShape)>::Iterator aModIter(aModifList);
    for(; !aModifList.IsEmpty() && aModIter.More(); aModIter.Next()) {
       aCandidate = aModIter.Value();
      TDF_Label aFatherLab = aCandidate->Label().Father();
      Handle(TNaming_NamedShape) aFatherNS;
      if (aFatherLab.FindAttribute(TNaming_NamedShape::GetID(), aFatherNS)) {
        for(TNaming_Iterator anIter(aFatherNS); anIter.More(); anIter.Next()) {
          if (theContext.IsSame(anIter.NewShape())) { // found the best modification
            aModifList.Clear();
            break;
          }
        }
      }
    }
    // take the best candidate, or the last in the iteration
    aModifList.Clear();
    aModifList.Append(aCandidate);
  }

  if (!aModifList.IsEmpty()) {
    // searching for all the base shapes of this modification
    findBases(aModifList.First(), theValue, myBases);
    myFinal = aModifList.First()->Label();
    myType = SELTYPE_MODIFICATION;
    return !myBases.IsEmpty();
  }

  // not found a good result
  return false;
}

void Selector_Selector::store()
{
  myLab.ForgetAllAttributes(true); // remove old naming data
  TDataStd_Integer::Set(myLab, kSEL_TYPE, (int)myType);
  switch(myType) {
  case SELTYPE_CONTAINER:
  case SELTYPE_INTERSECT: {
    TDataStd_Integer::Set(myLab, kSHAPE_TYPE, (int)myShapeType);
    // store also all sub-selectors
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++) {
      aSubSel->store();
    }
    break;
  }
  case SELTYPE_PRIMITIVE: {
    Handle(TDataStd_ReferenceArray) anArray =
      TDataStd_ReferenceArray::Set(myLab, kBASE_ARRAY, 0, 0);
    anArray->SetValue(0, myFinal);
    break;
  }
  case SELTYPE_MODIFICATION: {
    Handle(TDataStd_ReferenceArray) anArray =
      TDataStd_ReferenceArray::Set(myLab, kBASE_ARRAY, 0, myBases.Extent());
    TDF_LabelList::Iterator aBIter(myBases);
    for(int anIndex = 0; aBIter.More(); aBIter.Next(), anIndex++) {
      anArray->SetValue(anIndex, aBIter.Value());
    }
    anArray->SetValue(myBases.Extent(), myFinal); // final is in the end of array
    break;
  }
  default: { // unknown case
    break;
  }
  }
}

bool Selector_Selector::restore()
{
  Handle(TDataStd_Integer) aTypeAttr;
  if (!myLab.FindAttribute(kSEL_TYPE, aTypeAttr))
    return false;
  myType = Selector_Type(aTypeAttr->Get());
  switch(myType) {
  case SELTYPE_CONTAINER:
  case SELTYPE_INTERSECT: {
    Handle(TDataStd_Integer) aShapeTypeAttr;
    if (!myLab.FindAttribute(kSHAPE_TYPE, aShapeTypeAttr))
      return false;
    myShapeType = TopAbs_ShapeEnum(aShapeTypeAttr->Get());
    // restore sub-selectors
    bool aSubResult = true;
    mySubSelList.clear();
    for(TDF_ChildIDIterator aSub(myLab, kSEL_TYPE, false); aSub.More(); aSub.Next()) {
      mySubSelList.push_back(Selector_Selector(aSub.Value()->Label()));
      if (!mySubSelList.back().restore())
        aSubResult = false;
    }
    return aSubResult;
  }
  case SELTYPE_PRIMITIVE: {
    Handle(TDataStd_ReferenceArray) anArray;
    if (myLab.FindAttribute(kBASE_ARRAY, anArray)) {
      myFinal = anArray->Value(0);
      return true;
    }
    return false;
  }
  case SELTYPE_MODIFICATION: {
    Handle(TDataStd_ReferenceArray) anArray;
    if (myLab.FindAttribute(kBASE_ARRAY, anArray)) {
      int anUpper = anArray->Upper();
      for(int anIndex = 0; anIndex < anUpper; anIndex++) {
        myBases.Append(anArray->Value(anIndex));
      }
      myFinal = anArray->Value(anUpper);
      return true;
    }
    return false;
  }
  default: { // unknown case
  }
  }
  return false;
}

/// Returns in theResults all shapes with history started in theBase and ended in theFinal
static void findFinals(const TopoDS_Shape& theBase, const TDF_Label& theFinal,
  TopTools_MapOfShape& theResults)
{
  for(TNaming_NewShapeIterator aBaseIter(theBase, theFinal); aBaseIter.More(); aBaseIter.Next()) {
    TNaming_Evolution anEvolution = aBaseIter.NamedShape()->Evolution();
    if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
      if (aBaseIter.NamedShape()->Label().IsEqual(theFinal)) {
        theResults.Add(aBaseIter.Shape());
      } else {
        findFinals(aBaseIter.Shape(), theFinal, theResults);
      }
    }
  }

}

bool Selector_Selector::solve()
{
  TopoDS_Shape aResult; // null if invalid
  switch(myType) {
  case SELTYPE_CONTAINER: {
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
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++) {
      if (!aSubSel->solve()) {
        return false;
      }
      aBuilder.Add(aResult, aSubSel->value());
    }
    break;
  }
  case SELTYPE_INTERSECT: {
    TopoDS_ListOfShape aCommon; // common sub shapes in each sub-selector (a result)
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++) {
      if (!aSubSel->solve()) {
        return false;
      }
      TopoDS_Shape anArg = aSubSel->value();
      TopTools_MapOfShape aCurrentMap;
      for(TopExp_Explorer anExp(anArg, myShapeType); anExp.More(); anExp.Next()) {
        if (aCurrentMap.Add(anExp.Current()) && aSubSel == mySubSelList.begin())
          aCommon.Append(anExp.Current());
      }
      if (aSubSel != mySubSelList.begin()) { // remove from common shapes not in aCurrentMap
        for(TopoDS_ListOfShape::Iterator aComIter(aCommon); aComIter.More(); ) {
          if (aCurrentMap.Contains(aComIter.Value()))
            aComIter.Next();
          else
            aCommon.Remove(aComIter);
        }
      }
    }
    if (aCommon.Extent() != 1)
      return false;
    aResult = aCommon.First();
    break;
  }
  case SELTYPE_PRIMITIVE: {
    Handle(TNaming_NamedShape) aNS;
    if (myFinal.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      aResult = aNS->Get();
    }
  }
  case SELTYPE_MODIFICATION: {
    TopoDS_ListOfShape aFinalsCommon; // final shapes presented in all results from bases
    TDF_LabelList::Iterator aBase(myBases);
    for(; aBase.More(); aBase.Value()) {
      TopTools_MapOfShape aFinals;
      for(TNaming_Iterator aBaseShape(aBase.Value()); aBaseShape.More(); aBaseShape.Next())
        findFinals(aBaseShape.NewShape(), myFinal, aFinals);
      if (!aFinals.IsEmpty()) {
        if (aFinalsCommon.IsEmpty()) { // just copy all to common
          for(TopTools_MapOfShape::Iterator aFinal(aFinals); aFinal.More(); aFinal.Next()) {
            aFinalsCommon.Append(aFinal.Key());
          }
        } else { // keep only shapes presented in both lists
          TopoDS_ListOfShape::Iterator aCommon(aFinalsCommon);
          for(; aCommon.More(); aCommon.Next()) {
            if (aFinals.Contains(aCommon.Value())) {
              aCommon.Next();
            } else { // common is not found, remove it
              aFinalsCommon.Remove(aCommon);
            }
          }
        }
      }
    }
    if (aFinalsCommon.Extent() == 1) // only in this case result is valid: found only one shape
      aResult = aFinalsCommon.First();
  }
  default: { // unknown case
  }
  }

  TNaming_Builder aBuilder(myLab);
  if (!aResult.IsNull()) {
    aBuilder.Select(aResult, aResult);
    return true;
  }
  return false; // builder just erases the named shape in case of error
}

TopoDS_Shape Selector_Selector::value()
{
  Handle(TNaming_NamedShape) aNS;
  if (myLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
    return aNS->Get();
  return TopoDS_Shape(); // empty, error shape
}

bool Selector_Selector::selectBySubSelector(const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  mySubSelList.push_back(Selector_Selector(myLab.FindChild(int(mySubSelList.size()) + 1)));
  if (!mySubSelList.back().select(theContext, theValue)) {
    mySubSelList.clear(); // if one of the selector is failed, all become invalid
    return false;
  }
  return true;
}
