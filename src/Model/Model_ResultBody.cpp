// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultBody.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultBody.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <ModelAPI_AttributeIntArray.h>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TDF_ChildIterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopExp.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <Config_PropManager.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#include <TDF_Tool.hxx>
//#define DEB_IMPORT 1

Model_ResultBody::Model_ResultBody()
{
  myIsDisabled = true; // by default it is not initialized and false to be after created
  setIsConcealed(false);
}

void Model_ResultBody::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
}

void Model_ResultBody::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_body_color";
  theDefault = DEFAULT_COLOR();
}

// Converts evolution of naming shape to selection evelution and back to avoid
// naming support on the disabled results. Deeply in the labels tree, recursively.
static void EvolutionToSelection(TDF_Label theLab, const bool theFlag) {
  std::list<std::pair<TopoDS_Shape, TopoDS_Shape> > aShapePairs; // to store old and new shapes
  Handle(TNaming_NamedShape) aName;
  int anEvolution = -1;
  if (theLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
    TNaming_Evolution aNSEvol = aName->Evolution();
    if ((aNSEvol == TNaming_SELECTED && theFlag) ||
        (aNSEvol != TNaming_SELECTED && !theFlag)) { // nothing to do, it is already correct
      return;
    }
    anEvolution = (int)(aNSEvol);
    for(TNaming_Iterator anIter(aName); anIter.More(); anIter.Next()) {
      aShapePairs.push_back(std::pair<TopoDS_Shape, TopoDS_Shape>
        (anIter.OldShape(), anIter.NewShape()));
    }
  }
  // remove old
  theLab.ForgetAttribute(TNaming_NamedShape::GetID());
  // create new
  TNaming_Builder aBuilder(theLab);
  TNaming_Evolution anEvol = (TNaming_Evolution)(anEvolution);
  std::list<std::pair<TopoDS_Shape, TopoDS_Shape> >::iterator aPairsIter = aShapePairs.begin();
  for(; aPairsIter != aShapePairs.end(); aPairsIter++) {
    if (theFlag) { // disabled => make selection
      aBuilder.Select(aPairsIter->first, aPairsIter->second);
    } else if (anEvol == TNaming_GENERATED) {
      aBuilder.Generated(aPairsIter->first, aPairsIter->second);
    } else if (anEvol == TNaming_MODIFY) {
      aBuilder.Modify(aPairsIter->first, aPairsIter->second);
    } else if (anEvol == TNaming_DELETE) {
      aBuilder.Delete(aPairsIter->first);
    } else if (anEvol == TNaming_PRIMITIVE) {
      aBuilder.Generated(aPairsIter->second);
    }
  }
  // recursive call for all sub-labels
  TDF_ChildIterator anIter(theLab, Standard_False);
  for(; anIter.More(); anIter.Next()) {
    EvolutionToSelection(anIter.Value(), theFlag);
  }
}

bool Model_ResultBody::setDisabled(std::shared_ptr<ModelAPI_Result> theThis, const bool theFlag)
{
  bool aChanged = ModelAPI_ResultBody::setDisabled(theThis, theFlag);
  if (aChanged) { // state is changed, so modifications are needed
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    if (!aData) // unknown case
      return aChanged;
    TDF_Label& aShapeLab = aData->shapeLab();
    EvolutionToSelection(aShapeLab, theFlag);
  }
  return aChanged;
}

void Model_ResultBody::store(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();   
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theShape)
      return;  // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull())
      return;  // null shape inside

    aBuilder.Generated(aShape);	
    // register name
    if(!aBuilder.NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder.NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder.NamedShape()->Label(), aName);
        }
      }
    }
  }
}

void Model_ResultBody::storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
  const std::shared_ptr<GeomAPI_Shape>& theToShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();   
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theFromShape || !theToShape)
      return;  // bad shape
    TopoDS_Shape aShapeBasis = theFromShape->impl<TopoDS_Shape>();
    if (aShapeBasis.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theToShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside
    aBuilder.Generated(aShapeBasis, aShapeNew);
    // register name
    if(!aBuilder.NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder.NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder.NamedShape()->Label(), aName);
        }
      }
    }
  }
}

void Model_ResultBody::storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const std::shared_ptr<GeomAPI_Shape>& theNewShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();   
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theOldShape || !theNewShape)
      return;  // bad shape
    TopoDS_Shape aShapeOld = theOldShape->impl<TopoDS_Shape>();
    if (aShapeOld.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theNewShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside
    aBuilder.Modify(aShapeOld, aShapeNew);
  }
}

std::shared_ptr<GeomAPI_Shape> Model_ResultBody::shape()
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    Handle(TNaming_NamedShape) aName;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
      TopoDS_Shape aShape = aName->Get();
      if (!aShape.IsNull()) {
        std::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
        aRes->setImpl(new TopoDS_Shape(aShape));
        return aRes;
      }
    }
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

void Model_ResultBody::clean()
{
  std::vector<TNaming_Builder*>::iterator aBuilder = myBuilders.begin();
  for(; aBuilder != myBuilders.end(); aBuilder++)
    delete *aBuilder;
  myBuilders.clear();
}

Model_ResultBody::~Model_ResultBody()
{
  clean();
}

TNaming_Builder* Model_ResultBody::builder(const int theTag)
{
  if (myBuilders.size() <= (unsigned int)theTag) {
    myBuilders.insert(myBuilders.end(), theTag - myBuilders.size() + 1, NULL);
  }
  if (!myBuilders[theTag]) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    myBuilders[theTag] = new TNaming_Builder(aData->shapeLab().FindChild(theTag));
    //TCollection_AsciiString entry;//
    //TDF_Tool::Entry(aData->shapeLab().FindChild(theTag), entry);
    //cout << "Label = " <<entry.ToCString() <<endl;
  }
  return myBuilders[theTag];
}

void Model_ResultBody::buildName(const int theTag, const std::string& theName)
{
  std::string aName = data()->name() + "/" + theName; 
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(document());
  aDoc->addNamingName(builder(theTag)->NamedShape()->Label(), aName);
  TDataStd_Name::Set(builder(theTag)->NamedShape()->Label(),aName.c_str());
}
void Model_ResultBody::generated(
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag)
{
  TopoDS_Shape aShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Generated(aShape);
  if(!theName.empty()) 
    buildName(theTag, theName);
}

void Model_ResultBody::generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Generated(anOldShape, aNewShape);
  if(!theName.empty()) 
    buildName(theTag, theName);
}


void Model_ResultBody::modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Modify(anOldShape, aNewShape);
  if(!theName.empty()) 
    buildName(theTag, theName);
}

void Model_ResultBody::deleted(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const int theTag)
{
  TopoDS_Shape aShape = theOldShape->impl<TopoDS_Shape>();
  builder(theTag)->Delete(aShape);
}

void Model_ResultBody::loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
  const int  theKindOfShape,
  const int  theTag)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  TopExp_Explorer ShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = ShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    std::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    if (theMS->isDeleted (aRShape)) {
      builder(theTag)->Delete(aRoot);
    }
  }
}

void Model_ResultBody::loadAndOrientModifiedShapes (
  GeomAlgoAPI_MakeShape* theMS,
  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
  const int  theKindOfShape,
  const int  theTag,
  const std::string& theName,
  GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  bool isBuilt = theName.empty();
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    ListOfShape aList;
    std::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    theMS->modified(aRShape, aList);
    std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>(); 	  
      if (theSubShapes.isBound(*anIt)) {
        std::shared_ptr<GeomAPI_Shape> aMapShape(theSubShapes.find(*anIt));
        aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) {
        builder(theTag)->Modify(aRoot,aNewShape);
        if(!isBuilt) 
          buildName(theTag, theName);		
      }
    }
  }
}

void Model_ResultBody::loadAndOrientGeneratedShapes (
  GeomAlgoAPI_MakeShape* theMS,
  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
  const int  theKindOfShape,
  const int  theTag,
  const std::string& theName,
  GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  bool isBuilt = theName.empty();
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    ListOfShape aList;
    std::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    theMS->generated(aRShape, aList);
    std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>(); 	  
      if (theSubShapes.isBound(*anIt)) {
        std::shared_ptr<GeomAPI_Shape> aMapShape(theSubShapes.find(*anIt));
        aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) {
        builder(theTag)->Generated(aRoot,aNewShape);
        if(!isBuilt) 
          buildName(theTag, theName);	
      }
    }
  }
}

//=======================================================================
int getDangleShapes(const TopoDS_Shape&           theShapeIn, 
  const TopAbs_ShapeEnum        theGeneratedFrom,
  TopTools_DataMapOfShapeShape& theDangles) 
{
  theDangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (theGeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (theGeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(theShapeIn, GeneratedTo, theGeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) theDangles.Bind(ancestors.First(), mayBeDangle);
  }
  return theDangles.Extent();
}

//=======================================================================
void loadGeneratedDangleShapes(
  const TopoDS_Shape&      theShapeIn,
  const TopAbs_ShapeEnum   theGeneratedFrom,
  TNaming_Builder *        theBuilder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!getDangleShapes(theShapeIn, theGeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next()) 
    theBuilder->Generated(itr.Key(), itr.Value());
}

//=======================================================================
void Model_ResultBody::loadNextLevels(std::shared_ptr<GeomAPI_Shape> theShape, 
  const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();    
  std::string aName;
  if (aShape.ShapeType() == TopAbs_SOLID) {		    
    TopExp_Explorer expl(aShape, TopAbs_FACE);
    for (; expl.More(); expl.Next()) {  
      builder(theTag)->Generated(expl.Current()); 
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
  else if (aShape.ShapeType() == TopAbs_SHELL || aShape.ShapeType() == TopAbs_FACE) {
    // load faces and all the free edges
    TopTools_IndexedMapOfShape Faces;
    TopExp::MapShapes(aShape, TopAbs_FACE, Faces);
    if (Faces.Extent() > 1 || (aShape.ShapeType() == TopAbs_SHELL && Faces.Extent() == 1)) {
      TopExp_Explorer expl(aShape, TopAbs_FACE);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());          
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
    }
    TopTools_IndexedDataMapOfShapeListOfShape anEdgeAndNeighbourFaces;
    TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, anEdgeAndNeighbourFaces);
    for (Standard_Integer i = 1; i <= anEdgeAndNeighbourFaces.Extent(); i++) 
    {
      const TopTools_ListOfShape& aLL = anEdgeAndNeighbourFaces.FindFromIndex(i);
      if (aLL.Extent() < 2) {
        if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeAndNeighbourFaces.FindKey(i))))
          continue;
        builder(theTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      } else {
        TopTools_ListIteratorOfListOfShape anIter(aLL);
        const TopoDS_Face& aFace = TopoDS::Face(anIter.Value());
        anIter.Next();
        if(aFace.IsEqual(anIter.Value())) {
          builder(theTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
          TCollection_AsciiString aStr(theTag);
          aName = theName + aStr.ToCString();
          buildName(theTag, aName);
          theTag++;
        }
      }
    }
  } else if (aShape.ShapeType() == TopAbs_WIRE) {
    TopTools_IndexedMapOfShape Edges;
    BRepTools::Map3DEdges(aShape, Edges);
    if (Edges.Extent() == 1) {
      builder(++theTag)->Generated(Edges.FindKey(1));
      TopExp_Explorer expl(aShape, TopAbs_VERTEX);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
    } else {
      TopExp_Explorer expl(aShape, TopAbs_EDGE); 
      for (; expl.More(); expl.Next()) {	
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }   
      // and load generated vertices.
      TopTools_DataMapOfShapeShape generated;
      if (getDangleShapes(aShape, TopAbs_EDGE, generated)) 
      {
        TNaming_Builder* pBuilder = builder(theTag++);
        loadGeneratedDangleShapes(aShape, TopAbs_EDGE, pBuilder);  
      }
    }
  } else if (aShape.ShapeType() == TopAbs_EDGE) {
    TopExp_Explorer expl(aShape, TopAbs_VERTEX);
    for (; expl.More(); expl.Next()) {      
      builder(theTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

//=======================================================================
int findAmbiguities(const TopoDS_Shape&           theShapeIn, 					
  TopTools_ListOfShape&   theList) 
{
  int aNumEdges(0);
  theList.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum aTS(TopAbs_EDGE);
  TopAbs_ShapeEnum aTA(TopAbs_FACE);
  TopTools_MapOfShape aMap1, aMap2; // map1 - for edge ancestors; map2 - for keys => edges
  TopTools_ListOfShape aKeyList;
  TopExp::MapShapesAndAncestors(theShapeIn, aTS, aTA, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& aKeyEdge1 = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors1 = subShapeAndAncestors.FindFromIndex(i);
    aMap1.Clear();
    TopTools_ListIteratorOfListOfShape it(ancestors1);
    for(;it.More();it.Next()) aMap1.Add(it.Value()); // fill map with key ancestors => aKey1
    for (Standard_Integer j = 1; j <= subShapeAndAncestors.Extent(); j++) {
      if (i == j) continue;
      const TopoDS_Shape& aKeyEdge2 = subShapeAndAncestors.FindKey(j);
      const TopTools_ListOfShape& ancestors2 = subShapeAndAncestors.FindFromIndex(j);
      if(ancestors1.Extent() == ancestors2.Extent() && ancestors1.Extent() > 1) {
        int aNum (ancestors2.Extent());
        TopTools_ListIteratorOfListOfShape it(ancestors2);
        for(;it.More();it.Next()) 
          if(aMap1.Contains(it.Value())) aNum--;
        if(aNum == 0) {
          if(aMap2.Add(aKeyEdge1)) 
            aKeyList.Append(aKeyEdge1);
          if(aMap2.Add(aKeyEdge2))
            aKeyList.Append(aKeyEdge2);
        }
      }
    } // at the end ==> List of edges to be named in addition	
  }
  aNumEdges = aKeyList.Extent();
  if(aNumEdges)
    theList.Assign(aKeyList);	
  return aNumEdges; 
}

//=======================================================================
void Model_ResultBody::loadFirstLevel(
  std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>(); 
  std::string aName;
  if (aShape.ShapeType() == TopAbs_COMPOUND || aShape.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(aShape);
    for (; itr.More(); itr.Next(),theTag++) {
      builder(theTag)->Generated(itr.Value());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      if(!theName.empty()) buildName(theTag, aName);
      if (itr.Value().ShapeType() == TopAbs_COMPOUND || 
        itr.Value().ShapeType() == TopAbs_COMPSOLID) 
      {
        std::shared_ptr<GeomAPI_Shape> itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadFirstLevel(itrShape, theName, theTag);
      } else {
        std::shared_ptr<GeomAPI_Shape> itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));		
        loadNextLevels(itrShape, theName, theTag);
      }
    }
  } else {
    std::shared_ptr<GeomAPI_Shape> itrShape(new GeomAPI_Shape());
    itrShape->setImpl(new TopoDS_Shape(aShape));
    loadNextLevels(itrShape, theName, theTag); 
  }
  TopTools_ListOfShape   aList;
  if(findAmbiguities(aShape, aList)) {
    TopTools_ListIteratorOfListOfShape it(aList);
    for (; it.More(); it.Next(),theTag++) {
      builder(theTag)->Generated(it.Value());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
    }
  }
}

//=======================================================================
void Model_ResultBody::loadDisconnectedEdges(
  std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();  
  TopTools_DataMapOfShapeListOfShape edgeNaborFaces;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(aShape, TopAbs_FACE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer explV(aFace, TopAbs_EDGE);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& anEdge = explV.Current();
      if (!edgeNaborFaces.IsBound(anEdge)) edgeNaborFaces.Bind(anEdge, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(edgeNaborFaces.Find(anEdge));
      for (; itrF.More(); itrF.Next()) {
        if (itrF.Value().IsSame(aFace)) {
          faceIsNew = Standard_False;
          break;
        }
      }
      if (faceIsNew) 
        edgeNaborFaces.ChangeFind(anEdge).Append(aFace);      
    }
  }

  /*  TopTools_IndexedDataMapOfShapeListOfShape aDM;
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, aDM);
  for(int i=1; i <= aDM.Extent(); i++) {
  if(aDM.FindFromIndex(i).Extent() > 1) continue;
  if (BRep_Tool::Degenerated(TopoDS::Edge(aDM.FindKey(i))))
  continue;
  builder(theTag)->Generated(aDM.FindKey(i));
  TCollection_AsciiString aStr(theTag);
  std::string aName = theName + aStr.ToCString();
  buildName(theTag, aName);
  #ifdef DEB_IMPORT
  aName +=  + ".brep";
  BRepTools::Write(aDM.FindKey(i), aName.c_str());
  #endif
  theTag++;
  }
  */
  TopTools_MapOfShape anEdgesToDelete;
  TopExp_Explorer anEx(aShape,TopAbs_EDGE); 
  std::string aName;
  for(;anEx.More();anEx.Next()) {
    Standard_Boolean aC0 = Standard_False;
    TopoDS_Shape anEdge1 = anEx.Current();
    if (edgeNaborFaces.IsBound(anEdge1)) {
      const TopTools_ListOfShape& aList1 = edgeNaborFaces.Find(anEdge1);
      if (aList1.Extent()<2) continue;
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(edgeNaborFaces);
      for (; itr.More(); itr.Next()) {
        TopoDS_Shape anEdge2 = itr.Key();
        if(anEdgesToDelete.Contains(anEdge2)) continue;
        if (anEdge1.IsSame(anEdge2)) continue;
        const TopTools_ListOfShape& aList2 = itr.Value();
        // compare lists of the neighbour faces of edge1 and edge2
        if (aList1.Extent() == aList2.Extent()) {
          Standard_Integer aMatches = 0;
          for(TopTools_ListIteratorOfListOfShape aLIter1(aList1);aLIter1.More();aLIter1.Next())
            for(TopTools_ListIteratorOfListOfShape aLIter2(aList2);aLIter2.More();aLIter2.Next())
              if (aLIter1.Value().IsSame(aLIter2.Value())) aMatches++;
          if (aMatches == aList1.Extent()) {
            aC0=Standard_True;
            builder(theTag)->Generated(anEdge2);
            anEdgesToDelete.Add(anEdge2);
            TCollection_AsciiString aStr(theTag);
            aName = theName + aStr.ToCString();
            buildName(theTag, aName);
            theTag++;
          }
        }
      }      
      TopTools_MapIteratorOfMapOfShape itDelete(anEdgesToDelete);
      for(;itDelete.More();itDelete.Next()) 
        edgeNaborFaces.UnBind(itDelete.Key());      
      edgeNaborFaces.UnBind(anEdge1);
    }
    if (aC0) {
      builder(theTag)->Generated(anEdge1);
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);	 
      theTag++;
    }
  }  
}

void Model_ResultBody::loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();  
  TopTools_DataMapOfShapeListOfShape vertexNaborEdges;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(aShape, TopAbs_EDGE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& anEdge = explF.Current();
    TopExp_Explorer explV(anEdge, TopAbs_VERTEX);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& aVertex = explV.Current();
      if (!vertexNaborEdges.IsBound(aVertex)) vertexNaborEdges.Bind(aVertex, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(vertexNaborEdges.Find(aVertex));
      for (; itrF.More(); itrF.Next()) {
        if (itrF.Value().IsSame(anEdge)) {
          faceIsNew = Standard_False;
          break;
        }
      }
      if (faceIsNew) {
        vertexNaborEdges.ChangeFind(aVertex).Append(anEdge);
      }
    }
  }
  std::string aName;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(vertexNaborEdges);
  for (; itr.More(); itr.Next()) {
    const TopTools_ListOfShape& naborEdges = itr.Value();
    if (naborEdges.Extent() < 2) {		
      builder(theTag)->Generated(itr.Key());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);	 
      theTag++;
    }
  }
}
