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

#include <Selector_Algo.h>

#include <Selector_Primitive.h>
#include <Selector_Intersect.h>
#include <Selector_Modify.h>
#include <Selector_Container.h>
#include <Selector_FilterByNeighbors.h>
#include <Selector_WeakName.h>

#include <TDF_Tool.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_Integer.hxx>

/// type of the selection, integer keeps the Selector_Type value
static const Standard_GUID kSEL_TYPE("db174d59-c2e3-4a90-955e-55544df090d6");
// geometrical naming indicator
static const Standard_GUID kGEOMETRICAL_NAMING("a5322d02-50fb-43ed-9255-75c7b93f6657");


//  reference attribute that contains the reference to labels where the "from" or "base" shapes
// of selection are located
static const Standard_GUID kBASE_ARRAY("7c515b1a-9549-493d-9946-a4933a22f45f");
// if the base array contains reference to the root label, this means that it refers to an
// external document and this list contains a tag in the document
static const Standard_GUID kBASE_LIST("7c515b1a-9549-493d-9946-a4933a22f45a");

Selector_Algo::Selector_Algo()
{
  myGeometricalNaming = false; // default values
  myAlwaysGeometricalNaming = false;
}

#define SET_ALGO_FLAGS(algo) \
  algo->myLab = theAccess; \
  algo->myBaseDocumentLab = theBaseDocument; \
  algo->myGeometricalNaming = theGeometricalNaming; \
  algo->myUseNeighbors = theUseNeighbors; \
  algo->myUseIntersections = theUseIntersections; \
  algo->myAlwaysGeometricalNaming = theAlwaysGeometricalNaming;

Selector_Algo* Selector_Algo::select(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
  const TDF_Label theAccess, const TDF_Label theBaseDocument,
  const bool theGeometricalNaming, const bool theUseNeighbors, const bool theUseIntersections,
  const bool theAlwaysGeometricalNaming)
{
  Selector_Algo* aResult = NULL;
  if (theValue.IsNull() || theContext.IsNull())
    return aResult;

  // check the value shape can be named as it is, or it is needed to construct it from the
  // higher level shapes (like a box vertex by faces that form this vertex)
  bool aIsFound = TNaming_Tool::HasLabel(theAccess, theValue);
  if (aIsFound) { // additional check for selection and delete evolution only: also could not use
    aIsFound = false;
    for(TNaming_SameShapeIterator aShapes(theValue, theAccess); aShapes.More(); aShapes.Next())
    {
      Handle(TNaming_NamedShape) aNS;
      if (aShapes.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
        if (aNS->Evolution() == TNaming_MODIFY || aNS->Evolution() == TNaming_GENERATED ||
          aNS->Evolution() == TNaming_PRIMITIVE) {
          aIsFound = true;
          break;
        }
      }
    }
  }
  // searching in the base document
  if (!aIsFound && !theBaseDocument.IsNull() && TNaming_Tool::HasLabel(theBaseDocument, theValue))
  {
    TNaming_SameShapeIterator aShapes(theValue, theBaseDocument);
    for(; aShapes.More(); aShapes.Next())
    {
      Handle(TNaming_NamedShape) aNS;
      if (aShapes.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
        if (aNS->Evolution() == TNaming_MODIFY || aNS->Evolution() == TNaming_GENERATED ||
          aNS->Evolution() == TNaming_PRIMITIVE) {
          aIsFound = true;
          break;
        }
      }
    }
  }
  if (!aIsFound) {
    TopAbs_ShapeEnum aSelectionType = theValue.ShapeType();
    if (aSelectionType == TopAbs_COMPOUND || aSelectionType == TopAbs_COMPSOLID ||
      aSelectionType == TopAbs_SHELL || aSelectionType == TopAbs_WIRE)
    {
      Selector_Container* aContainer = new Selector_Container;
      SET_ALGO_FLAGS(aContainer);
      if (aContainer->select(theContext, theValue))
        return aContainer;
      delete aContainer;
      return false;
    }
    Selector_Intersect* anIntersect = new Selector_Intersect;
    SET_ALGO_FLAGS(anIntersect);
    bool aGoodIntersector = anIntersect->select(theContext, theValue);
    // weak intersector is bad for not use neighbors and has lower priority than neighbors
    if (aGoodIntersector && anIntersect->myWeakIndex == -1) {
      return anIntersect;
    }
    if (!theUseNeighbors) {
      delete anIntersect;
      return false;
    }
    // searching by neighbors
    Selector_FilterByNeighbors* aNBs = new Selector_FilterByNeighbors;
    SET_ALGO_FLAGS(aNBs);
    if (aNBs->select(theContext, theValue)) {
      delete anIntersect;
      return aNBs;
    }
    delete aNBs;
    if (aGoodIntersector) { // if neighbors are bad, use intersector with weak index
      return anIntersect;
    }
    delete anIntersect;

    // pure weak naming: there is no sense to use pure weak naming for neighbors selection
    if (theUseNeighbors) {
      Selector_WeakName* aWeak = new Selector_WeakName;
      SET_ALGO_FLAGS(aWeak);
      if (aWeak->select(theContext, theValue)) {
        return aWeak;
      }
      delete aWeak;
    }
    return NULL; // not found value in the tree, not found context shape in the tree also
  }
  // searching for the base shapes of the value
  Handle(TNaming_NamedShape) aPrimitiveNS;
  NCollection_List<Handle(TNaming_NamedShape)> aModifList;
  for(int aUseExternal = 0; aUseExternal < 2; aUseExternal++) {
    TDF_Label aLab = aUseExternal == 0 ? theAccess : theBaseDocument;
    if (aLab.IsNull() || !TNaming_Tool::HasLabel(aLab, theValue))
      continue;
    for(TNaming_SameShapeIterator aShapes(theValue, aLab); aShapes.More(); aShapes.Next())
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
  }

  if (!aPrimitiveNS.IsNull()) {
    Selector_Primitive* aPrimitive = new Selector_Primitive;
    SET_ALGO_FLAGS(aPrimitive);
    aPrimitive->select(aPrimitiveNS->Label());
    return aPrimitive;
  } else {
    Selector_Modify* aModify = new Selector_Modify;
    SET_ALGO_FLAGS(aModify);
    bool aGoodModify = aModify->select(aModifList, theContext, theValue);
    // weak intersector is bad for not use neighbors and has lower priority than neighbors
    if (aGoodModify && aModify->myWeakIndex == -1) {
      return aModify;
    }
    if (!theUseNeighbors) {
      delete aModify;
      return false;
    }
    // searching by neighbors
    Selector_FilterByNeighbors* aNBs = new Selector_FilterByNeighbors;
    SET_ALGO_FLAGS(aNBs);
    if (aNBs->select(theContext, theValue)) {
      delete aModify;
      return aNBs;
    }
    delete aNBs;

    if (aGoodModify) { // if neighbors are bad, use modify algo with weak index
      return aModify;
    }
    delete aModify;
  }

  return NULL; // invalid case
}

Selector_Algo* Selector_Algo::relesectWithAllGeometry(
  Selector_Algo* theOldAlgo, const TopoDS_Shape theContext)
{
  return select(theContext, theOldAlgo->value(),
    theOldAlgo->myLab, theOldAlgo->myBaseDocumentLab, true, true, true, true);
}

void Selector_Algo::storeBaseArray(const TDF_LabelList& theRef, const TDF_Label& theLast)
{
  Handle(TDataStd_ReferenceArray) anArray =
    TDataStd_ReferenceArray::Set(myLab, kBASE_ARRAY, 0, theRef.Extent());
  Handle(TDataStd_ExtStringList) anEntries; // entries of references to external document
  const TDF_Label aThisDocRoot = myLab.Root();
  TDF_LabelList::Iterator aBIter(theRef);
  for(int anIndex = 0; true; aBIter.Next(), anIndex++) {
    const TDF_Label& aLab = aBIter.More() ? aBIter.Value() : theLast;
    // check this is a label of this document
    if (aLab.Root().IsEqual(aThisDocRoot)) {
      anArray->SetValue(anIndex, aLab);
    } else { // store reference to external document as an entry-string
      if (anEntries.IsNull()) {
        anEntries = TDataStd_ExtStringList::Set(myLab, kBASE_LIST);
      }
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aLab, anEntry);
      anEntries->Append(anEntry);
      anArray->SetValue(anIndex, aThisDocRoot); // stored root means it is external reference
    }
    if (!aBIter.More())
      break;
  }
}

bool Selector_Algo::restoreBaseArray(TDF_LabelList& theRef, TDF_Label& theLast)
{
  const TDF_Label aThisDocRoot = myLab.Root();
  Handle(TDataStd_ExtStringList) anEntries; // entries of references to external document
  TDataStd_ListOfExtendedString::Iterator anIter;
  Handle(TDataStd_ReferenceArray) anArray;
  if (myLab.FindAttribute(kBASE_ARRAY, anArray)) {
    int anUpper = anArray->Upper();
    for(int anIndex = anArray->Lower(); anIndex <= anUpper; anIndex++) {
      TDF_Label aLab = anArray->Value(anIndex);
      if (aLab.IsEqual(aThisDocRoot)) { // external document reference
        if (myBaseDocumentLab.IsNull())
          return false;
        if (anEntries.IsNull()) {
          if (!myLab.FindAttribute(kBASE_LIST, anEntries))
            return false;
          anIter.Initialize(anEntries->List());
        }
        if (!anIter.More())
          return false;
        TDF_Tool::Label(myBaseDocumentLab.Data(), anIter.Value(), aLab);
        anIter.Next();
      }
      if (anIndex == anUpper) {
        theLast = aLab;
      } else {
        theRef.Append(aLab);
      }
    }
    return true;
  }
  return false;
}

void Selector_Algo::store(const TopoDS_Shape theShape)
{
  TNaming_Builder aBuilder(myLab);
  aBuilder.Select(theShape, theShape);
}

bool Selector_Algo::sameGeometry(const TopoDS_Shape theShape1, const TopoDS_Shape theShape2) {
  if (!theShape1.IsNull() && !theShape2.IsNull() && theShape1.ShapeType() == theShape2.ShapeType())
  {
    if (theShape1.ShapeType() == TopAbs_FACE) { // check surfaces
      TopLoc_Location aLoc1, aLoc2;
      TopoDS_Face aFace1 = TopoDS::Face(theShape1);
      Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(aFace1, aLoc1);
      TopoDS_Face aFace2 = TopoDS::Face(theShape2);
      Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(aFace2, aLoc2);
      return aSurf1 == aSurf2 && aLoc1.IsEqual(aLoc2);
    } else if (theShape1.ShapeType() == TopAbs_EDGE) { // check curves
      TopLoc_Location aLoc1, aLoc2;
      Standard_Real aFirst, aLast;
      TopoDS_Edge anEdge1 = TopoDS::Edge(theShape1);
      Handle(Geom_Curve) aCurve1 = BRep_Tool::Curve(anEdge1, aLoc1, aFirst, aLast);
      TopoDS_Edge anEdge2 = TopoDS::Edge(theShape2);
      Handle(Geom_Curve) aCurve2 = BRep_Tool::Curve(anEdge2, aLoc2, aFirst, aLast);
      return aCurve1 == aCurve2 && aLoc1.IsEqual(aLoc2);
    }
  }
  return false;
}

TopoDS_Shape Selector_Algo::value()
{
  Handle(TNaming_NamedShape) aNS;
  if (myLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
    return aNS->Get();
  return TopoDS_Shape(); // empty, error shape
}

Selector_Algo* Selector_Algo::restoreByLab(TDF_Label theLab, TDF_Label theBaseDocLab)
{
  Handle(TDataStd_Integer) aTypeAttr;
  if (!theLab.FindAttribute(kSEL_TYPE, aTypeAttr))
    return NULL;
  Selector_Type aType = Selector_Type(aTypeAttr->Get());
  Selector_Algo* aResult = NULL;
  switch (aType) {
  case SELTYPE_CONTAINER: {
    aResult = new Selector_Container;
    break;
  }
  case SELTYPE_INTERSECT: {
    aResult = new Selector_Intersect;
    break;
  }
  case SELTYPE_MODIFICATION: {
    aResult = new Selector_Modify;
    break;
  }
  case SELTYPE_PRIMITIVE: {
    aResult = new Selector_Primitive;
    break;
  }
  case SELTYPE_FILTER_BY_NEIGHBOR: {
    aResult = new Selector_FilterByNeighbors;
    break;
  }
  case SELTYPE_WEAK_NAMING: {
    aResult = new Selector_WeakName;
    break;
  }
  default: { // unknown case
  }
  }
  if (aResult) {
    aResult->myLab = theLab;
    aResult->myBaseDocumentLab = theBaseDocLab;
    aResult->myGeometricalNaming = theLab.IsAttribute(kGEOMETRICAL_NAMING);
    if (!aResult->restore()) {
      delete aResult;
      aResult = NULL;
    }
  }
  return aResult;
}

Selector_Algo* Selector_Algo::restoreByName(TDF_Label theLab, TDF_Label theBaseDocLab,
  std::string theName, const TopAbs_ShapeEnum theShapeType,
  Selector_NameGenerator* theNameGenerator, TDF_Label& theContextLab)
{
  Selector_Algo* aResult = NULL;
  if (theName[0] == '[') { // intersection or container
    switch(theShapeType) {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SHELL:
    case TopAbs_WIRE:
      aResult = new Selector_Container;
      break;
    case TopAbs_VERTEX:
    case TopAbs_EDGE:
    case TopAbs_FACE:
      aResult = new Selector_Intersect;
      break;
    default:;
    }
  } else if (theName[0] == '(') { // filter by neighbors
    aResult = new Selector_FilterByNeighbors;
  } else if (theName.find(pureWeakNameID()) == 0) { // weak naming identifier
    aResult = new Selector_WeakName;
  } else if (theName.find('&') != std::string::npos) { // modification
    aResult = new Selector_Modify;
  } else { // primitive
    aResult = new Selector_Primitive;
  }
  if (aResult) {
    aResult->myLab = theLab;
    aResult->myBaseDocumentLab = theBaseDocLab;
    theContextLab = aResult->restoreByName(theName, theShapeType, theNameGenerator);
    if (theContextLab.IsNull()) {
      delete aResult;
      aResult = NULL;
    }
  }
  return aResult;
}

void Selector_Algo::storeType(const Selector_Type theType)
{
  TDataStd_Integer::Set(myLab, kSEL_TYPE, (int)(theType));
}
