// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_SelectionNaming.cpp
// Created:     11 Aug 2015
// Author:      Mikhail PONIKAROV

#include "Model_SelectionNaming.h"
#include "Model_Document.h"
#include "Model_Objects.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <Events_InfoMessage.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAPI_Wire.h>

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
#include <TColStd_MapOfTransient.hxx>
#include <algorithm>
#include <stdexcept>

#ifdef DEB_NAMING
#include <BRepTools.hxx>
#endif

Model_SelectionNaming::Model_SelectionNaming(TDF_Label theSelectionLab)
{
  myLab = theSelectionLab;
}

std::string Model_SelectionNaming::getShapeName(
  std::shared_ptr<Model_Document> theDoc, const TopoDS_Shape& theShape,
  ResultPtr& theContext, const bool theAnotherDoc, const bool theWholeContext)
{
  std::string aName;
  // add the result name to the name of the shape
  // (it was in BodyBuilder, but did not work on Result rename)
  bool isNeedContextName = theContext->shape().get() != NULL;
  // && !theContext->shape()->isEqual(theSubSh);
  // check if the subShape is already in DF
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(theShape, myLab);
  Handle(TDataStd_Name) anAttr;
  if(!aNS.IsNull() && !aNS->IsEmpty()) { // in the document
    if(aNS->Label().FindAttribute(TDataStd_Name::GetID(), anAttr)) {
      std::shared_ptr<Model_Data> aData =
        std::dynamic_pointer_cast<Model_Data>(theContext->data());
      if (isNeedContextName && aData && aData->label().IsEqual(aNS->Label())) {
        // do nothing because this context name will be added later in this method
      } else {
        aName = TCollection_AsciiString(anAttr->Get()).ToCString();
        // indexes are added to sub-shapes not primitives
        // (primitives must not be located at the same label)
        if(!aName.empty() && aNS->Evolution() != TNaming_PRIMITIVE && isNeedContextName) {
          const TDF_Label& aLabel = aNS->Label();//theDoc->findNamingName(aName);
          static const std::string aPostFix("_");
          TNaming_Iterator anItL(aNS);
          for(int i = 1; anItL.More(); anItL.Next(), i++) {
            // in #1766 IsEqual produced no index of the face
            if(anItL.NewShape().IsSame(theShape)) { 
              aName += aPostFix;
              aName += TCollection_AsciiString (i).ToCString();
              break;
            }
          }
        }
        // if a shape is under another context, use this name, not theContext
        std::shared_ptr<Model_Data> aContextData =
          std::dynamic_pointer_cast<Model_Data>(theContext->data());
        // for constructions the naming is in arguments and has no evolution, so, apply this only
        // for bodies
        if (isNeedContextName && theContext->groupName() == ModelAPI_ResultBody::group() &&
            !aNS->Label().IsDescendant(aContextData->label())) {
          isNeedContextName = false;
          TDF_Label aNSDataLab = aNS->Label();
          while(aNSDataLab.Depth() != 7 && aNSDataLab.Depth() > 5)
            aNSDataLab = aNSDataLab.Father();
          ObjectPtr aNewContext = theDoc->objects()->object(aNSDataLab);
          if (!aNewContext.get() && aNSDataLab.Depth() == 7) {
            aNSDataLab = aNSDataLab.Father().Father();
            aNewContext = theDoc->objects()->object(aNSDataLab);
          }
          if (aNewContext.get()) {
            aName = aNewContext->data()->name() + "/" + aName;
          }
        }
      }
    }
  }

  // Name is empty and this is full context, it just add the whole context name that must be added
  bool isEmptyName = aName.empty();
  if (isNeedContextName && (!isEmptyName || theWholeContext)) {
    aName = theContext->data()->name() + (isEmptyName ? "" : ("/" + aName));
    if (theAnotherDoc)
      aName = theContext->document()->kind() + "/" + aName; // PartSet
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
  std::shared_ptr<GeomAPI_Shape> theSubSh, const std::string& theDefaultName,
  const bool theAnotherDoc)
{
  std::string aName("Undefined name");
  if(!theContext.get() || theContext->shape()->isNull())
    return !theDefaultName.empty() ? theDefaultName : aName;

  // if it is in result of another part
  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(theContext->document());
  if (theContext->groupName() == ModelAPI_ResultPart::group()) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(theContext);
    int anIndex;
    if (theSubSh.get())
      return aPart->data()->name() + "/" + aPart->nameInPart(theSubSh, anIndex);
    else
      return aPart->data()->name();
  }

  if (!theSubSh.get() || theSubSh->isNull()) { // no subshape, so just the whole feature name
    // but if it is in another Part, add this part name
    std::string aPartName;
    if (theAnotherDoc)
      aPartName = theContext->document()->kind() + "/"; // PartSet
    return aPartName + theContext->data()->name();
  }
  TopoDS_Shape aSubShape = theSubSh->impl<TopoDS_Shape>();
  TopoDS_Shape aContext  = theContext->shape()->impl<TopoDS_Shape>();
#ifdef DEB_NAMING
  if(aSubShape.ShapeType() == TopAbs_COMPOUND) {
    BRepTools::Write(aSubShape, "Selection.brep");
    BRepTools::Write(aContext, "Context.brep");
  }
#endif
  aName = getShapeName(aDoc, aSubShape, theContext, theAnotherDoc,
    theContext->shape()->isEqual(theSubSh));

  if(aName.empty() ) { // not in the document!
    TopAbs_ShapeEnum aType = aSubShape.ShapeType();
    switch (aType) {
    case TopAbs_FACE:
      // the Face should be in DF. If it is not the case, it is an error ==> to be debugged
      break;
    case TopAbs_EDGE:
      {
        // name structure: F1 & F2 [& F3 & F4],
        // where F1 & F2 the faces which gives the Edge in trivial case
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
          std::string aFaceName = getShapeName(aDoc, aFace, theContext, theAnotherDoc, false);
          if(i == 1)
            aName = aFaceName;
          else
            aName += "&" + aFaceName;
        }
        TopTools_ListIteratorOfListOfShape itl(aListOfNbs);
        for (;itl.More();itl.Next()) {
          std::string aFaceName = getShapeName(aDoc, itl.Value(), theContext, theAnotherDoc, false);
          aName += "&" + aFaceName;
        }
      }
      break;

    case TopAbs_VERTEX:
      // name structure (Monifold Topology):
      // 1) F1 | F2 | F3 - intersection of 3 faces defines a vertex - trivial case.
      // 2) F1 | F2 | F3 [|F4 [|Fn]] - redundant definition,
      //                               but it should be kept as is to obtain safe recomputation
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
        TopTools_ListOfShape aList;
        TopTools_MapOfShape aFMap;
        // simetimes when group is moved in history, naming may be badly updated, so
        // avoid crash in FindFromKey (issue 1842)
        if (aMap.Contains(aSubShape)) {
          const TopTools_ListOfShape& aList2  = aMap.FindFromKey(aSubShape);
          // fix is below
          TopTools_ListIteratorOfListOfShape itl2(aList2);
          for (int i = 1;itl2.More();itl2.Next(),i++) {
            if(aFMap.Add(itl2.Value()))
              aList.Append(itl2.Value());
          }
        } else
          break;
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
              std::string anEdgeName = getShapeName(aDoc, anEdge, theContext, theAnotherDoc, false);
              if (anEdgeName.empty()) { // edge is not in DS, trying by faces anyway
                isByFaces = true;
                aName.clear();
                break;
              }
              if(i == 1)
                aName = anEdgeName;
              else
                aName += "&" + anEdgeName;
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
            std::string aFaceName = getShapeName(aDoc, aFace, theContext, theAnotherDoc, false);
            if(i == 1)
              aName = aFaceName;
            else
              aName += "&" + aFaceName;
          }
        }
      }
      break;
    }
  }

  return aName;
}

TopAbs_ShapeEnum translateType (const std::string& theType)
{
  // map from the textual shape types to OCCT enumeration
  static std::map<std::string, TopAbs_ShapeEnum> aShapeTypes;

  if(aShapeTypes.size() == 0) {
    aShapeTypes["compound"]   = TopAbs_COMPOUND;
    aShapeTypes["compounds"]  = TopAbs_COMPOUND;
    aShapeTypes["compsolid"]  = TopAbs_COMPSOLID;
    aShapeTypes["compsolids"] = TopAbs_COMPSOLID;
    aShapeTypes["solid"]      = TopAbs_SOLID;
    aShapeTypes["solids"]     = TopAbs_SOLID;
    aShapeTypes["shell"]      = TopAbs_SHELL;
    aShapeTypes["shells"]     = TopAbs_SHELL;
    aShapeTypes["face"]       = TopAbs_FACE;
    aShapeTypes["faces"]      = TopAbs_FACE;
    aShapeTypes["wire"]       = TopAbs_WIRE;
    aShapeTypes["wires"]      = TopAbs_WIRE;
    aShapeTypes["edge"]       = TopAbs_EDGE;
    aShapeTypes["edges"]      = TopAbs_EDGE;
    aShapeTypes["vertex"]     = TopAbs_VERTEX;
    aShapeTypes["vertices"]   = TopAbs_VERTEX;
    aShapeTypes["COMPOUND"]   = TopAbs_COMPOUND;
    aShapeTypes["COMPOUNDS"]  = TopAbs_COMPOUND;
    aShapeTypes["COMPSOLID"]  = TopAbs_COMPSOLID;
    aShapeTypes["COMPSOLIDS"] = TopAbs_COMPSOLID;
    aShapeTypes["SOLID"]      = TopAbs_SOLID;
    aShapeTypes["SOLIDS"]     = TopAbs_SOLID;
    aShapeTypes["SHELL"]      = TopAbs_SHELL;
    aShapeTypes["SHELLS"]     = TopAbs_SHELL;
    aShapeTypes["FACE"]       = TopAbs_FACE;
    aShapeTypes["FACES"]      = TopAbs_FACE;
    aShapeTypes["WIRE"]       = TopAbs_WIRE;
    aShapeTypes["WIRES"]      = TopAbs_WIRE;
    aShapeTypes["EDGE"]       = TopAbs_EDGE;
    aShapeTypes["EDGES"]      = TopAbs_EDGE;
    aShapeTypes["VERTEX"]     = TopAbs_VERTEX;
    aShapeTypes["VERTICES"]   = TopAbs_VERTEX;
  }
  if (aShapeTypes.find(theType) != aShapeTypes.end())
    return aShapeTypes[theType];
  Events_InfoMessage("Model_SelectionNaming",
    "Shape type defined in XML is not implemented!").send();
  return TopAbs_SHAPE;
}

const TopoDS_Shape getShapeFromNS(
  const std::string& theSubShapeName, Handle(TNaming_NamedShape) theNS)
{
  TopoDS_Shape aSelection;
  std::string::size_type n = theSubShapeName.rfind('/');
  if (n == std::string::npos) n = -1;
  std::string aSubString = theSubShapeName.substr(n + 1);
  n = aSubString.rfind('_');
  int indx = 1;
  if (n != std::string::npos) {// for primitives this is a first
    // if we have here the same name as theSubShapeName, there is no index in compound, it is whole
    Handle(TDataStd_Name) aName;
    if (!theNS->Label().FindAttribute(TDataStd_Name::GetID(), aName) ||
        aName->Get() != theSubShapeName.c_str()) {
      aSubString = aSubString.substr(n+1);
      indx = atoi(aSubString.c_str());
    }
  }

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
  //std::string::size_type n, nb = theSubShapeName.rfind('/');
  //if (nb == std::string::npos) nb = 0;
  //std::string aSubString = theSubShapeName.substr(nb + 1);
  std::string aSubString = theSubShapeName;

  TDF_Label aLabel = theDoc->findNamingName(aSubString);
  if (aLabel.IsNull()) { // try to remove additional artificial suffix
    std::string::size_type n = aSubString.rfind('_');
    if (n != std::string::npos) {
      aSubString = aSubString.substr(0, n);
       aLabel = theDoc->findNamingName(aSubString);
    }
  }
  if(aLabel.IsNull()) return aFace;
  Handle(TNaming_NamedShape) aNS;
  if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    aFace = getShapeFromNS(theSubShapeName, aNS);
  }
  return aFace;
}

size_t ParseName(const std::string& theSubShapeName,   std::list<std::string>& theList)
{
  std::string aName = theSubShapeName;
  std::string aLastName;
  size_t n1(0), n2(0); // n1 - start position, n2 - position of the delimiter
  while ((n2 = aName.find('&', n1)) != std::string::npos) {
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
  if(theList.IsEmpty() || theList.Extent() == 1) {
    return TopoDS_Shape();
  }

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
  if (n == std::string::npos) return theSubShapeName;
  aName = theSubShapeName.substr(0, n);
  return aName;
}

/// Parses naming name of sketch sub-elements: takes indices and orientation
/// (if theOriented = true) from this name. Map theIDs constains indices ->
/// orientations and start/end vertices: negative is reversed, 2 - start, 3 - end
bool parseSubIndices(CompositeFeaturePtr theComp, //< to iterate names
  const std::string& theName, const char* theShapeType,
  std::map<int, int>& theIDs, const bool theOriented = false)
{
  // collect all IDs in the name
  std::map<std::string, int> aNames; // short name of sub -> ID of sub of theComp
  const int aSubNum = theComp->numberOfSubs();
  for(int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = theComp->subFeature(a);
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    // there may be many shapes (circle and center)
    for(; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (aConstr.get()) {
        aNames[Model_SelectionNaming::shortName(aConstr)] = theComp->subFeatureId(a);
      }
    }
  }

  size_t aPrevPos = theName.find("/") + 1, aLastNamePos;
  bool isShape = false; // anyway the first world must be 'Vertex'
  do {
    aLastNamePos = theName.find('-', aPrevPos);
    std::string anID = theName.substr(aPrevPos, aLastNamePos - aPrevPos);
    if (!isShape) {
      if (anID != theShapeType)
        return false;
      isShape = true;
    } else {
      int anOrientation = 1; // default
      if (theOriented) { // here must be a symbol in the end of digit 'f' or 'r'
        std::string::iterator aSymbol = anID.end() - 1;
        if (*aSymbol == 'r') anOrientation = -1;
        anID.erase(aSymbol); // remove last symbol
      }
      // check start/end symbols
      std::string::iterator aBack = anID.end() - 1;
      if (*aBack == 's') {
        anOrientation *= 2;
        anID.erase(aBack); // remove last symbol
      } else if (*aBack == 'e') {
        anOrientation *= 3;
        anID.erase(aBack); // remove last symbol
      }

      if (aNames.find(anID) != aNames.end()) {
        theIDs[aNames[anID]] = anOrientation;
      }
    }
    aPrevPos = aLastNamePos + 1;
  } while (aLastNamePos != std::string::npos);
  return true;
}

/// produces theEdge orientation relatively to theContext face
int Model_SelectionNaming::edgeOrientation(const TopoDS_Shape& theContext, TopoDS_Edge& theEdge)
{
  if (theContext.ShapeType() != TopAbs_FACE && theContext.ShapeType() != TopAbs_WIRE)
    return 0;
  if (theEdge.Orientation() == TopAbs_FORWARD)
    return 1;
  if (theEdge.Orientation() == TopAbs_REVERSED)
    return -1;
  return 0; // unknown
}

std::shared_ptr<GeomAPI_Shape> Model_SelectionNaming::findAppropriateFace(
  std::shared_ptr<ModelAPI_Result>& theConstr,
  NCollection_DataMap<Handle(Geom_Curve), int>& theCurves, const bool theIsWire)
{
  int aBestFound = 0; // best number of found edges (not percentage: issue 1019)
  int aBestOrient = 0; // for the equal "BestFound" additional parameter is orientation
  std::shared_ptr<GeomAPI_Shape> aResult;
  ResultConstructionPtr aConstructionContext =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theConstr);
  if (!aConstructionContext.get())
    return aResult;
  for(int aFaceIndex = 0; aFaceIndex < aConstructionContext->facesNum(); aFaceIndex++) {
    int aFound = 0, aNotFound = 0, aSameOrientation = 0;
    TopoDS_Face aFace =
      TopoDS::Face(aConstructionContext->face(aFaceIndex)->impl<TopoDS_Shape>());
    std::list<TopoDS_Shape> aFacesWires; // faces or wires to iterate
    if (theIsWire) {
      for(TopExp_Explorer aWires(aFace, TopAbs_WIRE); aWires.More(); aWires.Next()) {
        aFacesWires.push_back(aWires.Current());
      }
    } else {
      aFacesWires.push_back(aFace);
    }
    std::list<TopoDS_Shape>::iterator aFW = aFacesWires.begin();
    for(; aFW != aFacesWires.end(); aFW++) {
      TopExp_Explorer anEdgesExp(*aFW, TopAbs_EDGE);
      TColStd_MapOfTransient alreadyProcessed; // to avoid counting edges with same curved (841)
      for(; anEdgesExp.More(); anEdgesExp.Next()) {
        TopoDS_Edge anEdge = TopoDS::Edge(anEdgesExp.Current());
        if (!anEdge.IsNull()) {
          Standard_Real aFirst, aLast;
          Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
          if (alreadyProcessed.Contains(aCurve))
            continue;
          alreadyProcessed.Add(aCurve);
          if (theCurves.IsBound(aCurve)) {
            aFound++;
            int anOrient = theCurves.Find(aCurve);
            if (anOrient != 0) {  // extra comparision score is orientation
              if (edgeOrientation(aFace, anEdge) == anOrient)
                aSameOrientation++;
            }
          } else {
            aNotFound++;
          }
        }
      }
      if (aFound + aNotFound != 0) {
        if (aFound > aBestFound ||
          (aFound == aBestFound && aSameOrientation > aBestOrient)) {
            aBestFound = aFound;
            aBestOrient = aSameOrientation;
            if (theIsWire) {
              std::shared_ptr<GeomAPI_Wire> aWire(new GeomAPI_Wire);
              aWire->setImpl(new TopoDS_Shape(*aFW));
              aResult = aWire;
            } else {
              aResult = aConstructionContext->face(aFaceIndex);
            }
        }
      }
    }
  }
  return aResult;
}

std::string Model_SelectionNaming::shortName(
  std::shared_ptr<ModelAPI_ResultConstruction>& theConstr, const int theEdgeVertexPos)
{
  std::string aName = theConstr->data()->name();
  // remove "-", "/" and "&" command-symbols
  aName.erase(std::remove(aName.begin(), aName.end(), '-'), aName.end());
  aName.erase(std::remove(aName.begin(), aName.end(), '/'), aName.end());
  aName.erase(std::remove(aName.begin(), aName.end(), '&'), aName.end());
  // remove the last 's', 'e', 'f' and 'r' symbols:
  // they are used as markers of start/end/forward/rewersed indicators
  static const std::string aSyms("sefr");
  std::string::iterator aSuffix = aName.end() - 1;
  while(aSyms.find(*aSuffix) != std::string::npos) {
    --aSuffix;
  }
  aName.erase(aSuffix + 1, aName.end());

  if (theEdgeVertexPos == 1) {
    aName += "s"; // start
  } else if (theEdgeVertexPos == 2) {
    aName += "e"; // end
  }
  return aName;
}

// type ::= COMP | COMS | SOLD | SHEL | FACE | WIRE | EDGE | VERT
bool Model_SelectionNaming::selectSubShape(const std::string& theType,
  const std::string& theSubShapeName, std::shared_ptr<Model_Document> theDoc,
  std::shared_ptr<GeomAPI_Shape>& theShapeToBeSelected, std::shared_ptr<ModelAPI_Result>& theCont)
{
  if(theSubShapeName.empty() || theType.empty()) return false;
  TopAbs_ShapeEnum aType = translateType(theType);

  // check that it was selected in another document
  size_t aSlash = theSubShapeName.find("/");
  std::string aSubShapeName = theSubShapeName;
  std::shared_ptr<Model_Document> aDoc = theDoc;
  if (aSlash != std::string::npos) {
    std::string aDocName = theSubShapeName.substr(0, aSlash);
    ResultPartPtr aFoundPart;
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    if (aDocName == aRootDoc->kind()) {
      aDoc = std::dynamic_pointer_cast<Model_Document>(aRootDoc);
    } else {
      for (int a = aRootDoc->size(ModelAPI_ResultPart::group()) - 1; a >= 0; a--) {
        ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(
            aRootDoc->object(ModelAPI_ResultPart::group(), a));
        if (aPart.get() && aPart->isActivated() && aPart->data()->name() == aDocName) {
          aDoc = std::dynamic_pointer_cast<Model_Document>(aPart->partDoc());
          aFoundPart = aPart;
          break;
        }
      }
    }
    if (aDoc != theDoc) {
      // so, the first word is the document name => reduce the string for the next manips
      aSubShapeName = theSubShapeName.substr(aSlash + 1);
      if (aSubShapeName.empty() && aFoundPart.get()) { // the whole Part result
        theCont = aFoundPart;
        return true;
      }
    }
  }

  std::string aContName = getContextName(aSubShapeName);
  if(aContName.empty()) return false;
  ResultPtr aCont = aDoc->findByName(aContName);
   // possible this is body where postfix is added to distinguish several shapes on the same label
  int aSubShapeId = -1; // -1 means sub shape not found
  // for result body the name wihtout "_" has higher priority than with it: it is always added
  if ((!aCont.get()/* || (aCont->groupName() == ModelAPI_ResultBody::group())*/) &&
       aContName == aSubShapeName) {
    size_t aPostIndex = aContName.rfind('_');
    if (aPostIndex != std::string::npos) {
      std::string aSubContName = aContName.substr(0, aPostIndex);
      ResultPtr aSubCont = aDoc->findByName(aSubContName);
      if (aSubCont.get()) {
        try {
          std::string aNum = aContName.substr(aPostIndex + 1);
          aSubShapeId = std::stoi(aNum);
        } catch (std::invalid_argument&) {
          aSubShapeId = -1;
        }
        if (aSubShapeId > 0) {
          aContName = aSubContName;
          aCont = aSubCont;
        }
      }
    }
  }


  TopoDS_Shape aSelection;
  switch (aType)
  {
  case TopAbs_FACE:
  case TopAbs_WIRE:
    {
      aSelection = findFaceByName(aSubShapeName, aDoc);
    }
    break;
  case TopAbs_EDGE:
    {
      const TDF_Label& aLabel = aDoc->findNamingName(aSubShapeName);
      if(!aLabel.IsNull()) {
        Handle(TNaming_NamedShape) aNS;
        if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          aSelection = getShapeFromNS(aSubShapeName, aNS);
        }
      }
    }
    break;
  case TopAbs_VERTEX:
    {
      const TDF_Label& aLabel = aDoc->findNamingName(aSubShapeName);
      if(!aLabel.IsNull()) {
        Handle(TNaming_NamedShape) aNS;
        if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          aSelection = getShapeFromNS(aSubShapeName, aNS);
        }
      }
    }
    break;
  case TopAbs_COMPOUND:
  case TopAbs_COMPSOLID:
  case TopAbs_SOLID:
  case TopAbs_SHELL:
  default: {//TopAbs_SHAPE
    /// case when the whole sketch is selected, so,
    /// selection type is compound, but there is no value
    if (aCont.get() && aCont->shape().get()) {
      if (aCont->shape()->impl<TopoDS_Shape>().ShapeType() == aType) {
        theCont = aCont;
        return true;
      } else if (aSubShapeId > 0) { // try to find sub-shape by the index
        TopExp_Explorer anExp(aCont->shape()->impl<TopoDS_Shape>(), aType);
        for(; aSubShapeId > 1 && anExp.More(); aSubShapeId--) {
          anExp.Next();
        }
        if (anExp.More()) {
          std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
          aShapeToBeSelected->setImpl(new TopoDS_Shape(anExp.Current()));
          theShapeToBeSelected = aShapeToBeSelected;
          theCont = aCont;
          return true;
        }
      }
    }
    return false;
    }
  }
  // another try to find edge or vertex by faces
  std::list<std::string> aListofNames;
  size_t aN = aSelection.IsNull() ? ParseName(aSubShapeName, aListofNames) : 0;
  if (aSelection.IsNull() && (aType == TopAbs_EDGE || aType == TopAbs_VERTEX)) {
    if(aN > 1 && (aN < 4 || (aType == TopAbs_EDGE && aN < 5))) { // 2 || 3 or 4 for EDGE
      TopTools_ListOfShape aList;
      std::list<std::string>::iterator it = aListofNames.begin();
      for(; it != aListofNames.end(); it++){
        const TopoDS_Shape aFace = findFaceByName(*it, aDoc);
        if(!aFace.IsNull())
          aList.Append(aFace);
      }
      aSelection = findCommonShape(aType, aList);
    }
  }
  // in case of construction, there is no registered names for all sub-elements,
  // even for the main element; so, trying to find them by name (without "&" intersections)
  if (aN == 0) {
    size_t aConstrNamePos = aSubShapeName.find("/");
    bool isFullName = aConstrNamePos == std::string::npos;
    std::string aContrName = aContName;
    ResultPtr aConstr = aDoc->findByName(aContrName);
    if (aConstr.get() && aConstr->groupName() == ModelAPI_ResultConstruction::group()) {
      theCont = aConstr;
      if (isFullName) {
        // For the full construction selection shape must be empty.
        //theShapeToBeSelected = aConstr->shape();
        return true;
      }
      // for sketch sub-elements selected
      CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aDoc->feature(aConstr));
      if (aComposite.get()) {
        if (aType == TopAbs_VERTEX || aType == TopAbs_EDGE) {
          // collect all IDs in the name
          std::map<int, int> anIDs;
          if (!parseSubIndices(aComposite, aSubShapeName,
              aType == TopAbs_EDGE ? "Edge" : "Vertex", anIDs))
            return false;

          const int aSubNum = aComposite->numberOfSubs();
          for(int a = 0; a < aSubNum; a++) {
            int aCompID = aComposite->subFeatureId(a);
            if (anIDs.find(aCompID) != anIDs.end()) { // found the vertex/edge shape
              FeaturePtr aSub = aComposite->subFeature(a);
              const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIt = aResults.cbegin();
              // there may be many shapes (circle and center)
              for(; aRIt != aResults.cend(); aRIt++) {
                ResultConstructionPtr aRes =
                  std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRIt);
                if (aRes) {
                  int anOrientation = abs(anIDs[aCompID]);
                  TopoDS_Shape aShape = aRes->shape()->impl<TopoDS_Shape>();
                  if (anOrientation == 1) {
                    if (aType == aShape.ShapeType()) {
                      theShapeToBeSelected = aRes->shape();
                      return true;
                    }
                  } else { // take first or second vertex of the edge
                    TopoDS_Shape aShape = aRes->shape()->impl<TopoDS_Shape>();
                    TopExp_Explorer anExp(aShape, aType);
                    for(; anExp.More() && anOrientation != 2; anOrientation--)
                      anExp.Next();
                    if (anExp.More()) {
                      std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
                      aShapeToBeSelected->setImpl(new TopoDS_Shape(anExp.Current()));
                      theShapeToBeSelected = aShapeToBeSelected;
                      return true;
                    }
                  }
                }
              }
            }
          }
          // sketch faces is identified by format "Sketch_1/Face-2f-8f-11r"
        } else if (aType == TopAbs_FACE || aType == TopAbs_WIRE) {
          std::map<int, int> anIDs;
          if (!parseSubIndices(aComposite, aSubShapeName,
              aType == TopAbs_FACE ? "Face" : "Wire", anIDs, true))
            return false;

          // curves and orientations of edges
          NCollection_DataMap<Handle(Geom_Curve), int> allCurves;
          const int aSubNum = aComposite->numberOfSubs();
          for(int a = 0; a < aSubNum; a++) {
            int aSubID = aComposite->subFeatureId(a);
            if (anIDs.find(aSubID) != anIDs.end()) {
              FeaturePtr aSub = aComposite->subFeature(a);
              const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes;
              for(aRes = aResults.cbegin(); aRes != aResults.cend(); aRes++) {
                ResultConstructionPtr aConstr =
                  std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
                if (aConstr->shape() && aConstr->shape()->isEdge()) {
                  const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
                  TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
                  if (!anEdge.IsNull()) {
                    Standard_Real aFirst, aLast;
                    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                    allCurves.Bind(aCurve, anIDs[aSubID] > 0 ? 1 : -1);
                  }
                }
              }
            }
          }
          std::shared_ptr<GeomAPI_Shape> aFoundFW =
            findAppropriateFace(aConstr, allCurves, aType == TopAbs_WIRE);
          if (aFoundFW.get()) {
            theShapeToBeSelected = aFoundFW;
            return true;
          }
        } else if (aType == TopAbs_WIRE) {
          // sketch faces is identified by format "Sketch_1/Face-2f-8f-11r"
          std::map<int, int> anIDs;
          if (!parseSubIndices(aComposite, aSubShapeName, "Wire", anIDs))
            return false;

           // curves and orientations of edges
          NCollection_DataMap<Handle(Geom_Curve), int> allCurves;
          const int aSubNum = aComposite->numberOfSubs();
          for(int a = 0; a < aSubNum; a++) {
            int aSubID = aComposite->subFeatureId(a);
            if (anIDs.find(aSubID) != anIDs.end()) {
              FeaturePtr aSub = aComposite->subFeature(a);
              const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes;
              for(aRes = aResults.cbegin(); aRes != aResults.cend(); aRes++) {
                ResultConstructionPtr aConstr =
                  std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
                if (aConstr->shape() && aConstr->shape()->isEdge()) {
                  const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
                  TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
                  if (!anEdge.IsNull()) {
                    Standard_Real aFirst, aLast;
                    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                    allCurves.Bind(aCurve, anIDs[aSubID] > 0 ? 1 : -1);
                  }
                }
              }
            }
          }
          std::shared_ptr<GeomAPI_Shape> aFoundFW =
            findAppropriateFace(aConstr, allCurves, aType == TopAbs_WIRE);
          if (aFoundFW.get()) {
            theShapeToBeSelected = aFoundFW;
            return true;
          }
        }
      }
    }
  }
  if (!aSelection.IsNull()) {
    // Select it (must be after N=0 checking,
    // since for simple constructions the shape must be null)
    std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
    aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
    theShapeToBeSelected = aShapeToBeSelected;
    theCont = aCont;
    return true;
  }

  return false;
}
