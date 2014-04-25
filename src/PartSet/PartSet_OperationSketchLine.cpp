// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchLine.h>

#include <SketchPlugin_Feature.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketchLine::PartSet_OperationSketchLine(const QString& theId,
	                                          QObject* theParent,
                                              boost::shared_ptr<ModelAPI_Feature> theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature)
{
}

PartSet_OperationSketchLine::~PartSet_OperationSketchLine()
{
}

bool PartSet_OperationSketchLine::isPerformedImmediately() const
{
  return false;
}

int PartSet_OperationSketchLine::getSelectionMode() const
{
  return TopAbs_FACE;
}

void PartSet_OperationSketchLine::setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList)
{
  if (theList.IsEmpty())
    return;
}

void PartSet_OperationSketchLine::startOperation()
{
  PartSet_OperationSketchBase::startOperation();

  if (mySketch) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(mySketch);

    aFeature->addSub(feature());
  }
}
