// File:        PartSet_OperationSketchBase.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchBase.h>

#include <SketchPlugin_Feature.h>
#include <Model_Object.h>

#include <V3d_View.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketchBase::PartSet_OperationSketchBase(const QString& theId,
	                                                     QObject* theParent)
: ModuleBase_Operation(theId, theParent)
{
}

PartSet_OperationSketchBase::~PartSet_OperationSketchBase()
{
}

boost::shared_ptr<GeomAPI_Shape> PartSet_OperationSketchBase::preview(
                                      boost::shared_ptr<ModelAPI_Feature> theFeature) const
{
  boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                              boost::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
  if (!aFeature) { // if it is reference to a object feature
    boost::shared_ptr<Model_Object> anObj = 
      boost::dynamic_pointer_cast<Model_Object>(theFeature);
    if (anObj) 
      aFeature = boost::dynamic_pointer_cast<SketchPlugin_Feature>(anObj->featureRef());
  }
  return aFeature->preview();
}

std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >
                                                     PartSet_OperationSketchBase::preview() const
{
  return std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >();
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationSketchBase::createFeature()
{
  boost::shared_ptr<ModelAPI_Feature> aFeature = ModuleBase_Operation::createFeature();
  emit featureConstructed(aFeature, FM_Activation);
  return aFeature;
}


void PartSet_OperationSketchBase::mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                                               const std::list<XGUI_ViewerPrs>& theSelected)
{
}
void PartSet_OperationSketchBase::mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                                                const std::list<XGUI_ViewerPrs>& theSelected)
{
}
void PartSet_OperationSketchBase::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
}
