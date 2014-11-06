// File:        PartSet_OperationFeatureEdit.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureEdit.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>
#include <SketchPlugin_Constraint.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IViewer.h>

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
#include <AIS_DimensionOwner.hxx>
#include <AIS_DimensionSelectionMode.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationFeatureEdit::PartSet_OperationFeatureEdit(const QString& theId,
                                                           QObject* theParent,
                                                           CompositeFeaturePtr theFeature)
    : PartSet_OperationFeatureBase(theId, theParent, theFeature),
      myIsBlockedSelection(false)
{
  myIsEditing = true;
}

PartSet_OperationFeatureEdit::~PartSet_OperationFeatureEdit()
{
}

void PartSet_OperationFeatureEdit::initSelection(ModuleBase_ISelection* theSelection,
                                                      ModuleBase_IViewer* theViewer)
{
  // the method of the parent should is useless here because it processes the given
  // selection in different way
  //PartSet_OperationFeatureBase::initSelection(theSelection, theViewer);

  // 1. unite selected and hightlighted objects in order to have an opportunity to drag
  // by the highlighted object
  QList<ModuleBase_ViewerPrs> aFeatures = theSelection->getSelected();
  QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();
  // add highlighted elements if they are not selected
  foreach (ModuleBase_ViewerPrs aPrs, aHighlighted) {
    if (!PartSet_Tools::isContainPresentation(aFeatures, aPrs))
      aFeatures.append(aPrs);
  }

  // 1. find all features with skipping features with selected vertex shapes
  myFeature2Attribute.clear();
  // firstly, collect the features without local selection
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
        // using an empty list as a sign, that this feature should be moved itself
        myFeature2Attribute[aFeature] = aList;
      }
    }
  }
  // 2. collect the features with a local selection on them.
  // if the list already has this feature, the local selection is skipped
  // that means that if the selection contains a feature and a feature with local selected point,
  // the edit is performed for a full feature
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

      // append the attribute of the vertex if it is found on the current feature
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

void PartSet_OperationFeatureEdit::mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection)
{
  ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
  if(aActiveWgt && aActiveWgt->isViewerSelector()) {
    // Almost do nothing, all stuff in on PartSet_OperationFeatureBase::mouseReleased
    PartSet_OperationFeatureBase::mousePressed(theEvent, theViewer, theSelection);
    return;
  }
  else {
    // commit always until the selection restore is realized (for feature and local selection)
    // TODO: check whether the selection is changed and restart the operation only if it is modified
    commit();
    emitFeaturesDeactivation();
    // find nearest feature and restart the operation for it
    Handle(V3d_View) aView = theViewer->activeView();
    QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
    QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();

    ObjectPtr aFeature = PartSet_Tools::nearestFeature(theEvent->pos(), aView, sketch(),
                                                       aSelected, aHighlighted);
    if (aFeature) {
      restartOperation(PartSet_OperationFeatureEdit::Type(), aFeature);
    }
  }
  // the next code is commented because the new attempt to commit/restart operation implementation:
  //QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
  //QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();
  //bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
  //if (aHasShift && !aHighlighted.empty()) {
  //  foreach (ModuleBase_ViewerPrs aPrs, aHighlighted) {
  //    aSelected.append(aPrs);
  //  }
  //}
  //ObjectPtr aObject;
  ///*if (!aSelected.empty()) {
  //  aObject = aSelected.first().object();
  //} else {   
  //  if (!aHighlighted.empty())
  //    aObject = aHighlighted.first().object();
  //}*/
  //// the priority to a highlighted object in order to edit it, even if the selected object is
  //// the feature of this operation. Otherwise, the highlighting is ignored and the selected
  //// object is moved
  //if (!aHighlighted.empty()) {
  //  aObject = aHighlighted.front().object();
  //}
  //if (!aObject && !aSelected.empty())  // changed for a constrain
  //  aObject = aSelected.front().object();

  //FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
  //if (!aFeature || aFeature != feature() || (aSelected.size() > 1)) {
  //  if (commit()) {
  //    theViewer->enableSelection(true);
  //    emit featureConstructed(feature(), FM_Deactivation);

  //    // If we have selection and prehilighting with shift pressed 
  //    // Then we have to select all these objects and restart as multi edit operfation
  //    //bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
  //    //if (aHasShift && !theHighlighted.empty()) {
  //    //  QList<ObjectPtr> aSelected;
  //    //  std::list<ModuleBase_ViewerPrs>::const_iterator aIt;
  //    //  for (aIt = theSelected.cbegin(); aIt != theSelected.cend(); ++aIt)
  //    //    aSelected.append((*aIt).object());

  //    //  for (aIt = theHighlighted.cbegin(); aIt != theHighlighted.cend(); ++aIt) {
  //    //    if (!aSelected.contains((*aIt).object()))
  //    //      aSelected.append((*aIt).object());
  //    //  }
  //    //  emit setSelection(aSelected);
  //    //} else 
  //    if (aFeature) {
  //      std::string anOperationType = PartSet_OperationFeatureEdit::Type();
  //      restartOperation(anOperationType, aFeature);
  //    }
  //    //}
  //  }
  //}
}

void PartSet_OperationFeatureEdit::mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer)
{
  if (!(theEvent->buttons() & Qt::LeftButton))
    return;
  Handle(V3d_View) aView = theViewer->activeView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);

  theViewer->enableSelection(false);

  //blockSelection(true);
  if (myCurPoint.myIsInitialized) {
    double aCurX, aCurY;
    PartSet_Tools::convertTo2D(myCurPoint.myPoint, sketch(), aView, aCurX, aCurY);

    double aX, anY;
    PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aX, anY);

    double aDeltaX = aX - aCurX;
    double aDeltaY = anY - aCurY;

    // the next code is commented because it is obsolete by the multi edit operation realization here
    //if (myIsMultiOperation) {
    //  std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
    //  while (aFeatIter != myFeature2Attribute.end()) {
    //    FeaturePtr aFeature = aFeatIter->first;
    //    std::list<std::string> anAttributes = aFeatIter->second;
    //    // perform edit for the feature
    //    if (anAttributes.empty()) {
    //      boost::shared_ptr<SketchPlugin_Feature> aSketchFeature =
    //                                     boost::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    //      if (aSketchFeature) {
    //        aSketchFeature->move(aDeltaX, aDeltaY);
    //      }
    //    }
    //    // perform edit for the feature's attribute
    //    else {
    //      std::list<std::string>::const_iterator anAttrIter = anAttributes.begin(),
    //                                             anAttrEnd = anAttributes.end();
    //      for (; anAttrIter != anAttrEnd; anAttrIter++) {
    //        boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr = boost::dynamic_pointer_cast<
    //                         GeomDataAPI_Point2D>(aFeature->data()->attribute(*anAttrIter));
    //        if (aPointAttr) {
    //          aPointAttr->move(aDeltaX, aDeltaY);
    //        }
    //      }      
    //    }
    //    aFeatIter++;
    //  }
    //}
    //else { // multieditoperation

    //boost::shared_ptr<SketchPlugin_Feature> aSketchFeature = boost::dynamic_pointer_cast<
    //    SketchPlugin_Feature>(feature());

    bool isMoved = false;
    // the functionality to move the feature attribute if it exists in the internal map
    std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
    while (aFeatIter != myFeature2Attribute.end()) {
      FeaturePtr aFeature = aFeatIter->first;
      // MPV: added condition because it could be external edge of some object, not sketch
      if (aFeature && !sketch()->isSub(aFeature)) {
        aFeatIter++;
        continue;
      }

      std::list<std::string> anAttributes = aFeatIter->second;
      // perform edit for the feature
      if (anAttributes.empty()) {
        boost::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                                       boost::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature) {
          aSketchFeature->move(aDeltaX, aDeltaY);
          isMoved = true;
        }
      }
      // perform edit for the feature's attribute
      else {
        std::list<std::string>::const_iterator anAttrIter = anAttributes.begin(),
                                               anAttrEnd = anAttributes.end();
        for (; anAttrIter != anAttrEnd; anAttrIter++) {
          boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr = boost::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(aFeature->data()->attribute(*anAttrIter));
          if (aPointAttr) {
            aPointAttr->move(aDeltaX, aDeltaY);
            isMoved = true;
          }
        }      
      }
      aFeatIter++;
    }
    // the next code is commented because it is obsolete by the multi edit operation realization here
    // the feature is moved only if there is no a local selection on this feature
    //if (!isMoved) {
    //  // MPV: added condition because it could be external edge of some object, not sketch
    //  if (aSketchFeature && sketch()->isSub(aSketchFeature)) {
    //   aSketchFeature->move(aDeltaX, aDeltaY);
    //    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
    //   ModelAPI_EventCreator::get()->sendUpdated(feature(), anEvent);
    //  }
    // }
    //} // multieditoperation
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationFeatureEdit::mouseReleased(
    QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
    ModuleBase_ISelection* theSelection)
{
  theViewer->enableSelection(true);
  // the next code is commented because it is obsolete by the multi edit operation realization here
  //if (myIsMultiOperation) {
  //  if (commit()) {
  //    std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
  //    while (aFeatIter != myFeature2Attribute.end()) {
  //      FeaturePtr aFeature = aFeatIter->first;
  //      if (aFeature) {
  //        emit featureConstructed(aFeature, FM_Deactivation);
  //      }
  //      aFeatIter++;
  //    }
  //  }
  //}
  //else { // multieditoperation
  ModuleBase_ModelWidget* aActiveWgt = 0;
  if (myPropertyPanel)
    aActiveWgt = myPropertyPanel->activeWidget();
  if(aActiveWgt && aActiveWgt->isViewerSelector()) {
    // Almost do nothing, all stuff in on PartSet_OperationFeatureBase::mouseReleased
    PartSet_OperationFeatureBase::mouseReleased(theEvent, theViewer, theSelection);
  //}// else {
  ////blockSelection(false);
  ////}
  //} // multieditoperation
  }
  else {
    theViewer->enableSelection(true);

    // commit operation if there is no selected an highlighted objects anymore
    Handle(V3d_View) aView = theViewer->activeView();
    QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
    QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();

    if (aSelected.empty() && aHighlighted.empty()) {
      commit();
      emitFeaturesDeactivation();
    }
  }
}

void PartSet_OperationFeatureEdit::mouseDoubleClick(
    QMouseEvent* theEvent, Handle_V3d_View theView,
    ModuleBase_ISelection* theSelection)
{
  // TODO the functionality is important only for constraint feature. Should be moved in another place
  QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
  if (!aSelected.empty()) {
    ModuleBase_ViewerPrs aFeaturePrs = aSelected.first();
    if (!aFeaturePrs.owner().IsNull()) {
      Handle(AIS_DimensionOwner) anOwner = Handle(AIS_DimensionOwner)::DownCast(
          aFeaturePrs.owner());
      if (!anOwner.IsNull() && anOwner->SelectionMode() == AIS_DSM_Text) {
        bool isValid;
        double aValue = PartSet_Tools::featureValue(feature(), SketchPlugin_Constraint::VALUE(),
                                                    isValid);
        if (isValid) {
          ModuleBase_WidgetEditor::editFeatureValue(feature(), SketchPlugin_Constraint::VALUE());
          flushUpdated();
        }
      }
    }
  }
}

void PartSet_OperationFeatureEdit::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //emit multiSelectionEnabled(false);

  myCurPoint.clear();
}

void PartSet_OperationFeatureEdit::stopOperation()
{
  //emit multiSelectionEnabled(true);

  //blockSelection(false, false);

  myFeature2Attribute.clear();
}

//void PartSet_OperationFeatureEdit::blockSelection(bool isBlocked, const bool isRestoreSelection)
//{
//  if (myIsBlockedSelection == isBlocked)
//    return;
//
//  myIsBlockedSelection = isBlocked;
//  QList<ObjectPtr> aFeatureList;
//  aFeatureList.append(feature());
//
//  //if (isBlocked) {
//  //  emit setSelection(QList<ObjectPtr>());
//  //  emit stopSelection(aFeatureList, true);
//  //} else {
//  //  emit stopSelection(aFeatureList, false);
//  //  if (isRestoreSelection)
//  //    emit setSelection(aFeatureList);
//  //}
//}

FeaturePtr PartSet_OperationFeatureEdit::createFeature(const bool theFlushMessage,
  CompositeFeaturePtr theCompositeFeature)
{
  // do nothing in order to do not create a new feature
  return FeaturePtr();
}

void PartSet_OperationFeatureEdit::sendFeatures()
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

void PartSet_OperationFeatureEdit::emitFeaturesDeactivation()
{
  std::map<FeaturePtr, std::list<std::string>>::iterator aFeatIter = myFeature2Attribute.begin();
  while (aFeatIter != myFeature2Attribute.end()) {
    FeaturePtr aFeature = aFeatIter->first;
    if (aFeature) {
      emit featureConstructed(aFeature, FM_Deactivation);
    }
    aFeatIter++;
  }
}

