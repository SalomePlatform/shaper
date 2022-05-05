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

#include <Selector_Modify.h>

#include <Selector_NameGenerator.h>
#include <Selector_NExplode.h>

#include <Locale_Convert.h>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_ChildIterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>

Selector_Modify::Selector_Modify() : Selector_Algo()
{
  myWeakIndex = -1; // no index by default
  myRecomputeWeakIndex = false;
}

// adds to theResult all labels that contain initial shapes for theValue located in theFinal
static void findBases(TDF_Label theAccess, Handle(TNaming_NamedShape) theFinal,
  const TopoDS_Shape& theValue,
  bool aMustBeAtFinal, const TDF_Label& theAdditionalDocument, TDF_LabelList& theResult)
{
  bool aFoundAnyShape = false;
  if (TNaming_Tool::HasLabel(theAccess, theValue)) {
    TNaming_SameShapeIterator aLabIter(theValue, theAccess);
    for(; aLabIter.More(); aLabIter.Next()) {
      Handle(TNaming_NamedShape) aNS;
      if (aLabIter.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
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
          aFoundAnyShape = true;
        }
        if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
          for(TNaming_Iterator aThisIter(aNS); aThisIter.More(); aThisIter.Next()) {
            if (aThisIter.NewShape().IsSame(theValue)) {
              // continue recursively, null NS means that any NS are ok
              findBases(theAccess, theFinal, aThisIter.OldShape(),
                false, theAdditionalDocument, theResult);
              aFoundAnyShape = true;
            }
          }
        }
      }
    }
  }
  if (!aFoundAnyShape && !theAdditionalDocument.IsNull()) { // try to find in additional document
    static TDF_Label anEmpty;
    if (TNaming_Tool::HasLabel(theAdditionalDocument, theValue))
      findBases(theAdditionalDocument, Handle(TNaming_NamedShape)(), theValue,
        false, anEmpty, theResult);
  }
}

/// Returns in theResults all shapes with history started in theBase and ended in theFinal
static void findFinals(const TDF_Label& anAccess, const TopoDS_Shape& theBase,
  const TDF_Label& theFinal,
  const TDF_Label& theAdditionalDoc, TopTools_MapOfShape& thePass, TopTools_MapOfShape& theResults)
{
  if (TNaming_Tool::HasLabel(anAccess, theBase)) {
    for(TNaming_NewShapeIterator aBaseIter(theBase, anAccess); aBaseIter.More(); aBaseIter.Next())
    {
      TNaming_Evolution anEvolution = aBaseIter.NamedShape()->Evolution();
      if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
        if (aBaseIter.NamedShape()->Label().IsEqual(theFinal)) {
          theResults.Add(aBaseIter.Shape());
        } else {
          if (thePass.Add(aBaseIter.Shape()))
            findFinals(
              anAccess, aBaseIter.Shape(), theFinal, theAdditionalDoc, thePass, theResults);
        }
      }
    }
  }
  if (!theAdditionalDoc.IsNull()) { // search additionally by the additional access label
    static TDF_Label anEmpty;
    TopTools_MapOfShape aPass;
    findFinals(theAdditionalDoc, theBase, theFinal, anEmpty, aPass, theResults);
  }
}

void Selector_Modify::findModificationResult(TopoDS_ListOfShape& theCommon) {
  for(TDF_LabelList::Iterator aBase(myBases); aBase.More(); aBase.Next()) {
    TDF_Label anAdditionalDoc; // this document if base is started in extra document
    if (aBase.Value().Root() != label().Root()) {
      anAdditionalDoc = label();
    }
    TopTools_MapOfShape aFinals;
    TopTools_MapOfShape aPass;
    for(TNaming_Iterator aBaseShape(aBase.Value()); aBaseShape.More(); aBaseShape.Next()) {
      findFinals(aBase.Value(), aBaseShape.NewShape(), myFinal, anAdditionalDoc, aPass, aFinals);
    }
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

bool Selector_Modify::select(NCollection_List<Handle(TNaming_NamedShape)>& theModifList,
  const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  if (theModifList.Extent() > 1) { // searching for the best modification result: by context
    bool isFound = false;
    Handle(TNaming_NamedShape) aCandidate;
    NCollection_List<Handle(TNaming_NamedShape)>::Iterator aModIter(theModifList);
    for (; aModIter.More() && !isFound; aModIter.Next()) {
      aCandidate = aModIter.Value();
      TDF_Label aFatherLab = aCandidate->Label().Father();
      Handle(TNaming_NamedShape) aFatherNS;
      if (aFatherLab.FindAttribute(TNaming_NamedShape::GetID(), aFatherNS)) {
        for (TNaming_Iterator anIter(aFatherNS);
             anIter.More() && !isFound; anIter.Next()) {
          if (theContext.IsSame(anIter.NewShape())) { // found the best modification
            isFound = true;
          }
        }
      }
    }
    // take the best candidate, or the last in the iteration
    theModifList.Clear();
    theModifList.Append(aCandidate);
  }

  if (!theModifList.IsEmpty()) {
    // searching for all the base shapes of this modification
    findBases(label(), theModifList.First(), theValue, true, baseDocument(), myBases);
    if (!myBases.IsEmpty()) {
      myFinal = theModifList.First()->Label();
      TopoDS_ListOfShape aCommon;
      findModificationResult(aCommon);
      // trying to search by neighbors
      if (aCommon.Extent() > 1) { // more complicated selection
        if (alwaysGeometricalNaming()) {
          TopoDS_ListOfShape::Iterator aCommonIter(aCommon);
          TopoDS_Shape aFirst = aCommonIter.Value();
          for (aCommonIter.Next(); aCommonIter.More(); aCommonIter.Next()) {
            if (!sameGeometry(aFirst, aCommonIter.Value()))
              break;
          }
          if (!aCommonIter.More()) { // all geometry is same, result is a compound
            return true;
          }
        }
      } else if (aCommon.Extent() == 1) {
        return true; // simple modification
      }
      if (useNeighbors()) { // optimization: for the current moment only in one case this method is
                            //  called where this is not needed if neighbors option is disabled
        // weak naming between the common results
        Selector_NExplode aNexp(aCommon);
        myWeakIndex = aNexp.index(theValue);
      } else
        myWeakIndex = 0;
      return myWeakIndex != -1;
    }
    // weak naming case
    TopoDS_ListOfShape aCommon;
    myFinal = theModifList.First()->Label();
    Handle(TNaming_NamedShape) aNS;
    if (myFinal.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      for(TNaming_Iterator aFinalIter(aNS); aFinalIter.More(); aFinalIter.Next()) {
        const TopoDS_Shape& aNewShape = aFinalIter.NewShape();
        if (!aNewShape.IsNull())
          aCommon.Append(aNewShape);
      }
    }
    Selector_NExplode aNexp(aCommon);
    myWeakIndex = aNexp.index(theValue);
    return myWeakIndex != -1;
  }
  return false;
}

void Selector_Modify::store()
{
  storeType(Selector_Algo::SELTYPE_MODIFICATION);
  storeBaseArray(myBases, myFinal);
  if (myWeakIndex != -1) {
    TDataStd_Integer::Set(label(), weakID(), myWeakIndex);
  }
}

bool Selector_Modify::restore()
{
  if (restoreBaseArray(myBases, myFinal)) {
    Handle(TDataStd_Integer) aWeakInt;
    if (label().FindAttribute(weakID(), aWeakInt)) {
      myWeakIndex = aWeakInt->Get();
    }
    return true;
  }
  return false;
}

TDF_Label Selector_Modify::restoreByName(std::wstring theName,
  const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator)
{
  typedef NCollection_DataMap<TopoDS_Shape, bool, TopTools_ShapeMapHasher> MapOfCompsolids;
  MapOfCompsolids aWrongSubsCompsolids;

  TDF_Label aContext;
  for(size_t anEnd, aStart = 0; aStart != std::wstring::npos; aStart = anEnd) {
    if (aStart != 0)
      aStart++;
    anEnd = theName.find(L'&', aStart);
    std::wstring aSubStr =
      theName.substr(aStart, anEnd == std::wstring::npos ? anEnd : anEnd - aStart);
    size_t aFoundOldWeak = aSubStr.find(oldWeakNameID());
    size_t aFoundNewWeak = aFoundOldWeak != std::wstring::npos ?
                           aSubStr.find(weakNameID()) :
                           aFoundOldWeak;
    if (aFoundOldWeak == 0 || aFoundNewWeak == 0) { // weak name identifier
      std::wstring aWeakIndex = aSubStr.substr(aFoundOldWeak + oldWeakNameID().size());
      myWeakIndex = atoi(Locale::Convert::toString(aWeakIndex).c_str());
      myRecomputeWeakIndex = aFoundOldWeak == 0;
      continue;
    }
    TDF_Label aSubContext, aValue;
    if (!theNameGenerator->restoreContext(aSubStr, aSubContext, aValue))
      return TDF_Label(); // can not restore
    if (aSubContext.IsNull() || (aValue.IsNull() && theShapeType <= TopAbs_SHELL))
      return TDF_Label(); // can not restore
    if (myFinal.IsNull()) {
      myFinal = aValue;
      aContext = aSubContext;
    } else {
      // This could be a solid in a compsolid, which was not modified by the previous operation,
      // however, the selected subshape is stored on its sub-label by mistake. Thus, wait until
      // the end of processing to check whether the subshape is found in another solid.
      TDF_Label aParent = aSubContext.Father().Father();
      Handle(TNaming_NamedShape) aNS;
      if (aParent.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
        TopoDS_Shape aShape = aNS->Get();
        if (aShape.ShapeType() == TopAbs_COMPSOLID) {
          if (aWrongSubsCompsolids.IsBound(aShape)) {
            if (!aValue.IsNull())
              aWrongSubsCompsolids.Bind(aShape, true);
          } else
            aWrongSubsCompsolids.Bind(aShape, !aValue.IsNull());
        }
      } else if (aValue.IsNull())
        return TDF_Label();

      if (!aValue.IsNull())
        myBases.Append(aValue);
    }
  }

  // check all compsolids are processed and names are resolved
  for (MapOfCompsolids::Iterator anIt(aWrongSubsCompsolids); anIt.More(); anIt.Next())
    if (!anIt.Value())
      return TDF_Label();

  return aContext;
}

bool Selector_Modify::solve(const TopoDS_Shape& theContext)
{
  TopoDS_Shape aResult;
  if (myBases.IsEmpty() && myWeakIndex > 0) { // weak name by the final shapes index
    TopoDS_ListOfShape aCommon;
    Handle(TNaming_NamedShape) aNS;
    if (myFinal.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      for(TNaming_Iterator aFinalIter(aNS); aFinalIter.More(); aFinalIter.Next()) {
        const TopoDS_Shape& aNewShape = aFinalIter.NewShape();
        if (!aNewShape.IsNull())
          aCommon.Append(aNewShape);
      }
    }
    Selector_NExplode aNexp(aCommon, myRecomputeWeakIndex);
    aResult = aNexp.shape(myWeakIndex);
    myRecomputeWeakIndex = false;
  } else { // standard case
    TopoDS_ListOfShape aFinalsCommon; // final shapes presented in all results from bases
    findModificationResult(aFinalsCommon);
    if (aFinalsCommon.Extent() == 1) { // result is valid: found only one shape
      aResult = aFinalsCommon.First();
      findNewVersion(theContext, aResult);
    } else if (aFinalsCommon.Extent() > 1 && myWeakIndex > 0) {
      Selector_NExplode aNExp(aFinalsCommon, myRecomputeWeakIndex);
      aResult = aNExp.shape(myWeakIndex);
      myRecomputeWeakIndex = false;
      findNewVersion(theContext, aResult);
    } else if (aFinalsCommon.Extent() > 1 && geometricalNaming()) {// if same geometry - compound
      TopoDS_ListOfShape::Iterator aCommonIter(aFinalsCommon);
      TopoDS_Shape aFirst = aCommonIter.Value();
      for(aCommonIter.Next(); aCommonIter.More(); aCommonIter.Next()) {
        if (!sameGeometry(aFirst, aCommonIter.Value()))
          break;
      }
      if (!aCommonIter.More()) { // all geometry is same, create a result compound
        TopoDS_Builder aBuilder;
        TopoDS_Compound aCompound;
        aBuilder.MakeCompound(aCompound);
        for(aCommonIter.Initialize(aFinalsCommon); aCommonIter.More(); aCommonIter.Next()) {
          TopoDS_Shape aSub = aCommonIter.Value();
          findNewVersion(theContext, aSub);
          aBuilder.Add(aCompound, aSub);
        }
        aResult = aCompound;
      }

    }
  }

  if (!aResult.IsNull()) {
    Selector_Algo::store(aResult);
    return true;
  }
  return false;
}

std::wstring Selector_Modify::name(Selector_NameGenerator* theNameGenerator)
{
  // final&base1&base2 +optionally: [weak_name_1]
  std::wstring aResult;
  Handle(TDataStd_Name) aName;
  if (!myFinal.FindAttribute(TDataStd_Name::GetID(), aName))
    return L"";
  aResult += theNameGenerator->contextName(myFinal) + L"/";
  aResult += Locale::Convert::toWString(aName->Get().ToExtString());
  for(TDF_LabelList::iterator aBase = myBases.begin(); aBase != myBases.end(); aBase++) {
    if (!aBase->FindAttribute(TDataStd_Name::GetID(), aName))
      return L"";
    aResult += L"&";
    aResult += theNameGenerator->contextName(*aBase) + L"/";
    aResult += Locale::Convert::toWString(aName->Get().ToExtString());
  }
  if (myWeakIndex != -1) {
    std::wostringstream aWeakStr;
    aWeakStr<<L"&"<<weakNameID()<<myWeakIndex;
    aResult += aWeakStr.str();
  }
  return aResult;
}
