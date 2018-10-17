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

#include <Selector_NameGenerator.h>
#include <Selector_NExplode.h>

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
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>

#include <list>

/// type of the selection, integerm keeps the Selector_Type value
static const Standard_GUID kSEL_TYPE("db174d59-c2e3-4a90-955e-55544df090d6");
/// type of the shape, stored in case it is intersection or container
static const Standard_GUID kSHAPE_TYPE("864b3267-cb9d-4107-bf58-c3ce1775b171");
//  reference attribute that contains the reference to labels where the "from" or "base" shapes
// of selection are located
static const Standard_GUID kBASE_ARRAY("7c515b1a-9549-493d-9946-a4933a22f45f");
// array of the neighbor levels
static const Standard_GUID kLEVELS_ARRAY("ee4c4b45-e859-4e86-aa4f-6eac68e0ca1f");
// weak index (integer) of the sub-shape
static const Standard_GUID kWEAK_INDEX("e9373a61-cabc-4ee8-aabf-aea47c62ed87");

Selector_Selector::Selector_Selector(TDF_Label theLab) : myLab(theLab)
{
  myWeakIndex = -1;
}

TDF_Label Selector_Selector::label()
{
  return myLab;
}

// adds to theResult all labels that contain initial shapes for theValue located in theFinal
static void findBases(Handle(TNaming_NamedShape) theFinal, const TopoDS_Shape& theValue,
  bool aMustBeAtFinal, TDF_LabelList& theResult)
{
  TNaming_SameShapeIterator aLabIter(theValue, theFinal->Label());
  for(; aLabIter.More(); aLabIter.Next()) {
    Handle(TNaming_NamedShape) aNS;
    aLabIter.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS);
    if (aMustBeAtFinal && aNS != theFinal)
      continue; // looking for old at the same final label only
    TNaming_Evolution anEvolution = aNS->Evolution();
    if (anEvolution == TNaming_PRIMITIVE) {
      // check that this is not in the results already
      const TDF_Label aResult = aNS->Label();
      TDF_LabelList::Iterator aResIter(theResult);
      for(; aResIter.More(); aResIter.Next()) {
        if (aResIter.Value().IsEqual(aResult))
          break;
      }
      if (!aResIter.More()) // not found, so add this new
        theResult.Append(aResult);
    }
    if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
      for(TNaming_Iterator aThisIter(aNS); aThisIter.More(); aThisIter.Next()) {
        if (aThisIter.NewShape().IsSame(theValue)) {
          // continue recursively, null NS means that any NS are ok
          findBases(theFinal, aThisIter.OldShape(), false, theResult);
        }
      }
    }
  }
}

// returns the sub-shapes of theSubType which belong to all theShapes (so, common or intersection)
static void commonShapes(const TopoDS_ListOfShape& theShapes, TopAbs_ShapeEnum theSubType,
  TopoDS_ListOfShape& theResults)
{
  TopoDS_ListOfShape::iterator aSubSel = theShapes.begin();
  for(; aSubSel != theShapes.end(); aSubSel++) {
    TopTools_MapOfShape aCurrentMap;
    for(TopExp_Explorer anExp(*aSubSel, theSubType); anExp.More(); anExp.Next()) {
      if (aCurrentMap.Add(anExp.Current()) && aSubSel == theShapes.begin())
        theResults.Append(anExp.Current());
    }
    if (aSubSel != theShapes.begin()) { // remove from common shapes not in aCurrentMap
      for(TopoDS_ListOfShape::Iterator aComIter(theResults); aComIter.More(); ) {
        if (aCurrentMap.Contains(aComIter.Value()))
          aComIter.Next();
        else
          theResults.Remove(aComIter);
      }
    }
  }
}

/// Searches neighbor of theLevel of neighborhood to theValue in theContex
static void findNeighbors(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
  const int theLevel, TopTools_MapOfShape& theResult)
{
  TopAbs_ShapeEnum aConnectorType = TopAbs_VERTEX; // type of the connector sub-shapes
  if (theValue.ShapeType() == TopAbs_FACE)
    aConnectorType = TopAbs_EDGE;
  TopTools_MapOfShape aNBConnectors; // connector shapes that already belong to neighbors
  for(TopExp_Explorer aValExp(theValue, aConnectorType); aValExp.More(); aValExp.Next()) {
    aNBConnectors.Add(aValExp.Current());
  }

  TopTools_MapOfShape alreadyProcessed;
  alreadyProcessed.Add(theValue);

  for(int aLevel = 1; aLevel <= theLevel; aLevel++) {
    TopoDS_ListOfShape aGoodCandidates;
    TopExp_Explorer aCandidate(theContext, theValue.ShapeType());
    for(; aCandidate.More(); aCandidate.Next()) {
      if (alreadyProcessed.Contains(aCandidate.Current()))
        continue;
      TopExp_Explorer aCandConnector(aCandidate.Current(), aConnectorType);
      for(; aCandConnector.More(); aCandConnector.Next()) {
        if (aNBConnectors.Contains(aCandConnector.Current())) // candidate is neighbor
          break;
      }
      if (aCandConnector.More()) {
        if (aLevel == theLevel) { // add a NB into result: it is connected to other neighbors
          theResult.Add(aCandidate.Current());
        } else { // add to the NB of the current level
          aGoodCandidates.Append(aCandidate.Current());
        }
      }
    }
    if (aLevel != theLevel) { // good candidates are added to neighbor of this level by connectors
      for(TopoDS_ListOfShape::Iterator aGood(aGoodCandidates); aGood.More(); aGood.Next()) {
        TopExp_Explorer aGoodConnector(aGood.Value(), aConnectorType);
        for(; aGoodConnector.More(); aGoodConnector.Next()) {
          aNBConnectors.Add(aGoodConnector.Current());
        }
        alreadyProcessed.Add(aGood.Value());
      }
    }
  }
}

/// Searches the neighbor shape by neighbors defined in theNB in theContext shape
static const TopoDS_Shape findNeighbor(const TopoDS_Shape theContext,
  const std::list<std::pair<TopoDS_Shape, int> >& theNB)
{
  // searching for neighbors with minimum level
  int aMinLevel = 0;
  std::list<std::pair<TopoDS_Shape, int> >::const_iterator aNBIter = theNB.cbegin();
  for(; aNBIter != theNB.cend(); aNBIter++) {
    if (aMinLevel == 0 || aNBIter->second < aMinLevel) {
      aMinLevel = aNBIter->second;
    }
  }
  // collect all neighbors which are neighbors of sub-shapes with minimum level
  bool aFirst = true;
  TopoDS_ListOfShape aMatches;
  for(aNBIter = theNB.cbegin(); aNBIter != theNB.cend(); aNBIter++) {
    if (aNBIter->second == aMinLevel) {
      TopTools_MapOfShape aThisNBs;
      findNeighbors(theContext, aNBIter->first, aMinLevel, aThisNBs);
      // aMatches must contain common part of all NBs lists
      for(TopTools_MapOfShape::Iterator aThisNB(aThisNBs); aThisNB.More(); aThisNB.Next()) {
        if (aFirst) {
          aMatches.Append(aThisNB.Value());
        } else {
          // remove all in aMatches which are not in this NBs
          for(TopoDS_ListOfShape::Iterator aMatch(aMatches); aMatch.More(); ) {
            if (aThisNBs.Contains(aMatch.Value())) {
              aMatch.Next();
            } else {
              aMatches.Remove(aMatch);
            }
          }
        }
      }
      aFirst = false;
    }
  }
  if (aMatches.IsEmpty())
    return TopoDS_Shape(); // not found any candidate
  if (aMatches.Extent() == 1)
    return aMatches.First(); // already found good candidate
  // iterate all matches to find by other (higher level) neighbors the best candidate
  TopoDS_Shape aGoodCandidate;
  for(TopoDS_ListOfShape::Iterator aCandidate(aMatches); aCandidate.More(); aCandidate.Next()) {
    bool aValidCadidate = true;
    for(int aLevel = aMinLevel + 1; true; aLevel++) {
      bool aFooundHigherLevel = false;
      TopoDS_ListOfShape aLevelNBs;
      for(aNBIter = theNB.cbegin(); aNBIter != theNB.cend(); aNBIter++) {
        if (aNBIter->second == aLevel)
          aLevelNBs.Append(aNBIter->first);
        else if (aNBIter->second >= aLevel)
          aFooundHigherLevel = true;
      }
      if (!aFooundHigherLevel && aLevelNBs.IsEmpty()) { // iterated all, so, good candidate
        if (aGoodCandidate.IsNull()) {
          aGoodCandidate = aCandidate.Value();
        } else { // too many good candidates
          return TopoDS_Shape();
        }
      }
      if (!aLevelNBs.IsEmpty()) {
        TopTools_MapOfShape aNBsOfCandidate;
        findNeighbors(theContext, aCandidate.Value(), aLevel, aNBsOfCandidate);
        // check all stored neighbors are in the map of real neighbors
        for(TopoDS_ListOfShape::Iterator aLevIter(aLevelNBs); aLevIter.More(); aLevIter.Next()) {
          if (!aNBsOfCandidate.Contains(aLevIter.Value())) {
            aValidCadidate = false;
            break;
          }
        }
      }
      if (!aValidCadidate) // candidate is not valid, break the checking
        break;
    }
  }
  return aGoodCandidate;
}

bool Selector_Selector::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
  const bool theUseNeighbors)
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
    bool aFacesTried = false; // for identification of vertices, faces are tried, then edges
    while(aSelectionType != TopAbs_FACE || !aFacesTried) {
      if (aSelectionType == TopAbs_FACE) {
        if (theValue.ShapeType() != TopAbs_VERTEX)
          break;
        aFacesTried = true;
        aSelectionType = TopAbs_EDGE;
      } else
        aSelectionType = TopAbs_FACE;
      TopTools_MapOfShape anIntersectors; // shapes of aSelectionType that contain theValue
      TopoDS_ListOfShape anIntList; // same as anIntersectors
      for(TopExp_Explorer aSelExp(theContext, aSelectionType); aSelExp.More(); aSelExp.Next()) {
        TopExp_Explorer aSubExp(aSelExp.Current(), theValue.ShapeType());
        for(; aSubExp.More(); aSubExp.Next()) {
          if (aSubExp.Current().IsSame(theValue)) {
            if (anIntersectors.Add(aSelExp.Current()))
              anIntList.Append(aSelExp.Current());
            break;
          }
        }
      }
      // check that solution is only one
      TopoDS_ListOfShape aCommon;
      commonShapes(anIntList, theValue.ShapeType(), aCommon);
      if (aCommon.Extent() == 1 && aCommon.First().IsSame(theValue)) {
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
    }

    if (!theUseNeighbors)
      return false;

    // searching by neighbours
    std::list<std::pair<TopoDS_Shape, int> > aNBs; /// neighbor sub-shape -> level of neighborhood
    for(int aLevel = 1; true; aLevel++) {
      TopTools_MapOfShape aNewNB;
      findNeighbors(theContext, theValue, aLevel, aNewNB);
      if (aNewNB.Extent() == 0) { // there are no neighbors of the given level, stop iteration
        break;
      }
      // check which can be named correctly, without by neighbors type
      for(TopTools_MapOfShape::Iterator aNBIter(aNewNB); aNBIter.More(); ) {
        Selector_Selector aSelector(myLab.FindChild(1));
        if (aSelector.select(theContext, aNBIter.Value(), false)) { // add to the list of good NBs
          aNBs.push_back(std::pair<TopoDS_Shape, int>(aNBIter.Value(), aLevel));
        }
        aNewNB.Remove(aNBIter.Key());
        aNBIter.Initialize(aNewNB);
      }
      TopoDS_Shape aResult = findNeighbor(theContext, aNBs);
      if (!aResult.IsNull() && aResult.IsSame(theValue)) {
        std::list<std::pair<TopoDS_Shape, int> >::iterator aNBIter = aNBs.begin();
        for(; aNBIter != aNBs.end(); aNBIter++) {
          if (!selectBySubSelector(theContext, aNBIter->first, false)) {
            return false; // something is wrong because before this selection was ok
          }
          myNBLevel.push_back(aNBIter->second);

        }
        myType = SELTYPE_FILTER_BY_NEIGHBOR;
        return true;
      }
    }
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
    findBases(aModifList.First(), theValue, true, myBases);
    if (!myBases.IsEmpty()) {
      myFinal = aModifList.First()->Label();
      TopoDS_ListOfShape aCommon;
      findModificationResult(aCommon);
      // trying to search by neighbours
      if (aCommon.Extent() > 1) { // more complicated selection
        if (!theUseNeighbors)
          return false;

        // searching by neighbours
        std::list<std::pair<TopoDS_Shape, int> > aNBs; /// neighbor sub-shape -> level of neighborhood
        for(int aLevel = 1; true; aLevel++) {
          TopTools_MapOfShape aNewNB;
          findNeighbors(theContext, theValue, aLevel, aNewNB);
          if (aNewNB.Extent() == 0) { // there are no neighbors of the given level, stop iteration
            break;
          }
          // check which can be named correctly, without by neighbors type
          for(TopTools_MapOfShape::Iterator aNBIter(aNewNB); aNBIter.More(); ) {
            Selector_Selector aSelector(myLab.FindChild(1));
            if (aSelector.select(theContext, aNBIter.Value(), false)) { // add to the list of good NBs
              aNBs.push_back(std::pair<TopoDS_Shape, int>(aNBIter.Value(), aLevel));
            }
            aNewNB.Remove(aNBIter.Key());
            aNBIter.Initialize(aNewNB);
          }
          TopoDS_Shape aResult = findNeighbor(theContext, aNBs);
          if (!aResult.IsNull() && aResult.IsSame(theValue)) {
            std::list<std::pair<TopoDS_Shape, int> >::iterator aNBIter = aNBs.begin();
            for(; aNBIter != aNBs.end(); aNBIter++) {
              if (!selectBySubSelector(theContext, aNBIter->first)) {
                return false; // something is wrong because before this selection was ok
              }
              myNBLevel.push_back(aNBIter->second);

            }
            myType = SELTYPE_FILTER_BY_NEIGHBOR;
            return true;
          }
        }
        // filter by neighbours did not help
        if (aCommon.Extent() > 1) { // weak naming between the common results
          Selector_NExplode aNexp(aCommon);
          myWeakIndex = aNexp.index(theValue);
          if (myWeakIndex == -1)
            return false;
        }
      }
    }
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
    if (myWeakIndex != -1) {
      TDataStd_Integer::Set(myLab, kWEAK_INDEX, myWeakIndex);
    }
    break;
  }
  case SELTYPE_FILTER_BY_NEIGHBOR: {
    TDataStd_Integer::Set(myLab, kSHAPE_TYPE, (int)myShapeType);
    // store numbers of levels corresponded to the neighbors in sub-selectors
    Handle(TDataStd_IntegerArray) anArray =
      TDataStd_IntegerArray::Set(myLab, kLEVELS_ARRAY, 0, int(myNBLevel.size()) - 1);
    std::list<int>::iterator aLevel = myNBLevel.begin();
    for(int anIndex = 0; aLevel != myNBLevel.end(); aLevel++, anIndex++) {
      anArray->SetValue(anIndex, *aLevel);
    }
    // store all sub-selectors
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++) {
      aSubSel->store();
    }
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
      Handle(TDataStd_Integer) aWeakInt;
      if (myLab.FindAttribute(kWEAK_INDEX, aWeakInt)) {
        myWeakIndex = aWeakInt->Get();
      }
      return true;
    }
    return false;
  }
  case SELTYPE_FILTER_BY_NEIGHBOR: {
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
    // restore levels indices
    Handle(TDataStd_IntegerArray) anArray;
    if (!myLab.FindAttribute(kLEVELS_ARRAY, anArray))
      return false;
    for(int anIndex = 0; anIndex <= anArray->Upper(); anIndex++) {
      myNBLevel.push_back(anArray->Value(anIndex));
    }
    return true;
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

void Selector_Selector::findModificationResult(TopoDS_ListOfShape& theCommon) {
  for(TDF_LabelList::Iterator aBase(myBases); aBase.More(); aBase.Next()) {
    TopTools_MapOfShape aFinals;
    for(TNaming_Iterator aBaseShape(aBase.Value()); aBaseShape.More(); aBaseShape.Next())
      findFinals(aBaseShape.NewShape(), myFinal, aFinals);
    if (!aFinals.IsEmpty()) {
      if (theCommon.IsEmpty()) { // just copy all to common
        for(TopTools_MapOfShape::Iterator aFinal(aFinals); aFinal.More(); aFinal.Next()) {
          theCommon.Append(aFinal.Key());
        }
      } else { // keep only shapes presented in both lists
        for(TopoDS_ListOfShape::Iterator aCommon(theCommon); aCommon.More(); ) {
          if (aFinals.Contains(aCommon.Value())) {
            aCommon.Next();
          } else { // common is not found, remove it
            theCommon.Remove(aCommon);
          }
        }
      }
    }
  }
}

bool Selector_Selector::solve(const TopoDS_Shape& theContext)
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
      if (!aSubSel->solve(theContext)) {
        return false;
      }
      aBuilder.Add(aResult, aSubSel->value());
    }
    break;
  }
  case SELTYPE_INTERSECT: {
    TopoDS_ListOfShape aSubSelectorShapes;
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++) {
      if (!aSubSel->solve(theContext)) {
        return false;
      }
      aSubSelectorShapes.Append(aSubSel->value());
    }
    TopoDS_ListOfShape aCommon; // common sub shapes in each sub-selector (a result)
    commonShapes(aSubSelectorShapes, myShapeType, aCommon);
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
    findModificationResult(aFinalsCommon);
    if (aFinalsCommon.Extent() == 1) // only in this case result is valid: found only one shape
      aResult = aFinalsCommon.First();
    else if (aFinalsCommon.Extent() > 1 && myWeakIndex) {
      Selector_NExplode aNExp(aFinalsCommon);
      aResult = aNExp.shape(myWeakIndex);

    }
    break;
  }
  case SELTYPE_FILTER_BY_NEIGHBOR: {
    std::list<std::pair<TopoDS_Shape, int> > aNBs; /// neighbor sub-shape -> level of neighborhood
    std::list<int>::iterator aLevel = myNBLevel.begin();
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++, aLevel++) {
      if (!aSubSel->solve(theContext)) {
        return false;
      }
      aNBs.push_back(std::pair<TopoDS_Shape, int>(aSubSel->value(), *aLevel));
    }
    aResult = findNeighbor(theContext, aNBs);
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

std::string Selector_Selector::name(Selector_NameGenerator* theNameGenerator) {
  switch(myType) {
  case SELTYPE_CONTAINER:
  case SELTYPE_INTERSECT: {
    std::string aResult;
    // add names of sub-components one by one separated by "&"
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++) {
      if (aSubSel != mySubSelList.begin())
        aResult += "&";
      aResult += aSubSel->name(theNameGenerator);
    }
    return aResult;
  }
  case SELTYPE_PRIMITIVE: {
    Handle(TDataStd_Name) aName;
    if (!myFinal.FindAttribute(TDataStd_Name::GetID(), aName))
      return "";
    return theNameGenerator->contextName(myFinal) + "/" +
      std::string(TCollection_AsciiString(aName->Get()).ToCString());
  }
  case SELTYPE_MODIFICATION: {
    // final&/base1&base2
    std::string aResult;
    Handle(TDataStd_Name) aName;
    if (!myFinal.FindAttribute(TDataStd_Name::GetID(), aName))
      return "";
    aResult += theNameGenerator->contextName(myFinal) + "/" +
      std::string(TCollection_AsciiString(aName->Get()).ToCString()) + "&/";
    for(TDF_LabelList::iterator aBase = myBases.begin(); aBase != myBases.end(); aBase++) {
      if (aBase != myBases.begin())
        aResult += "&";
      if (!aBase->FindAttribute(TDataStd_Name::GetID(), aName))
        return "";
      aResult += theNameGenerator->contextName(*aBase) + "/" +
        std::string(TCollection_AsciiString(aName->Get()).ToCString());
      if (myWeakIndex != -1) {
        std::ostringstream aWeakStr;
        aWeakStr<<"&weak_name_"<<myWeakIndex;
        aResult += aWeakStr.str();
      }
    }
    return aResult;
  }
  case SELTYPE_FILTER_BY_NEIGHBOR: {
    // (nb1)level_if_more_than_1&(nb2)level_if_more_than_1&(nb3)level_if_more_than_1
    std::string aResult;
    std::list<int>::iterator aLevel = myNBLevel.begin();
    std::list<Selector_Selector>::iterator aSubSel = mySubSelList.begin();
    for(; aSubSel != mySubSelList.end(); aSubSel++, aLevel++) {
      if (aSubSel != mySubSelList.begin())
        aResult += "&";
      aResult += "(" + aSubSel->name(theNameGenerator) + ")";
      if (*aLevel > 1)
        aResult += *aLevel;
    }
    return aResult;
  }
  default: { // unknown case
  }
  };
  return "";
}

TDF_Label Selector_Selector::restoreByName(std::string theName,
  Selector_NameGenerator* theNameGenerator)
{
  if (theName.find('&') == std::string::npos) { // wihtout '&' it can be only primitive
    myType = SELTYPE_PRIMITIVE;
    TDF_Label aContext;
    if (theNameGenerator->restoreContext(theName, aContext, myFinal)) {
      if (!myFinal.IsNull())
        return aContext;
    }
  } else {
  }
  return TDF_Label();
}

bool Selector_Selector::selectBySubSelector(
  const TopoDS_Shape theContext, const TopoDS_Shape theValue, const bool theUseNeighbors)
{
  mySubSelList.push_back(Selector_Selector(myLab.FindChild(int(mySubSelList.size()) + 1)));
  if (!mySubSelList.back().select(theContext, theValue, theUseNeighbors)) {
    mySubSelList.clear(); // if one of the selector is failed, all become invalid
    return false;
  }
  return true;
}
