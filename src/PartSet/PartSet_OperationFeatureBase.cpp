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
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_IViewer.h"

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
                                                               CompositeFeaturePtr theFeature)
    : PartSet_OperationSketchBase(theId, theParent),
      mySketch(theFeature)
{
}

PartSet_OperationFeatureBase::~PartSet_OperationFeatureBase()
{
}

CompositeFeaturePtr PartSet_OperationFeatureBase::sketch() const
{
  return mySketch;
}

void PartSet_OperationFeatureBase::mouseReleased(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
                                                 ModuleBase_ISelection* theSelection)
{
  Handle(V3d_View) aView = theViewer->activeView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
  double aX = aPoint.X(), anY = aPoint.Y();
  QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();

  if (aSelected.empty()) {
    PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aX, anY);
  } else {
    ModuleBase_ViewerPrs aPrs = aSelected.first();
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) { // a point is selected
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          aPoint = BRep_Tool::Pnt(aVertex);
          PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aX, anY);
          ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
          PartSet_Tools::setConstraints(sketch(), feature(), aActiveWgt->attributeID(), aX, anY);
        }
      } else if (aShape.ShapeType() == TopAbs_EDGE) { // a line is selected
        PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aX, anY);
      }
    }
  }
  ObjectPtr aFeature;
  if (!aSelected.empty()) {
    ModuleBase_ViewerPrs aPrs = aSelected.first();
    aFeature = aPrs.object();
  } else {
    aFeature = feature();  // for the widget distance only
  }

  bool isApplyed = setWidgetValue(aFeature, aX, anY);
  if (isApplyed) {
    flushUpdated();
    myPropertyPanel->activateNextWidget();
  }
  // the operation can be committed only when there is no an active widget anymore
  // if this check is absent, the edit operation for constraint perpendicular is stopped
  // after the first object selection in the viewer(there are two objects to be selected)
  // the second case is the constraint distance, the edit is stopped after any mouse click
  // in the viewer whenever it is applyed or not to the selection control
  if (!myPropertyPanel->activeWidget())
    commit();
}

/*bool PartSet_OperationFeatureBase::setWidgetValue(ObjectPtr theFeature, double theX, double theY)
{
  ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
  if (!aActiveWgt)
    return false;
  ModuleBase_WidgetValueFeature* aValue = new ModuleBase_WidgetValueFeature();
  aValue->setObject(theFeature);
  aValue->setPoint(boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY)));
  bool isApplyed = aActiveWgt->setValue(aValue);

  delete aValue;
  myIsModified = (myIsModified || isApplyed);
  return isApplyed;
}*/
