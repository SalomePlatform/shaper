// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>

#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketch::PartSet_OperationSketch(const QString& theId,
	                                             QObject* theParent)
: PartSet_OperationSketchBase(theId, theParent)
{
}

PartSet_OperationSketch::~PartSet_OperationSketch()
{
}

bool PartSet_OperationSketch::isPerformedImmediately() const
{
  return false;
}

int PartSet_OperationSketch::getSelectionMode() const
{
  return TopAbs_FACE;
}

void PartSet_OperationSketch::setSelectedObjects(const AIS_ListOfInteractive& theList)
{
  if (theList.IsEmpty())
    return;

  // 1. get selected fase
  Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(theList.First());
  if (anAISShape.IsNull())
    return;

  const TopoDS_Shape& aShape = anAISShape->Shape();
  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aShape));

  // get plane parameters
  double anX = 1, anY = 0, aZ = 0, anOrigin = 0;

  // set plane parameters to feature
  //boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  //boost::shared_ptr<ModelAPI_AttributeDocRef> anAttr = aData->docRef(SKETCH_ATTR_X);
  //anAttr->setValue(anX);

  commit();
}
