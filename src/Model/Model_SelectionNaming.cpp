// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_SelectionNaming.cpp
// Created:     11 Aug 2015
// Author:      Mikhail PONIKAROV

#include "Model_SelectionNaming.h"
#include "Model_Document.h"
#include <ModelAPI_Feature.h>
#include <Events_Error.h>

#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <BRep_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Localizer.hxx>
#include <TDataStd_Name.hxx>

#ifdef DEB_NAMING
#include <BRepTools.hxx>
#endif

Model_SelectionNaming::Model_SelectionNaming(TDF_Label theSelectionLab)
{
  myLab = theSelectionLab;
}


std::string Model_SelectionNaming::getShapeName(
  std::shared_ptr<Model_Document> theDoc, const TopoDS_Shape& theShape)
{
  std::string aName;
  // check if the subShape is already in DF
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(theShape, myLab);
  Handle(TDataStd_Name) anAttr;
  if(!aNS.IsNull() && !aNS->IsEmpty()) { // in the document    
    if(aNS->Label().FindAttribute(TDataStd_Name::GetID(), anAttr)) {
      aName = TCollection_AsciiString(anAttr->Get()).ToCString();
      if(!aName.empty()) {	    
        const TDF_Label& aLabel = theDoc->findNamingName(aName);
        /* MPV: the same shape with the same name may be duplicated on different labels (selection of the same construction object)
        if(!aLabel.IsEqual(aNS->Label())) {
        //aName.erase(); //something is wrong, to be checked!!!
        aName += "_SomethingWrong";
        return aName;
        }*/

        static const std::string aPostFix("_");
        TNaming_Iterator anItL(aNS);
        for(int i = 1; anItL.More(); anItL.Next(), i++) {
          if(anItL.NewShape() == theShape) {
            aName += aPostFix;
            aName += TCollection_AsciiString (i).ToCString();
            break;
          }
        }
      }	
    }
  }
  return aName;
}



bool isTrivial (const TopTools_ListOfShape& theAncestors, TopTools_IndexedMapOfShape& theSMap)
{
  // a trivial case: F1 & F2,  aNumber = 1, i.e. intersection gives 1 edge.
  TopoDS_Compound aCmp;
  BRep_Builder BB;
  BB.MakeCompound(aCmp);
  TopTools_ListIteratorOfListOfShape it(theAncestors);
  for(;it.More();it.Next()) {
    BB.Add(aCmp, it.Value());
    theSMap.Add(it.Value());
  }
  int aNumber(0);
  TopTools_IndexedDataMapOfShapeListOfShape aMap2;
  TopExp::MapShapesAndAncestors(aCmp, TopAbs_EDGE, TopAbs_FACE, aMap2);
  for (int i = 1; i <= aMap2.Extent(); i++) {
    const TopoDS_Shape& aKey = aMap2.FindKey(i);
    const TopTools_ListOfShape& anAncestors = aMap2.FindFromIndex(i);
    if(anAncestors.Extent() > 1) aNumber++;
  }
  if(aNumber > 1) return false;
  return true;
}

std::string Model_SelectionNaming::namingName(ResultPtr& theContext,
  std::shared_ptr<GeomAPI_Shape> theSubSh, const std::string& theDefaultName)
{
  std::string aName("Undefined name");
  if(!theContext.get() || theContext->shape()->isNull()) 
    return !theDefaultName.empty() ? theDefaultName : aName;
  if (!theSubSh.get() || theSubSh->isNull()) { // no subshape, so just the whole feature name
    return theContext->data()->name();
  }
  TopoDS_Shape aSubShape = theSubSh->impl<TopoDS_Shape>();
  TopoDS_Shape aContext  = theContext->shape()->impl<TopoDS_Shape>();
#ifdef DEB_NAMING
  if(aSubShape.ShapeType() == TopAbs_COMPOUND) {
    BRepTools::Write(aSubShape, "Selection.brep");
    BRepTools::Write(aContext, "Context.brep");
  }
#endif
  std::shared_ptr<Model_Document> aDoc = 
    std::dynamic_pointer_cast<Model_Document>(theContext->document());

  // check if the subShape is already in DF
  aName = getShapeName(aDoc, aSubShape);
  if(aName.empty() ) { // not in the document!
    TopAbs_ShapeEnum aType = aSubShape.ShapeType();
    switch (aType) {
    case TopAbs_FACE:
      // the Face should be in DF. If it is not the case, it is an error ==> to be debugged		
      break;
    case TopAbs_EDGE:
      {
        // name structure: F1 | F2 [| F3 | F4], where F1 & F2 the faces which gives the Edge in trivial case
        // if it is not atrivial case we use localization by neighbours. F3 & F4 - neighbour faces	
        if (BRep_Tool::Degenerated(TopoDS::Edge(aSubShape))) {
          aName = "Degenerated_Edge";
          break;
        }    
        TopTools_IndexedDataMapOfShapeListOfShape aMap;
        TopExp::MapShapesAndAncestors(aContext, TopAbs_EDGE, TopAbs_FACE, aMap);
        TopTools_IndexedMapOfShape aSMap; // map for ancestors of the sub-shape
        bool isTrivialCase(true);
        if(aMap.Contains(aSubShape)) {
          const TopTools_ListOfShape& anAncestors = aMap.FindFromKey(aSubShape);
          // check that it is not a trivial case (F1 & F2: aNumber = 1)
          isTrivialCase = isTrivial(anAncestors, aSMap);		
        } else 
          break;
        TopTools_ListOfShape aListOfNbs;
        if(!isTrivialCase) { // find Neighbors
          TNaming_Localizer aLocalizer;
          TopTools_MapOfShape aMap3;
          aLocalizer.FindNeighbourg(aContext, aSubShape, aMap3);
          //int n = aMap3.Extent();
          TopTools_MapIteratorOfMapOfShape it(aMap3);
          for(;it.More();it.Next()) {
            const TopoDS_Shape& aNbShape = it.Key(); // neighbor edge
            //TopAbs_ShapeEnum aType = aNbShape.ShapeType();
            const TopTools_ListOfShape& aList  = aMap.FindFromKey(aNbShape);
            TopTools_ListIteratorOfListOfShape it2(aList);
            for(;it2.More();it2.Next()) {
              if(aSMap.Contains(it2.Value())) continue; // skip this Face
              aListOfNbs.Append(it2.Value());
            }
          }
        }  // else a trivial case

        // build name of the sub-shape Edge
        for(int i=1; i <= aSMap.Extent(); i++) {
          const TopoDS_Shape& aFace = aSMap.FindKey(i);
          std::string aFaceName = getShapeName(aDoc, aFace);
          if(i == 1)
            aName = aFaceName;
          else 
            aName += "|" + aFaceName;
        }
        TopTools_ListIteratorOfListOfShape itl(aListOfNbs);
        for (;itl.More();itl.Next()) {
          std::string aFaceName = getShapeName(aDoc, itl.Value());
          aName += "|" + aFaceName;
        }		  
      }
      break;

    case TopAbs_VERTEX:
      // name structure (Monifold Topology): 
      // 1) F1 | F2 | F3 - intersection of 3 faces defines a vertex - trivial case.
      // 2) F1 | F2 | F3 [|F4 [|Fn]] - redundant definition, but it should be kept as is to obtain safe recomputation
      // 2) F1 | F2      - intersection of 2 faces definses a vertex - applicable for case
      //                   when 1 faces is cylindrical, conical, spherical or revolution and etc.
      // 3) E1 | E2 | E3 - intersection of 3 edges defines a vertex - when we have case of a shell
      //                   or compound of 2 open faces.
      // 4) E1 | E2      - intesection of 2 edges defines a vertex - when we have a case of 
      //                   two independent edges (wire or compound)
      // implemented 2 first cases
      {
        TopTools_IndexedDataMapOfShapeListOfShape aMap;
        TopExp::MapShapesAndAncestors(aContext, TopAbs_VERTEX, TopAbs_FACE, aMap);
        const TopTools_ListOfShape& aList2  = aMap.FindFromKey(aSubShape);
        TopTools_ListOfShape aList;
        TopTools_MapOfShape aFMap;
        // fix is below
        TopTools_ListIteratorOfListOfShape itl2(aList2);
        for (int i = 1;itl2.More();itl2.Next(),i++) {
          if(aFMap.Add(itl2.Value()))
            aList.Append(itl2.Value());
        }
        int n = aList.Extent();
        bool isByFaces = n >= 3;
        if(!isByFaces) { // open topology case or Compound case => via edges
          TopTools_IndexedDataMapOfShapeListOfShape aMap;
          TopExp::MapShapesAndAncestors(aContext, TopAbs_VERTEX, TopAbs_EDGE, aMap);
          const TopTools_ListOfShape& aList22  = aMap.FindFromKey(aSubShape);
          if(aList22.Extent() >= 2)  { // regular solution

            // bug! duplication; fix is below
            aFMap.Clear();
            TopTools_ListOfShape aListE;
            TopTools_ListIteratorOfListOfShape itl2(aList22);
            for (int i = 1;itl2.More();itl2.Next(),i++) {
              if(aFMap.Add(itl2.Value()))
                aListE.Append(itl2.Value());
            }
            n = aListE.Extent();
            TopTools_ListIteratorOfListOfShape itl(aListE);
            for (int i = 1;itl.More();itl.Next(),i++) {
              const TopoDS_Shape& anEdge = itl.Value();
              std::string anEdgeName = getShapeName(aDoc, anEdge);
              if (anEdgeName.empty()) { // edge is not in DS, trying by faces anyway
                isByFaces = true;
                aName.clear();
                break;
              }
              if(i == 1)
                aName = anEdgeName;
              else 
                aName += "|" + anEdgeName;
            }
          }//reg
          else { // dangle vertex: if(aList22.Extent() == 1)
            //it should be already in DF
          }
        } 
        if (isByFaces) {
          TopTools_ListIteratorOfListOfShape itl(aList);
          for (int i = 1;itl.More();itl.Next(),i++) {
            const TopoDS_Shape& aFace = itl.Value();
            std::string aFaceName = getShapeName(aDoc, aFace);
            if(i == 1)
              aName = aFaceName;
            else 
              aName += "|" + aFaceName;
          }
        }
      }
      break;
    }
    // register name			
    // aDoc->addNamingName(selectionLabel(), aName);
    // the selected sub-shape will not be shared and as result it will not require registration
  }
  return aName;
}

TopAbs_ShapeEnum translateType (const std::string& theType)
{
  // map from the textual shape types to OCCT enumeration
  static std::map<std::string, TopAbs_ShapeEnum> MyShapeTypes;
  if (MyShapeTypes.size() == 0) {
    MyShapeTypes["face"] = TopAbs_FACE;
    MyShapeTypes["faces"] = TopAbs_FACE;
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
    MyShapeTypes["vertices"] = TopAbs_VERTEX;
    MyShapeTypes["wire"] = TopAbs_WIRE;
    MyShapeTypes["edge"] = TopAbs_EDGE;
    MyShapeTypes["edges"] = TopAbs_EDGE;
    MyShapeTypes["shell"] = TopAbs_SHELL;
    MyShapeTypes["solid"] = TopAbs_SOLID;
    MyShapeTypes["solids"] = TopAbs_SOLID;
    MyShapeTypes["FACE"] = TopAbs_FACE;
    MyShapeTypes["FACES"] = TopAbs_FACE;
    MyShapeTypes["VERTEX"] = TopAbs_VERTEX;
    MyShapeTypes["VERTICES"] = TopAbs_VERTEX;
    MyShapeTypes["WIRE"] = TopAbs_WIRE;
    MyShapeTypes["EDGE"] = TopAbs_EDGE;
    MyShapeTypes["EDGES"] = TopAbs_EDGE;
    MyShapeTypes["SHELL"] = TopAbs_SHELL;
    MyShapeTypes["SOLID"] = TopAbs_SOLID;
    MyShapeTypes["SOLIDS"] = TopAbs_SOLID;
  }
  if (MyShapeTypes.find(theType) != MyShapeTypes.end())
    return MyShapeTypes[theType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}

const TopoDS_Shape getShapeFromNS(
  const std::string& theSubShapeName, Handle(TNaming_NamedShape) theNS)
{
  TopoDS_Shape aSelection;
  std::string::size_type n = theSubShapeName.rfind('/');			
  if (n == std::string::npos) n = 0;
  std::string aSubString = theSubShapeName.substr(n + 1);
  n = aSubString.rfind('_');
  if (n == std::string::npos) return aSelection;
  aSubString = aSubString.substr(n+1);
  int indx = atoi(aSubString.c_str());

  TNaming_Iterator anItL(theNS);
  for(int i = 1; anItL.More(); anItL.Next(), i++) {
    if (i == indx) {
      return anItL.NewShape();
    }
  }
  return aSelection;	
}

const TopoDS_Shape findFaceByName(
  const std::string& theSubShapeName, std::shared_ptr<Model_Document> theDoc)
{
  TopoDS_Shape aFace;
  std::string::size_type n, nb = theSubShapeName.rfind('/');			
  if (nb == std::string::npos) nb = 0;
  std::string aSubString = theSubShapeName.substr(nb + 1);
  n = aSubString.rfind('_');
  if (n != std::string::npos) {
    std::string aSubStr2 = aSubString.substr(0, n);
    aSubString  = theSubShapeName.substr(0, nb + 1);
    aSubString = aSubString + aSubStr2;	
  } else
    aSubString = theSubShapeName;

  const TDF_Label& aLabel = theDoc->findNamingName(aSubString);
  if(aLabel.IsNull()) return aFace;
  Handle(TNaming_NamedShape) aNS;
  if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    aFace = getShapeFromNS(theSubShapeName, aNS);
  }
  return aFace;
}

int ParseName(const std::string& theSubShapeName,   std::list<std::string>& theList)
{
  std::string aName = theSubShapeName;
  std::string aLastName;
  int n1(0), n2(0); // n1 - start position, n2 - position of the delimiter
  while ((n2 = aName.find('|', n1)) != std::string::npos) {
    const std::string aName1 = aName.substr(n1, n2 - n1); //name of face
    theList.push_back(aName1);	
    n1 = n2 + 1;
    aLastName = aName.substr(n1);
  }
  if(!aLastName.empty())
    theList.push_back(aLastName);
  return theList.size();
}

const TopoDS_Shape findCommonShape(
  const TopAbs_ShapeEnum theType, const TopTools_ListOfShape& theList)
{
  TopoDS_Shape aShape;
  std::vector<TopTools_MapOfShape> aVec;
  TopTools_MapOfShape aMap1, aMap2, aMap3, aMap4;
  if(theList.Extent() > 1) {
    aVec.push_back(aMap1);
    aVec.push_back(aMap2);
  }
  if(theList.Extent() > 2)
    aVec.push_back(aMap3);
  if(theList.Extent() == 4)
    aVec.push_back(aMap4);

  //fill maps
  TopTools_ListIteratorOfListOfShape it(theList);
  for(int i = 0;it.More();it.Next(),i++) {
    const TopoDS_Shape& aFace = it.Value();		
    if(i < 2) {
      TopExp_Explorer anExp (aFace, theType);
      for(;anExp.More();anExp.Next()) {
        const TopoDS_Shape& anEdge = anExp.Current();
        if (!anEdge.IsNull())
          aVec[i].Add(anExp.Current());
      }
    } else {
      TopExp_Explorer anExp (aFace, TopAbs_VERTEX);
      for(;anExp.More();anExp.Next()) {
        const TopoDS_Shape& aVertex = anExp.Current();
        if (!aVertex.IsNull())
          aVec[i].Add(anExp.Current());
      }
    }
  }
  //trivial case: 2 faces
  TopTools_ListOfShape aList;
  TopTools_MapIteratorOfMapOfShape it2(aVec[0]);
  for(;it2.More();it2.Next()) {
    if(aVec[1].Contains(it2.Key())) {
      aShape = it2.Key();
      if(theList.Extent() == 2)
        break;
      else 
        aList.Append(it2.Key());
    }
  }
  if(aList.Extent() && aVec.size() > 3) {// list of common edges ==> search ny neighbors 
    if(aVec[2].Extent() && aVec[3].Extent()) {
      TopTools_ListIteratorOfListOfShape it(aList);
      for(;it.More();it.Next()) {
        const TopoDS_Shape& aCand = it.Value();
        // not yet completelly implemented, to be rechecked
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(TopoDS::Edge(aCand), aV1, aV2);
        int aNum(0);
        if(aVec[2].Contains(aV1)) aNum++;
        else if(aVec[2].Contains(aV2)) aNum++;
        if(aVec[3].Contains(aV1)) aNum++;
        else if(aVec[3].Contains(aV2)) aNum++;
        if(aNum == 2) {
          aShape = aCand;
          break;
        }
      }
    }
  }

  if(aList.Extent() && aVec.size() == 3) {

    TopTools_ListIteratorOfListOfShape it(aList);
    for(;it.More();it.Next()) {
      const TopoDS_Shape& aCand = it.Value();
      if(aVec[2].Contains(aCand)) {
        aShape = aCand;
        break;
      }
    }
  }
  return aShape;
}

std::string getContextName(const std::string& theSubShapeName)
{
  std::string aName;
  std::string::size_type n = theSubShapeName.find('/');			
  if (n == std::string::npos) return aName;
  aName = theSubShapeName.substr(0, n);
  return aName;
}

// type ::= COMP | COMS | SOLD | SHEL | FACE | WIRE | EDGE | VERT
bool Model_SelectionNaming::selectSubShape(const std::string& theType, 
  const std::string& theSubShapeName, std::shared_ptr<Model_Document> theDoc,
  std::shared_ptr<GeomAPI_Shape>& theShapeToBeSelected, std::shared_ptr<ModelAPI_Result>& theCont)
{
  if(theSubShapeName.empty() || theType.empty()) return false;
  TopAbs_ShapeEnum aType = translateType(theType);
  std::string aContName = getContextName(theSubShapeName);
  if(aContName.empty()) return false;
  ResultPtr aCont = theDoc->findByName(aContName);
  if(!aCont.get() || aCont->shape()->isNull()) return false;
  TopoDS_Shape aContext  = aCont->shape()->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aContType = aContext.ShapeType();
  if(aType <= aContType) return false; // not applicable


  TopoDS_Shape aSelection;
  switch (aType) 
  {
  case TopAbs_COMPOUND:
    break;
  case TopAbs_COMPSOLID:
    break;
  case TopAbs_SOLID:
    break;
  case TopAbs_SHELL:
    break;
  case TopAbs_FACE:
    {
      const TopoDS_Shape aSelection = findFaceByName(theSubShapeName, theDoc);
      if(!aSelection.IsNull()) {// Select it
        std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
        aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
        theShapeToBeSelected = aShapeToBeSelected;
        theCont = aCont;
        return true;
      }
    }
    break;
  case TopAbs_WIRE:
    break;
  case TopAbs_EDGE:
    {  
      TopoDS_Shape aSelection;// = findFaceByName(theSubShapeName, aDoc);
      const TDF_Label& aLabel = theDoc->findNamingName(theSubShapeName);
      if(!aLabel.IsNull()) {
        Handle(TNaming_NamedShape) aNS;
        if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          aSelection = getShapeFromNS(theSubShapeName, aNS);
        }
      }
      if(aSelection.IsNull()) {
        std::list<std::string> aListofNames;
        int n = ParseName(theSubShapeName, aListofNames);
        if(n > 1 && n < 5) {
          TopTools_ListOfShape aList;
          std::list<std::string>::iterator it =aListofNames.begin();
          for(;it != aListofNames.end();it++){
            const TopoDS_Shape aFace = findFaceByName(*it, theDoc);
            aList.Append(aFace);		
          }
          aSelection = findCommonShape(TopAbs_EDGE, aList);
        }
      }
      if(!aSelection.IsNull()) {// Select it
        std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
        aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
        theShapeToBeSelected = aShapeToBeSelected;
        theCont = aCont;
        return true;
      }
    }
    break;
  case TopAbs_VERTEX:
    {
      TopoDS_Shape aSelection;
      const TDF_Label& aLabel = theDoc->findNamingName(theSubShapeName);
      if(!aLabel.IsNull()) {
        Handle(TNaming_NamedShape) aNS;
        if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          aSelection = getShapeFromNS(theSubShapeName, aNS);
        }
      }
      if(aSelection.IsNull()) {
        std::list<std::string> aListofNames;
        int n = ParseName(theSubShapeName, aListofNames);
        if(n > 1 && n < 4) { // 2 || 3
          TopTools_ListOfShape aList;
          std::list<std::string>::iterator it = aListofNames.begin();
          for(; it != aListofNames.end(); it++){
            const TopoDS_Shape aFace = findFaceByName(*it, theDoc);
            if(!aFace.IsNull())
              aList.Append(aFace);		
          }
          aSelection = findCommonShape(TopAbs_VERTEX, aList);
        }
      }
      if(!aSelection.IsNull()) {// Select it
        std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
        aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
        theShapeToBeSelected = aShapeToBeSelected;
        theCont = aCont;
        return true;
      }
    }
    break;
  default: //TopAbs_SHAPE
    return false;
  }
  return false;
}

