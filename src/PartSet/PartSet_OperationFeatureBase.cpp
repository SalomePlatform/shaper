// File:        PartSet_OperationFeatureBase.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureBase.h>

#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintCoincidence.h>

#include <GeomAPI_Pnt2d.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetPoint2D.h>
#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_ViewerPrs.h>

#include <XGUI_Constants.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationFeatureBase::PartSet_OperationFeatureBase(const QString& theId,
                                                               QObject* theParent,
                                                               FeaturePtr theFeature)
    : PartSet_OperationSketchBase(theId, theParent),
      mySketch(theFeature),
      myActiveWidget(NULL)
{
}

PartSet_OperationFeatureBase::~PartSet_OperationFeatureBase()
{
}

void PartSet_OperationFeatureBase::initSelection(
    const std::list<ModuleBase_ViewerPrs>& theSelected,
    const std::list<ModuleBase_ViewerPrs>& /*theHighlighted*/)
{
  myPreSelection = theSelected;
}

void PartSet_OperationFeatureBase::initFeature(FeaturePtr theFeature)
{
  myInitFeature = theFeature;
}

FeaturePtr PartSet_OperationFeatureBase::sketch() const
{
  return mySketch;
}

void PartSet_OperationFeatureBase::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                 const std::list<ModuleBase_ViewerPrs>& theSelected,
                                                 const std::list<ModuleBase_ViewerPrs>& /*theHighlighted*/)
{
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
  double aX = aPoint.X(), anY = aPoint.Y();

  if (theSelected.empty()) {
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
  } else {
    ModuleBase_ViewerPrs aPrs = theSelected.front();
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) { // a point is selected
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          aPoint = BRep_Tool::Pnt(aVertex);
          PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
          PartSet_Tools::setConstraints(sketch(), feature(), myActiveWidget->attributeID(), aX, anY);
        }
      } else if (aShape.ShapeType() == TopAbs_EDGE) { // a line is selected
        PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
      }
    }
  }
  ObjectPtr aFeature;
  if (!theSelected.empty()) {
    ModuleBase_ViewerPrs aPrs = theSelected.front();
    aFeature = aPrs.object();
  } else {
    aFeature = feature();  // for the widget distance only
  }

  bool isApplyed = setWidgetValue(aFeature, aX, anY);
  if (isApplyed) {
    flushUpdated();
    emit activateNextWidget(myActiveWidget);
  }
  commit();
}

void PartSet_OperationFeatureBase::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  myActiveWidget = theWidget;
  activateByPreselection();
  if (myInitFeature && myActiveWidget) {
    ModuleBase_WidgetPoint2D* aWgt = dynamic_cast<ModuleBase_WidgetPoint2D*>(myActiveWidget);
    if (aWgt && aWgt->initFromPrevious(myInitFeature)) {
      myInitFeature = FeaturePtr();
      emit activateNextWidget(myActiveWidget);
    }
  }
}

void PartSet_OperationFeatureBase::activateByPreselection()
{
  if ((myPreSelection.size() > 0) && myActiveWidget) {
    const ModuleBase_ViewerPrs& aPrs = myPreSelection.front();
    ModuleBase_WidgetValueFeature aValue;
    aValue.setObject(aPrs.object());
    if (myActiveWidget->setValue(&aValue)) {
      myPreSelection.remove(aPrs);
      if(isValid()) {
        myActiveWidget = NULL;
        commit();
      } else {
        emit activateNextWidget(myActiveWidget);
      }
    }
    // If preselection is enough to make a valid feature - apply it immediately
  }
}

bool PartSet_OperationFeatureBase::setWidgetValue(ObjectPtr theFeature, double theX, double theY)
{
  if (!myActiveWidget)
    return false;
  ModuleBase_WidgetValueFeature* aValue = new ModuleBase_WidgetValueFeature();
  aValue->setObject(theFeature);
  aValue->setPoint(boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY)));
  bool isApplyed = myActiveWidget->setValue(aValue);

  delete aValue;
  myIsModified = (myIsModified || isApplyed);
  return isApplyed;
}
