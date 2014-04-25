// File:        PartSet_OperationSketchBase.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchBase.h>

#include <SketchPlugin_Feature.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketchBase::PartSet_OperationSketchBase(const QString& theId,
	                                                     QObject* theParent)
: ModuleBase_PropPanelOperation(theId, theParent)
{
}

PartSet_OperationSketchBase::~PartSet_OperationSketchBase()
{
}

boost::shared_ptr<GeomAPI_Shape> PartSet_OperationSketchBase::preview() const
{
  boost::shared_ptr<SketchPlugin_Feature> aFeature = 
    boost::dynamic_pointer_cast<SketchPlugin_Feature>(feature());
  return aFeature->preview();
}
