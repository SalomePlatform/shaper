// File:        Model_ResultBody.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultBody.h>
#include <Model_Data.h>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_ChildIterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#include <TDF_Tool.hxx>
Model_ResultBody::Model_ResultBody()
{
  setIsConcealed(false);
}

void Model_ResultBody::store(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(data());
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
  }
}

void Model_ResultBody::storeGenerated(const boost::shared_ptr<GeomAPI_Shape>& theFromShape,
	                                  const boost::shared_ptr<GeomAPI_Shape>& theToShape)
{
  boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(data());
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
  }
}

void Model_ResultBody::storeModified(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
	                                  const boost::shared_ptr<GeomAPI_Shape>& theNewShape)
{
  boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(data());
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
    aBuilder.Generated(aShapeOld, aShapeNew);
  }
}

boost::shared_ptr<GeomAPI_Shape> Model_ResultBody::shape()
{
  boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    Handle(TNaming_NamedShape) aName;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
      TopoDS_Shape aShape = aName->Get();
      if (!aShape.IsNull()) {
        boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
        aRes->setImpl(new TopoDS_Shape(aShape));
        return aRes;
      }
    }
  }
  return boost::shared_ptr<GeomAPI_Shape>();
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
    boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(data());
    myBuilders[theTag] = new TNaming_Builder(aData->shapeLab().FindChild(theTag));
	//TCollection_AsciiString entry;//
	//TDF_Tool::Entry(aData->shapeLab().FindChild(theTag), entry);
	//cout << "Label = " <<entry.ToCString() <<endl;
  }
  return myBuilders[theTag];
}

void Model_ResultBody::generated(
  const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag)
{
  TopoDS_Shape aShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Generated(aShape);
}

void Model_ResultBody::generated(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Generated(anOldShape, aNewShape);
}


void Model_ResultBody::modified(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Modify(anOldShape, aNewShape);
}

void Model_ResultBody::deleted(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const int theTag)
{
  TopoDS_Shape aShape = theOldShape->impl<TopoDS_Shape>();
  builder(theTag)->Delete(aShape);
}

void Model_ResultBody::loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                               boost::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  TopExp_Explorer ShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = ShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
	boost::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
	aRShape->setImpl((new TopoDS_Shape(aRoot)));
    if (theMS->isDeleted (aRShape)) {
		builder(theTag)->Delete(aRoot);
    }
  }
}

void Model_ResultBody::loadAndOrientModifiedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               boost::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
	ListOfShape aList;
	boost::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
	aRShape->setImpl((new TopoDS_Shape(aRoot)));
	theMS->generated(aRShape, aList);
	std::list<boost::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>(); 	  
	  if (theSubShapes.isBound(*anIt)) {
		boost::shared_ptr<GeomAPI_Shape> aMapShape(theSubShapes.find(*anIt));
		aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) 
		  builder(theTag)->Modify(aRoot,aNewShape);
    }
  }
}

void Model_ResultBody::loadAndOrientGeneratedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               boost::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
	ListOfShape aList;
	boost::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
	aRShape->setImpl((new TopoDS_Shape(aRoot)));
	theMS->generated(aRShape, aList);
	std::list<boost::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>(); 	  
	  if (theSubShapes.isBound(*anIt)) {
		boost::shared_ptr<GeomAPI_Shape> aMapShape(theSubShapes.find(*anIt));
		aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) 
		  builder(theTag)->Generated(aRoot,aNewShape);
    }
  }
}