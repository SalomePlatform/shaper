// File:        PartSet_OperationConstraint.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationConstraint.h>

#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>

#include <GeomDataAPI_Point2D.h>

#include <ModuleBase_OperationDescription.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <SketchPlugin_Constraint.h>

#include <Geom_Line.hxx>
#include <gp_Lin.hxx>

#include <XGUI_ViewerPrs.h>
#include <XGUI_Constants.h>

#include <SketchPlugin_Line.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationConstraint::PartSet_OperationConstraint(const QString& theId,
	                                          QObject* theParent,
                                              FeaturePtr theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature)
{
}

PartSet_OperationConstraint::~PartSet_OperationConstraint()
{
}

bool PartSet_OperationConstraint::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

void PartSet_OperationConstraint::init(FeaturePtr theFeature,
                                       const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                       const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  //if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
  //  return;
  // use the last point of the previous feature as the first of the new one
  //boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  //myInitPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));
}

FeaturePtr PartSet_OperationConstraint::sketch() const
{
  return mySketch;
}

void PartSet_OperationConstraint::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                const std::list<XGUI_ViewerPrs>& theSelected,
                                                const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  if (theSelected.empty()) {
  }
  else {
    XGUI_ViewerPrs aPrs = theSelected.front();
    FeaturePtr aFeature = aPrs.feature();

    setFeature(aFeature);
    setValue(120);
    flushUpdated();
  }
}

void PartSet_OperationConstraint::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
}

void PartSet_OperationConstraint::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //setPointSelectionMode(!myInitPoint ? SM_FirstPoint : SM_SecondPoint);

  emit multiSelectionEnabled(false);
}

void PartSet_OperationConstraint::abortOperation()
{
  emit featureConstructed(feature(), FM_Hide);
  PartSet_OperationSketchBase::abortOperation();
}

void PartSet_OperationConstraint::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit multiSelectionEnabled(true);
}

void PartSet_OperationConstraint::afterCommitOperation()
{
  PartSet_OperationSketchBase::afterCommitOperation();  
  emit featureConstructed(feature(), FM_Deactivation);
}

FeaturePtr PartSet_OperationConstraint::createFeature(const bool theFlushMessage)
{
  FeaturePtr aNewFeature = ModuleBase_Operation::createFeature(false);
  if (sketch()) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(sketch());

    aFeature->addSub(aNewFeature);
  }
  /*if (myInitPoint) {
    setLinePoint(aNewFeature, myInitPoint->x(), myInitPoint->y(), LINE_ATTR_START);
    setLinePoint(aNewFeature, myInitPoint->x(), myInitPoint->y(), LINE_ATTR_END);

    boost::shared_ptr<ModelAPI_Data> aData = aNewFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                (aData->attribute(LINE_ATTR_START));
    PartSet_Tools::createConstraint(myInitPoint, aPoint);
  }*/

  emit featureConstructed(aNewFeature, FM_Activation);
  if (theFlushMessage)
    flushCreated();
  return aNewFeature;
}

void PartSet_OperationConstraint::setFeature(FeaturePtr theFeature)
{
  if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
    return;

  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  anAttr->setFeature(theFeature);
}

void PartSet_OperationConstraint::setValue(const double theValue)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();

  AttributeDoublePtr anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  anAttr->setValue(theValue);

}
