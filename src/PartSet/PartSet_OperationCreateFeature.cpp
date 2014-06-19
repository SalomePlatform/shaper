// File:        PartSet_OperationCreateFeature.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationCreateFeature.h>

#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_FeaturePointPrs.h>
#include <PartSet_FeatureLinePrs.h>
#include <PartSet_FeatureCirclePrs.h>
#include <PartSet_FeatureArcPrs.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>

#include <ModuleBase_OperationDescription.h>

#include <XGUI_ViewerPrs.h>
#include <XGUI_Constants.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationCreateFeature::PartSet_OperationCreateFeature(const QString& theId,
                                                               QObject* theParent,
                                                               FeaturePtr theFeature)
: PartSet_OperationSketchBase(theId, theParent),
  myPointSelectionMode(SM_FirstPoint)
{
  std::string aKind = theId.toStdString();
  myFeaturePrs = PartSet_Tools::createFeaturePrs(aKind, theFeature);
}

PartSet_OperationCreateFeature::~PartSet_OperationCreateFeature()
{
}

bool PartSet_OperationCreateFeature::canProcessKind(const std::string& theId)
{
  return theId == SKETCH_LINE_KIND || theId == SKETCH_POINT_KIND || theId == SKETCH_CIRCLE_KIND ||
         theId == SKETCH_ARC_KIND;
}

bool PartSet_OperationCreateFeature::canBeCommitted() const
{
  return myPointSelectionMode == SM_DonePoint;
}

bool PartSet_OperationCreateFeature::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

std::list<int> PartSet_OperationCreateFeature::getSelectionModes(FeaturePtr theFeature) const
{
  std::list<int> aModes;
  if (theFeature != feature())
    aModes = PartSet_OperationSketchBase::getSelectionModes(theFeature);
  return aModes;
}

void PartSet_OperationCreateFeature::init(FeaturePtr theFeature,
                                       const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                       const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
    return;
  myInitFeature = theFeature;
}

FeaturePtr PartSet_OperationCreateFeature::sketch() const
{
  return myFeaturePrs->sketch();
}

void PartSet_OperationCreateFeature::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                const std::list<XGUI_ViewerPrs>& theSelected,
                                                const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  if (myPointSelectionMode == SM_DonePoint)
  {
    // if the point creation is finished, the next mouse release should commit the modification
    // the next release can happens by double click in the viewer
    commit();
    restartOperation(feature()->getKind(), feature());
    return;
  }

  double aX, anY;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
  if (theSelected.empty()) {
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
  }
  else {
    XGUI_ViewerPrs aPrs = theSelected.front();
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) // the point is selected
    {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          aPoint = BRep_Tool::Pnt(aVertex);
          PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);

          myFeaturePrs->setConstraints(aX, anY, myPointSelectionMode);
        }
      }
      else if (aShape.ShapeType() == TopAbs_EDGE) // the line is selected
      {
        PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
        // move to selected line
        if (feature()->getKind() == SKETCH_LINE_KIND) {
          boost::shared_ptr<PartSet_FeatureLinePrs> aLinePrs =
                                 boost::dynamic_pointer_cast<PartSet_FeatureLinePrs>(myFeaturePrs);
          if (aLinePrs) {
            FeaturePtr aFeature = aPrs.feature();
            aLinePrs->projectPointOnLine(aFeature, myPointSelectionMode, aPoint, theView, aX, anY);
          }
        }
      }
    }
  }

  switch (myPointSelectionMode)
  {
    case SM_FirstPoint:
    case SM_SecondPoint:
    case SM_ThirdPoint: {
      if (feature()->getKind() == SKETCH_ARC_KIND) {
        boost::shared_ptr<PartSet_FeatureArcPrs> anArcPrs =
                                 boost::dynamic_pointer_cast<PartSet_FeatureArcPrs>(myFeaturePrs);
        if (anArcPrs) {
          anArcPrs->projectPointOnArc(aPoint, theView, aX, anY);
        }
      }
      PartSet_SelectionMode aMode = myFeaturePrs->setPoint(aX, anY, myPointSelectionMode);
      flushUpdated();
      setPointSelectionMode(aMode);
    }
    break;
    default:
      break;
  }
}

void PartSet_OperationCreateFeature::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  switch (myPointSelectionMode)
  {
    case SM_FirstPoint:
    case SM_SecondPoint:
    case SM_ThirdPoint:
    {
      double aX, anY;
      gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
      PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
      if (myPointSelectionMode == SM_ThirdPoint) {
        if (feature()->getKind() == SKETCH_ARC_KIND) {
          boost::shared_ptr<PartSet_FeatureArcPrs> anArcPrs =
                                 boost::dynamic_pointer_cast<PartSet_FeatureArcPrs>(myFeaturePrs);
          if (anArcPrs) {
            anArcPrs->projectPointOnArc(aPoint, theView, aX, anY);
          }
        }
      }
      myFeaturePrs->setPoint(aX, anY, myPointSelectionMode);

      flushUpdated();
      emit focusActivated(myFeaturePrs->getAttribute(myPointSelectionMode));
    }
    break;
    case SM_DonePoint:
    {
      commit();
      restartOperation(feature()->getKind(), feature());
    }
    default:
      break;
  }
}

void PartSet_OperationCreateFeature::keyReleased(std::string theName, QKeyEvent* theEvent)
{
  int aKeyType = theEvent->key();
  // the second point should be activated by any modification in the property panel
  if (!theName.empty() /*&& aKeyType == Qt::Key_Return*/)
  {
    setPointSelectionMode(myFeaturePrs->getNextMode(theName), false);
  }
  keyReleased(theEvent->key());
}

void PartSet_OperationCreateFeature::keyReleased(const int theKey)
{
  switch (theKey) {
    case Qt::Key_Return: {
      if (myPointSelectionMode == SM_DonePoint)
      {
        commit();
        // it start a new line creation at a free point
        restartOperation(feature()->getKind(), FeaturePtr()/*feature()*/);
      }
      //else
      //  abort();
      //restartOperation(feature()->getKind(), FeaturePtr());
    }
    break;
    case Qt::Key_Escape: {
      if (myPointSelectionMode == SM_DonePoint)
      {
        commit();
      }
      else
      {
        abort();
      }
    }
    default:
    break;
  }
}

void PartSet_OperationCreateFeature::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  setPointSelectionMode(!myInitFeature ? SM_FirstPoint : SM_SecondPoint);

  emit multiSelectionEnabled(false);
}

void PartSet_OperationCreateFeature::abortOperation()
{
  emit featureConstructed(feature(), FM_Hide);
  PartSet_OperationSketchBase::abortOperation();
}

void PartSet_OperationCreateFeature::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit multiSelectionEnabled(true);
}

void PartSet_OperationCreateFeature::afterCommitOperation()
{
  PartSet_OperationSketchBase::afterCommitOperation();  
  emit featureConstructed(feature(), FM_Deactivation);
}

FeaturePtr PartSet_OperationCreateFeature::createFeature(const bool theFlushMessage)
{
  FeaturePtr aNewFeature = ModuleBase_Operation::createFeature(false);
  if (sketch()) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(sketch());

    aFeature->addSub(aNewFeature);
  }
  myFeaturePrs->init(aNewFeature);
  myFeaturePrs->setFeature(myInitFeature, SM_FirstPoint);

  emit featureConstructed(aNewFeature, FM_Activation);
  if (theFlushMessage)
    flushCreated();
  return aNewFeature;
}

void PartSet_OperationCreateFeature::setPointSelectionMode(const PartSet_SelectionMode& theMode,
                                                           const bool isToEmitSignal)
{
  myPointSelectionMode = theMode;
  if (isToEmitSignal) {
    std::string aName = myFeaturePrs->getAttribute(theMode);
    if (aName.empty() && theMode == SM_DonePoint) {
      aName = XGUI::PROP_PANEL_OK;
    }
    emit focusActivated(aName);
  }
}
