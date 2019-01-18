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

#include <Selector_FilterByNeighbors.h>

#include <Selector_NameGenerator.h>

#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_ChildIterator.hxx>

// array of the neighbor levels
static const Standard_GUID kLEVELS_ARRAY("ee4c4b45-e859-4e86-aa4f-6eac68e0ca1f");

Selector_FilterByNeighbors::Selector_FilterByNeighbors() : Selector_AlgoWithSubs()
{}

/// Searches neighbor of theLevel of neighborhood to theValue in theContex
static void findNeighbors(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
  const int theLevel, TopTools_MapOfShape& theResult)
{
  TopAbs_ShapeEnum aConnectorType = TopAbs_VERTEX; // type of the connector sub-shapes
  TopAbs_ShapeEnum aValueType = theValue.ShapeType();
  if (aValueType == TopAbs_FACE)
    aConnectorType = TopAbs_EDGE;
  else if (aValueType == TopAbs_COMPOUND) { // for geometrical naming: compound of faces
    TopExp_Explorer anExp(theValue, TopAbs_FACE);
    if (anExp.More()) {
      aConnectorType = TopAbs_EDGE;
      aValueType = TopAbs_FACE;
    } else {
      aValueType = TopAbs_EDGE;
    }
  }
  TopTools_MapOfShape aNBConnectors; // connector shapes that already belong to neighbors
  for(TopExp_Explorer aValExp(theValue, aConnectorType); aValExp.More(); aValExp.Next())
    aNBConnectors.Add(aValExp.Current());

  TopTools_MapOfShape alreadyProcessed;
  if (aValueType == theValue.ShapeType())
    alreadyProcessed.Add(theValue);
  else
    for(TopExp_Explorer aValExp(theValue, aValueType); aValExp.More(); aValExp.Next())
      alreadyProcessed.Add(aValExp.Current());

  for(int aLevel = 1; aLevel <= theLevel; aLevel++) {
    TopoDS_ListOfShape aGoodCandidates;
    TopExp_Explorer aCandidate(theContext, aValueType);
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
  const std::list<std::pair<TopoDS_Shape, int> >& theNB, const bool theGeometrical)
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
      if (aFirst) { // aMatches must contain common part of all NBs lists
        for(TopTools_MapOfShape::Iterator aThisNB(aThisNBs); aThisNB.More(); aThisNB.Next()) {
          aMatches.Append(aThisNB.Value());
        }
      } else { // remove all in aMatches which are not in this NBs
        for(TopoDS_ListOfShape::Iterator aMatch(aMatches); aMatch.More(); ) {
          if (aThisNBs.Contains(aMatch.Value())) {
            aMatch.Next();
          } else {
            aMatches.Remove(aMatch);
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
  TopoDS_Compound aResultCompound; // in case of geometrical name and many candidates
  // iterate all matches to find by other (higher level) neighbors the best candidate
  TopoDS_Shape aGoodCandidate;
  TopTools_MapOfShape aGoodCandidates; // already added good candidates to the map
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
        } else { // another good candidate
          if (theGeometrical && Selector_Algo::sameGeometry(aGoodCandidate, aCandidate.Value())) {
            if (!aGoodCandidates.Add(aCandidate.Value()))
              break;
            static TopoDS_Builder aBuilder;
            if (aResultCompound.IsNull()) {
              aBuilder.MakeCompound(aResultCompound);
              aBuilder.Add(aResultCompound, aGoodCandidate);
            }
            aBuilder.Add(aResultCompound, aCandidate.Value());
          } else
            return TopoDS_Shape();
        }
        break; // no more NBs with higher levels
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
  if (!aResultCompound.IsNull())
    return aResultCompound;
  return aGoodCandidate;
}

bool Selector_FilterByNeighbors::select(
  const TDF_Label theContextLab, const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  myShapeType = theValue.ShapeType();
  myContext = theContextLab;
  // searching by neighbors
  std::list<std::pair<TopoDS_Shape, int> > aNBs; /// neighbor sub-shape -> level of neighborhood
  for(int aLevel = 1; true; aLevel++) {
    TopTools_MapOfShape aNewNB;
    findNeighbors(theContext, theValue, aLevel, aNewNB);
    if (aNewNB.Extent() == 0) { // there are no neighbors of the given level, stop iteration
      break;
    }
    // iterate by the order in theContext to keep same naming names
    TopExp_Explorer anOrder(theContext, theValue.ShapeType());
    TopTools_MapOfShape alreadyProcessed;
    for (; anOrder.More(); anOrder.Next()) {
      if (alreadyProcessed.Add(anOrder.Current()) && aNewNB.Contains(anOrder.Current())) {
        TopoDS_Shape aNewNBShape = anOrder.Current();
        // check which can be named correctly, without "by neighbors" type
        Selector_Algo* aSubAlgo = Selector_Algo::select(theContext, aNewNBShape,
          newSubLabel(), baseDocument(), geometricalNaming(), false, useIntersections());
        if (aSubAlgo) {
          // add to list of good NBs
          aNBs.push_back(std::pair<TopoDS_Shape, int>(aNewNBShape, aLevel));
        }
        delete aSubAlgo; // don't keep this sub-algo until all subs and whole validity are checked
      }
    }
    TopoDS_Shape aResult = findNeighbor(theContext, aNBs, geometricalNaming());
    if (!aResult.IsNull() && aResult.IsSame(theValue)) {
      std::list<std::pair<TopoDS_Shape, int> >::iterator aNBIter = aNBs.begin();
      for(; aNBIter != aNBs.end(); aNBIter++) {
        Selector_Algo* aSubAlgo = Selector_Algo::select(theContext, aNBIter->first,
          newSubLabel(), baseDocument(), geometricalNaming(), false, useIntersections());
        if (append(aSubAlgo)) {
          myNBLevel.push_back(aNBIter->second);
        } else {
          delete aSubAlgo;
        }
      }
      return true;
    }
  }
  return false;
}

void Selector_FilterByNeighbors::store()
{
  storeType(Selector_Algo::SELTYPE_FILTER_BY_NEIGHBOR);
  TDataStd_Integer::Set(label(), shapeTypeID(), (int)myShapeType);
  // store numbers of levels corresponded to the neighbors in sub-selectors
  Handle(TDataStd_IntegerArray) anArray =
    TDataStd_IntegerArray::Set(label(), kLEVELS_ARRAY, 0, int(myNBLevel.size()) - 1);
  std::list<int>::iterator aLevel = myNBLevel.begin();
  for(int anIndex = 0; aLevel != myNBLevel.end(); aLevel++, anIndex++) {
    anArray->SetValue(anIndex, Abs(*aLevel));
  }
  // store all sub-selectors
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    (*aSubSel)->store();
  }
  // store context reference if exists
  if (!myContext.IsNull()) {
    static const TDF_LabelList anEmptyRefList;
    storeBaseArray(anEmptyRefList, myContext);
  }
}

bool Selector_FilterByNeighbors::restore()
{
  Handle(TDataStd_Integer) aShapeTypeAttr;
  if (!label().FindAttribute(shapeTypeID(), aShapeTypeAttr))
    return false;
  myShapeType = TopAbs_ShapeEnum(aShapeTypeAttr->Get());
  // restore levels indices
  Handle(TDataStd_IntegerArray) anArray;
  if (!label().FindAttribute(kLEVELS_ARRAY, anArray))
    return false;
  for(int anIndex = 0; anIndex <= anArray->Upper(); anIndex++) {
    myNBLevel.push_back(anArray->Value(anIndex));
  }
  // restore sub-selectors
  bool aSubResult = true;
  for(TDF_ChildIterator aSub(label(), false); aSub.More(); aSub.Next()) {
    Selector_Algo* aSubSel = restoreByLab(aSub.Value(), baseDocument());
    if (!append(aSubSel, false)) {
      if (!aSub.Value().HasAttribute())
        break; // some empty label left in the end
      // some selector fails, try to use rest selectors, myNBLevel becomes negative: unused
      if (myNBLevel.size() > list().size()) {
        std::list<int>::iterator aListIter = myNBLevel.begin();
        for(int a = 0; a < list().size(); a++)
          aListIter++;
        *aListIter = -*aListIter;
        list().push_back(NULL);
      }
    }
  }
  // restore context reference if exists
  static TDF_LabelList anEmptyRefList;
  restoreBaseArray(anEmptyRefList, myContext);

  return myNBLevel.size() == list().size() && !myNBLevel.empty();
}

TDF_Label Selector_FilterByNeighbors::restoreByName(std::string theName,
  const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator)
{
  myShapeType = theShapeType;
  TDF_Label aContext;
  std::string aLastLevel; // last level string (after '(' )  to check the context name in the end
  for (size_t aStart = 0; aStart != std::string::npos;
    aStart = theName.find('(', aStart + 1)) {
    size_t anEndPos = theName.find(')', aStart + 1);
    if (anEndPos != std::string::npos) {
      std::string aSubStr = theName.substr(aStart + 1, anEndPos - aStart - 1);
      TDF_Label aSubContext;
      Selector_Algo* aSubSel =
        Selector_Algo::restoreByName(newSubLabel(), baseDocument(), aSubStr, myShapeType,
          geometricalNaming(), theNameGenerator, aSubContext);
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
      //if (!aContext.IsNull()) // for filters by neighbor the latest context shape is vital
      //  aContext = theNameGenerator->newestContext(aContext);

      // searching for the level index
      std::string aLevel;
      for (anEndPos++; anEndPos != std::string::npos &&
        theName[anEndPos] != '(' && theName[anEndPos] != 0;
        anEndPos++) {
        aLevel += theName[anEndPos];
      }
      aLastLevel = aLevel;
      if (aLevel.empty() || aLevel[0] == '_')
        myNBLevel.push_back(1); // by default it is 1
      else {
        int aNum = atoi(aLevel.c_str());
        if (aNum > 0)
          myNBLevel.push_back(aNum);
        else
          return TDF_Label(); // invalid number
      }
    } else
      return TDF_Label(); // invalid parentheses
  }
  if (!aLastLevel.empty()) { // get the context
    size_t aLinePos = aLastLevel.find("_");
    if (aLinePos != std::string::npos) {
      std::string aContextName = aLastLevel.substr(aLinePos + 1);
      if (!aContextName.empty()) {
        TDF_Label aThisContext, aValue;
        if (theNameGenerator->restoreContext(aContextName, aThisContext, aValue)) {
          if (!aThisContext.IsNull())
            aContext = aThisContext;
        }
      }
    }
  }
  myContext = aContext;
  return aContext;
}

bool Selector_FilterByNeighbors::solve(const TopoDS_Shape& theContext)
{
  TopoDS_Shape aResult;

  std::list<std::pair<TopoDS_Shape, int> > aNBs; /// neighbor sub-shape -> level of neighborhood
  std::list<int>::iterator aLevel = myNBLevel.begin();
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++, aLevel++) {
    if (*aLevel < 0)
      continue; // skip because sub-selector is not good
    if ((*aSubSel)->solve(theContext)) {
      aNBs.push_back(std::pair<TopoDS_Shape, int>((*aSubSel)->value(), *aLevel));
    }
  }
  if (!aNBs.empty()) {
    aResult = findNeighbor(theContext, aNBs, geometricalNaming());
    if (!aResult.IsNull()) {
      Selector_Algo::store(aResult);
      return true;
    }
  }
  return false;
}

std::string Selector_FilterByNeighbors::name(Selector_NameGenerator* theNameGenerator)
{
  // (nb1)level_if_more_than_1(nb2)level_if_more_than_1(nb3)level_if_more_than_1
  bool aThisContextNameNeeded = !myContext.IsNull();
  std::string aContextName;
  if (aThisContextNameNeeded)
    aContextName = theNameGenerator->contextName(myContext);
  std::string aResult;
  std::list<int>::iterator aLevel = myNBLevel.begin();
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++, aLevel++) {
    if (!*aSubSel)
      continue;
    std::string aSubName = (*aSubSel)->name(theNameGenerator);
    aResult += "(" + aSubName + ")";
    if (*aLevel > 1) {
      std::ostringstream aLevelStr;
      aLevelStr<<*aLevel;
      aResult += aLevelStr.str();
    }
    // sub-name already contains the needed context name, so, here it is not needed
    if (aThisContextNameNeeded && (
         aSubName.find(aContextName) == 0 || aSubName.substr(1).find(aContextName) == 0))
      aThisContextNameNeeded = false;
  }
  if (aThisContextNameNeeded) {
    aResult = aResult + "_" + aContextName;
  }
  return aResult;
}
