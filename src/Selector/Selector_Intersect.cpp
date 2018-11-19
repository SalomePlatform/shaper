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

#include <Selector_Intersect.h>

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

Selector_Intersect::Selector_Intersect() : Selector_AlgoWithSubs()
{
  myWeakIndex = -1; // no index by default
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

bool Selector_Intersect::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  myShapeType = theValue.ShapeType();
  TopAbs_ShapeEnum aSelectionType = myShapeType;
  // try to find the shape of the higher level type in the context shape
  bool aFacesTried = false; // for identification of vertices, faces are tried, then edges
  TopoDS_ListOfShape aLastCommon; // store not good commons, but which may be used for weak naming
  TopoDS_ListOfShape aLastIntersectors;
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
      if (aSelectionType == TopAbs_EDGE &&
        BRep_Tool::Degenerated(TopoDS::Edge(aSelExp.Current())))
        continue;
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
      TopoDS_ListOfShape::Iterator anInt(anIntList);
      for (; anInt.More(); anInt.Next()) {
        Selector_Algo* aSubAlgo = Selector_Algo::select(theContext, anInt.Value(),
          newSubLabel(), baseDocument(), geometricalNaming(), useNeighbors(), false);
        if (!append(aSubAlgo))
          break; // if some selector is failed, stop and search another solution
      }
      if (!anInt.More()) { // all intersectors were correctly named
        return true;
      }
    } else if (aCommon.Extent() > 1 && aLastCommon.IsEmpty())  {
      aLastCommon = aCommon;
      aLastIntersectors = anIntList;
    }
  }
  if (aLastCommon.Extent() > 1) {
    if (alwaysGeometricalNaming()) {
      TopoDS_ListOfShape::Iterator aCommonIter(aLastCommon);
      TopoDS_Shape aFirst = aCommonIter.Value();
      for(aCommonIter.Next(); aCommonIter.More(); aCommonIter.Next()) {
        if (!sameGeometry(aFirst, aCommonIter.Value()))
          break;
      }
      if (!aCommonIter.More()) { // all geometry is same, result is a compound
        return true;
      }
    }
    // weak naming to distinguish commons coming from intersection
    Selector_NExplode aNexp(aLastCommon);
    myWeakIndex = aNexp.index(theValue);
    if (myWeakIndex != -1) {
      // name the intersectors
      TopoDS_ListOfShape::Iterator anInt(aLastIntersectors);
      for (; anInt.More(); anInt.Next()) {
        Selector_Algo* aSubAlgo = Selector_Algo::select(theContext, anInt.Value(),
          newSubLabel(), baseDocument(), geometricalNaming(), useNeighbors(), useIntersections());
        if (!append(aSubAlgo))
          break; // if some selector is failed, stop and search another solution
      }
      if (!anInt.More()) { // all intersectors were correctly named
        return true;
      }
    }
  }
  return false; // solution does not found
}

void Selector_Intersect::store()
{
  storeType(Selector_Algo::SELTYPE_INTERSECT);
  // store all sub-selectors
  TDataStd_Integer::Set(label(), shapeTypeID(), (int)myShapeType);
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    (*aSubSel)->store();
  }
  TDataStd_Integer::Set(label(), shapeTypeID(), (int)myShapeType);
  if (myWeakIndex != -1) {
    TDataStd_Integer::Set(label(), weakID(), myWeakIndex);
  }
}

bool Selector_Intersect::restore()
{
  Handle(TDataStd_Integer) aShapeTypeAttr;
  if (!label().FindAttribute(shapeTypeID(), aShapeTypeAttr))
    return false;
  myShapeType = TopAbs_ShapeEnum(aShapeTypeAttr->Get());
  // restore sub-selectors
  bool aSubResult = true;
  for(TDF_ChildIterator aSub(label(), false); aSub.More(); aSub.Next()) {
    Selector_Algo* aSubSel = restoreByLab(aSub.Value());
    if (!append(aSubSel, false)) {
      break; // some empty label left in the end
    }
  }
  Handle(TDataStd_Integer) aWeakInt;
  if (label().FindAttribute(weakID(), aWeakInt)) {
    myWeakIndex = aWeakInt->Get();
  }
  return true;
}

TDF_Label Selector_Intersect::restoreByName(std::string theName,
  const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator)
{
  myShapeType = theShapeType;
  TDF_Label aContext;
  for(size_t aStart = 0; aStart != std::string::npos; aStart = theName.find('[', aStart + 1)) {
    size_t anEndPos = theName.find(']', aStart + 1);
    if (anEndPos != std::string::npos) {
      std::string aSubStr = theName.substr(aStart + 1, anEndPos - aStart - 1);
      if (aSubStr.find(weakNameID()) == 0) { // weak name identifier
        std::string aWeakIndex = aSubStr.substr(weakNameID().size());
        myWeakIndex = atoi(aWeakIndex.c_str());
        continue;
      }
      TopAbs_ShapeEnum aSubShapeType = TopAbs_FACE;
      if (anEndPos != std::string::npos && anEndPos + 1 < theName.size()) {
        char aShapeChar = theName[anEndPos + 1];
        if (theName[anEndPos + 1] != '[') {
          switch(aShapeChar) {
          case 'c': aSubShapeType = TopAbs_COMPOUND; break;
          case 'o': aSubShapeType = TopAbs_COMPSOLID; break;
          case 's': aSubShapeType = TopAbs_SOLID; break;
          case 'h': aSubShapeType = TopAbs_SHELL; break;
          case 'w': aSubShapeType = TopAbs_WIRE; break;
          case 'e': aSubShapeType = TopAbs_EDGE; break;
          case 'v': aSubShapeType = TopAbs_VERTEX; break;
          default:;
          }
        }
      }
      TDF_Label aSubContext;
      Selector_Algo* aSubSel =
        Selector_Algo::restoreByName(
          label(), baseDocument(), aSubStr, aSubShapeType, theNameGenerator, aSubContext);
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
  }
  return aContext;
}

bool Selector_Intersect::solve(const TopoDS_Shape& theContext)
{
  TopoDS_Shape aResult;
  TopoDS_ListOfShape aSubSelectorShapes;
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    if (!(*aSubSel)->solve(theContext)) {
      return false;
    }
    aSubSelectorShapes.Append((*aSubSel)->value());
  }
  TopoDS_ListOfShape aCommon; // common sub shapes in each sub-selector (a result)
  commonShapes(aSubSelectorShapes, myShapeType, aCommon);
  if (aCommon.Extent() != 1) {
    if (myWeakIndex != -1) {
      Selector_NExplode aNexp(aCommon);
      aResult = aNexp.shape(myWeakIndex);
    } else if (geometricalNaming() && aCommon.Extent() > 1) {
      // check results are on the same geometry, create compound
      TopoDS_ListOfShape::Iterator aCommonIter(aCommon);
      TopoDS_Shape aFirst = aCommonIter.Value();
      for(aCommonIter.Next(); aCommonIter.More(); aCommonIter.Next()) {
        if (!sameGeometry(aFirst, aCommonIter.Value()))
          break;
      }
      if (!aCommonIter.More()) { // all geometry is same, create a result compound
        TopoDS_Builder aBuilder;
        TopoDS_Compound aCompound;
        aBuilder.MakeCompound(aCompound);
        for(aCommonIter.Initialize(aCommon); aCommonIter.More(); aCommonIter.Next()) {
          aBuilder.Add(aCompound, aCommonIter.Value());
        }
        aResult = aCompound;
      }
    } else {
      return false;
    }
  } else {
    aResult = aCommon.First();
  }
  if (!aResult.IsNull()) {
    Selector_Algo::store(aResult);
    return true;
  }
  return false;
}

std::string Selector_Intersect::name(Selector_NameGenerator* theNameGenerator)
{
  std::string aResult;
  // add names of sub-components one by one in "[]" +optionally [weak_name_1]
  std::list<Selector_Algo*>::const_iterator aSubSel = list().cbegin();
  for(; aSubSel != list().cend(); aSubSel++) {
    aResult += '[';
    aResult += (*aSubSel)->name(theNameGenerator);
    aResult += ']';
    TopoDS_Shape aSubVal = (*aSubSel)->value();
    if (!aSubVal.IsNull()) {
      TopAbs_ShapeEnum aSubType = aSubVal.ShapeType();
      if (aSubType != TopAbs_FACE) { // in case the sub shape type must be stored
        switch(aSubType) {
        case TopAbs_COMPOUND: aResult += "c"; break;
        case TopAbs_COMPSOLID: aResult += "o"; break;
        case TopAbs_SOLID: aResult += "s"; break;
        case TopAbs_SHELL: aResult += "h"; break;
        case TopAbs_WIRE: aResult += "w"; break;
        case TopAbs_EDGE: aResult += "e"; break;
        case TopAbs_VERTEX: aResult += "v"; break;
        default:;
        }
      }
    }
  }
  if (myWeakIndex != -1) {
    std::ostringstream aWeakStr;
    aWeakStr<<"["<<weakNameID()<<myWeakIndex<<"]";
    aResult += aWeakStr.str();
  }
  return aResult;
}
