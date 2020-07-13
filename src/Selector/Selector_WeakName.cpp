// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <Selector_WeakName.h>

#include <Selector_NameGenerator.h>
#include <Selector_NExplode.h>

#include <Locale_Convert.h>

#include <TNaming_Tool.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TDataStd_Integer.hxx>

Selector_WeakName::Selector_WeakName() : Selector_Algo(), myRecomputeWeakIndex(false)
{
}

bool Selector_WeakName::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  myShapeType = theValue.ShapeType();
  Selector_NExplode aNexp(theContext, myShapeType);
  myWeakIndex = aNexp.index(theValue);
  if (myWeakIndex != -1) {
    // searching for context shape label to store in myFinal
    if (TNaming_Tool::HasLabel(label(), theContext)) {
      for(TNaming_SameShapeIterator aShapes(theContext, label()); aShapes.More(); aShapes.Next())
      {
        Handle(TNaming_NamedShape) aNS;
        if (aShapes.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          TNaming_Evolution anEvolution = aNS->Evolution();
          if (anEvolution == TNaming_PRIMITIVE || anEvolution == TNaming_GENERATED ||
            anEvolution == TNaming_MODIFY) {
            // check this is a new shape
            for(TNaming_Iterator aNSIter(aNS); aNSIter.More(); aNSIter.Next()) {
              if (aNSIter.NewShape().IsSame(theContext)) {
                myContext = aNS->Label();
                break;
              }
            }
          }
        }
      }
    }
    return true;
  }
  return false;
}

void Selector_WeakName::store()
{
  static const TDF_LabelList anEmptyRefList;
  storeType(Selector_Algo::SELTYPE_WEAK_NAMING);
  storeBaseArray(anEmptyRefList, myContext);
  TDataStd_Integer::Set(label(), weakID(), myWeakIndex);
  TDataStd_Integer::Set(label(), shapeTypeID(), (int)myShapeType);
}

bool Selector_WeakName::restore()
{
  Handle(TDataStd_Integer) aWeakInt;
  if (!label().FindAttribute(weakID(), aWeakInt))
    return false;
  myWeakIndex = aWeakInt->Get();
  Handle(TDataStd_Integer) aShapeTypeAttr;
  if (!label().FindAttribute(shapeTypeID(), aShapeTypeAttr))
    return false;
  myShapeType = TopAbs_ShapeEnum(aShapeTypeAttr->Get());
  static TDF_LabelList anEmptyRefList;
  return restoreBaseArray(anEmptyRefList, myContext);
}

TDF_Label Selector_WeakName::restoreByName(std::wstring theName,
  const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator)
{
  size_t aFoundWeak = theName.find(oldPureWeakNameID());
  std::wstring aWeakIndex = theName.substr(aFoundWeak + oldPureWeakNameID().size());
  std::size_t aContextPosition = aWeakIndex.find(L"_");
  myWeakIndex = atoi(Locale::Convert::toString(aWeakIndex).c_str());
  myRecomputeWeakIndex = aFoundWeak == 0;
  myShapeType = theShapeType;
  TDF_Label aContext;
  if (aContextPosition != std::wstring::npos) { // context is also defined
    std::wstring aContextName = aWeakIndex.substr(aContextPosition + 1);
    if (theNameGenerator->restoreContext(aContextName, aContext, myContext)) {
      if (myContext.IsNull())
        aContext.Nullify();
    }
  }
  return aContext;
}

bool Selector_WeakName::solve(const TopoDS_Shape& theContext)
{

  TopoDS_Shape aContext;
  if (myContext.IsNull()) {
    aContext = theContext;
  } else {
    Handle(TNaming_NamedShape) aNS;
    if (myContext.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      aContext = aNS->Get();
    }
  }
  if (!aContext.IsNull()) {
    Selector_NExplode aNexp(aContext, myShapeType, myRecomputeWeakIndex);
    TopoDS_Shape aResult = aNexp.shape(myWeakIndex);
    myRecomputeWeakIndex = false;
    if (!aResult.IsNull()) {
      Selector_Algo::store(aResult);
      return true;
    }
  }
  return false;
}

std::wstring Selector_WeakName::name(Selector_NameGenerator* theNameGenerator)
{
  // _weak_naming_1_Context
  std::wostringstream aWeakStr;
  aWeakStr<<pureWeakNameID()<<myWeakIndex;
  std::wstring aResult = aWeakStr.str();
  if (!myContext.IsNull())
    aResult += L"_" + theNameGenerator->contextName(myContext);
  return aResult;
}
