// File:        PartSet_OperationFeatureEditMulti.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureEditMulti.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_ISelection.h>

#include <ModelAPI_Events.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <ModelAPI_Events.h>

#include <Events_Loop.h>

#include <SketchPlugin_Line.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#include <QMouseEvent>
#ifdef _DEBUG
#include <QDebug>
#endif

//using namespace std;

PartSet_OperationFeatureEditMulti::PartSet_OperationFeatureEditMulti(const QString& theId,
                                                                     QObject* theParent,
                                                                     CompositeFeaturePtr theFeature)
    : PartSet_OperationSketchBase(theId, theParent),
      mySketch(theFeature),
      myIsBlockedSelection(false)
{
  myIsEditing = true;
}

PartSet_OperationFeatureEditMulti::~PartSet_OperationFeatureEditMulti()
{
}


bool isContains(const QList<ModuleBase_ViewerPrs>& theSelected, const ModuleBase_ViewerPrs& thePrs)
{
  foreach (ModuleBase_ViewerPrs aPrs, theSelected) {
    if (aPrs.object() == thePrs.object())
      return true;
  }
  return false;
}


void PartSet_OperationFeatureEditMulti::initSelection(ModuleBase_ISelection* theSelection,
                                                      ModuleBase_IViewer* theViewer)
{
  QList<ModuleBase_ViewerPrs> aFeatures;
  aFeatures = theSelection->getSelected();
  QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();
  // add highlighted elements if they are not selected
  foreach (ModuleBase_ViewerPrs aPrs, aHighlighted) {
    if (!isContains(aFeatures, aPrs))
      aFeatures.append(aPrs);
  }

  // firstly iterate the operation presentations and move all features
  // if there is no selected vertex shape for it. Create a set of moved features
  myFeature2Attribute.clear();
  // firstly, collect the features without local selection
  std::set<FeaturePtr> aMovedFeatures;
  foreach (ModuleBase_ViewerPrs aPrs, aFeatures) {
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX) { // a point is selected
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
      if (!aVertex.IsNull()) {
        continue;
      }
    }
    else {
      ObjectPtr aObject = aPrs.object();
      if (!aObject)
        continue;
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (aFeature && myFeature2Attribute.find(aFeature) == myFeature2Attribute.end()) {
        std::list<std::string> aList;
        myFeature2Attribute[aFeature] = aList;
      }
    }
  }
  // secondly, collect the features with a local selection on them
  // if the list already has this feature, the local selection is skipped
  Handle(V3d_View) aView = theViewer->activeView();
  foreach (ModuleBase_ViewerPrs aPrs, aFeatures) {
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX) { // a point is selected
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
      if (aVertex.IsNull())
        continue;
      ObjectPtr aObject = aPrs.object();
      if (!aObject)
        continue;
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (!aFeature)
        continue;
      // if the feature is already moved, do nothing for this feature local selection

      if (myFeature2Attribute.find(aFeature) != myFeature2Attribute.end())
        continue;

      gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
      double aVX, aVY;
      PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aVX, aVY);

      boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D = PartSet_Tools::getFeaturePoint(
                                                                    aFeature, aVX, aVY);
      std::string anAttribute = aFeature->data()->id(aPoint2D);
      std::list<std::string> aList;
      if (myFeature2Attribute.find(aFeature) != myFeature2Attribute.end())
        aList = myFeature2Attribute[aFeature];

      aList.push_back(anAttribute);
      myFeature2Attribute[aFeature] = aList;
    }
  }
}

CompositeFeaturePtr PartSet_OperationFeatureEditMulti::sketch() const
{
  return mySketch;
}

//void PartSet_OperationFeatureEditMulti::mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection)
//{
//}

void PartSet_OperationFeatureEditMulti::mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer)
{
  if (!(theEvent->buttons() & Qt::LeftButton))
    return;

  if (theViewer->isSelectionEnabled())
    theViewer->enableSelection(false);

  Handle(V3d_View) aView = theViewer->activeView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);

  //blockSelection(true);
  if (myCurPoint.myIsInitialized) {
    double aCurX, aCurY;
    PartSet_Tools::convertTo2D(myCurPoint.myPoint, sketch(), aView, aCurX, aCurY);

    double aX, anY;
    PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aX, anY);

    double aDeltaX = aX - aCurX;
    double aDeltaY = anY - aCurY;

    std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
    while (aFeatIter != myFeature2Attribute.end()) {
      FeaturePtr aFeature = aFeatIter->first;
      std::list<std::string> anAttributes = aFeatIter->second;
      if (anAttributes.empty()) {
        boost::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                                       boost::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature) {
          aSketchFeature->move(aDeltaX, aDeltaY);
        }
      }
      else {
        std::list<std::string>::const_iterator anAttrIter = anAttributes.begin(),
                                               anAttrEnd = anAttributes.end();
        for (; anAttrIter != anAttrEnd; anAttrIter++) {
          boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr = boost::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(aFeature->data()->attribute(*anAttrIter));
          if (aPointAttr) {
            aPointAttr->move(aDeltaX, aDeltaY);
          }
        }      
      }
      aFeatIter++;
    }
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationFeatureEditMulti::mouseReleased(
    QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
    ModuleBase_ISelection* theSelection)
{
  theViewer->enableSelection(true);
  if (commit()) {
    std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
    while (aFeatIter != myFeature2Attribute.end()) {
      FeaturePtr aFeature = aFeatIter->first;
      if (aFeature) {
        emit featureConstructed(aFeature, FM_Deactivation);
      }
      aFeatIter++;
    }
  }
}

void PartSet_OperationFeatureEditMulti::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //emit multiSelectionEnabled(false);

  //blockSelection(true);

  myCurPoint.clear();
}

void PartSet_OperationFeatureEditMulti::stopOperation()
{
  //emit multiSelectionEnabled(true);

  //blockSelection(false, true);

  myFeature2Attribute.clear();
}

//void PartSet_OperationFeatureEditMulti::blockSelection(bool isBlocked,
//                                                       const bool isRestoreSelection)
//{
//  if (myIsBlockedSelection == isBlocked)
//    return;
//
//  myIsBlockedSelection = isBlocked;
//  QList<ObjectPtr> aFeatureList;
////  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast =
////      myFeatures.end();
//  /*for(; anIt != aLast; anIt++)
//   aFeatureList.append((*anIt).feature());*/
//  //if (isBlocked) {
//  //  emit setSelection(QList<ObjectPtr>());
//  //  emit stopSelection(aFeatureList, true);
//  //} else {
//  //  emit stopSelection(aFeatureList, false);
//  //  if (isRestoreSelection) {
//  //    emit setSelection(aFeatureList);
//  //  }
//  //}
//}

void PartSet_OperationFeatureEditMulti::sendFeatures()
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);

  std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
  while (aFeatIter != myFeature2Attribute.end()) {
    FeaturePtr aFeature = aFeatIter->first;
    if (aFeature) {
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
    }
    aFeatIter++;
  }

  Events_Loop::loop()->flush(anEvent);
  flushUpdated();
}

