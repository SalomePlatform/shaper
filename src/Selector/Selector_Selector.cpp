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

#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>

#include <TDataStd_Integer.hxx>
#include <TDataStd_ReferenceArray.hxx>

#include <list>

/// type of the selection, integerm keeps the Selector_Type value
static const Standard_GUID kSEL_TYPE("db174d59-c2e3-4a90-955e-55544df090d6");
//  reference attribute that contains the reference to labels where the "from" or "base" shapes
// of selection are located
static const Standard_GUID kBASE_LIST("7c515b1a-9549-493d-9946-a4933a22f45f");

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
      TNaming_NewShapeIterator aNSIter(anOldShape, theFinal->Label());
      for(; aNSIter.More(); aNSIter.Next()) {
        TNaming_Evolution anEvolution = aNSIter.NamedShape()->Evolution();
        if (anEvolution == TNaming_PRIMITIVE) { // found a good candidate, a base shape
          // check that this is not in the results already
          const TDF_Label aResult = aNSIter.NamedShape()->Label();
          TDF_LabelList::Iterator aResIter(theResult);
          for(; aResIter.More(); aResIter.Next()) {
            if (aResIter.Value().IsEqual(aResult))
              break;
          }
          if (!aResIter.More()) // not found, so add this new
            theResult.Append(aResult);
        } else if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
          // continue recursively
          findBases(aNSIter.NamedShape(), anOldShape, theResult);
        }
      }
    }
  }
}

bool Selector_Selector::Select(const TopoDS_Shape theContext, const TopoDS_Shape theValue)
{
  if (theValue.IsNull() || theContext.IsNull())
    return false;
  // check the value shape can be named as it is, or it is needed to construct it from the
  // higher level shapes (like a box vertex by faces that form this vertex)
  if (!TNaming_Tool::HasLabel(myLab, theValue)) {
    TopAbs_ShapeEnum aSelectionType = theValue.ShapeType();
    if (aSelectionType == TopAbs_COMPOUND || aSelectionType == TopAbs_COMPSOLID ||
        aSelectionType == TopAbs_SOLID) { // iterate all sub-shapes and select them on sublabels
      std::list<Selector_Selector> aSubSelList;
      for(TopoDS_Iterator aSubIter(theValue); aSubIter.More(); aSubIter.Next()) {
        aSubSelList.push_back(Selector_Selector(myLab.FindChild(aSubSelList.size() + 1)));
        if (!aSubSelList.back().Select(theContext, aSubIter.Value())) {
          return false; // if some selector is failed, everything is failed
        }
      }
      myType = SELTYPE_CONTAINER;
      myShapeType = aSelectionType;
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

      // name the intersectors

    }
  }
  // searching for the base shapes of the value
  Handle(TNaming_NamedShape) aPrimitiveNS;
  NCollection_List<Handle(TNaming_NamedShape)> aModifList;
  for(TNaming_NewShapeIterator aNewShapes(theValue, myLab); aNewShapes.More(); aNewShapes.Next())
  {
    TNaming_Evolution anEvolution = aNewShapes.NamedShape()->Evolution();
    if (anEvolution == TNaming_PRIMITIVE) { // the value shape is declared as primitive => PRIMITIVE
      aPrimitiveNS = aNewShapes.NamedShape();
      break;
    } else if (anEvolution == TNaming_GENERATED || anEvolution == TNaming_MODIFY) {
      aModifList.Append(aNewShapes.NamedShape());
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

void Selector_Selector::Store()
{
  TDataStd_Integer::Set(myLab, kSEL_TYPE, (int)myType);
  switch(myType) {
  case SELTYPE_CONTAINER: {
    /// TODO
    break;
  }
  case SELTYPE_INTERSECT: {
    /// TODO
    break;
  }
  case SELTYPE_PRIMITIVE: {
    Handle(TDataStd_ReferenceArray) anArray = TDataStd_ReferenceArray::Set(myLab, 0, 0);
    anArray->SetValue(0, myBases.First());
    break;
  }
  case SELTYPE_MODIFICATION: {
    Handle(TDataStd_ReferenceArray) anArray =
      TDataStd_ReferenceArray::Set(myLab, 0, myBases.Extent() - 1);
    TDF_LabelList::Iterator aBIter(myBases);
    for(int anIndex = 0; aBIter.More(); aBIter.Next(), anIndex++) {
      anArray->SetValue(anIndex, aBIter.Value());
    }
    break;
  }
  default: { // unknown case
    break;
  }
  }
}
