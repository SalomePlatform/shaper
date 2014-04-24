// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>

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
