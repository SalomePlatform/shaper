// File:        PartSet_OperationSketchBase.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchBase.h>
#include <PartSet_Tools.h>
#include <ModelAPI_ResultBody.h>

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_WidgetValueFeature.h>

#include <SketchPlugin_Feature.h>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>
#include <AIS_DimensionSelectionMode.hxx>

#include <QKeyEvent>
#include <QMessageBox>
#include <QApplication>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketchBase::PartSet_OperationSketchBase(const QString& theId, QObject* theParent)
    : ModuleBase_Operation(theId, theParent)
{
}

PartSet_OperationSketchBase::~PartSet_OperationSketchBase()
{
}

boost::shared_ptr<GeomAPI_Shape> PartSet_OperationSketchBase::preview(FeaturePtr theFeature)
{
  boost::shared_ptr<SketchPlugin_Feature> aFeature = boost::dynamic_pointer_cast<
      SketchPlugin_Feature>(theFeature);
  if (aFeature) {
    ResultPtr aRes = aFeature->firstResult();
    ResultBodyPtr aBody = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(aRes);
    if (aBody)
      return aBody->shape();
  }
  return boost::shared_ptr<GeomAPI_Shape>();
}

std::list<FeaturePtr> PartSet_OperationSketchBase::subFeatures() const
{
  return std::list<FeaturePtr>();
}

FeaturePtr PartSet_OperationSketchBase::createFeature(const bool theFlushMessage)
{
  ModuleBase_Operation::createFeature(theFlushMessage);
  if (myFeature)
    emit featureConstructed(myFeature, FM_Activation);
  return myFeature;
}

void PartSet_OperationSketchBase::mousePressed(
    QMouseEvent* theEvent, Handle_V3d_View theView,
    const std::list<ModuleBase_ViewerPrs>& theSelected,
    const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}
void PartSet_OperationSketchBase::mouseReleased(
    QMouseEvent* theEvent, Handle_V3d_View theView,
    const std::list<ModuleBase_ViewerPrs>& theSelected,
    const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}
void PartSet_OperationSketchBase::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
}
void PartSet_OperationSketchBase::mouseDoubleClick(
    QMouseEvent* theEvent, Handle_V3d_View theView,
    const std::list<ModuleBase_ViewerPrs>& theSelected,
    const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}

void PartSet_OperationSketchBase::restartOperation(const std::string& theType, ObjectPtr theFeature)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theFeature);
  if (aFeature) {
    QStringList aNested = this->nestedFeatures();
    if (!aNested.isEmpty()) {
      if (!aNested.contains(QString(aFeature->getKind().c_str())))
        return;
    }
  }
  emit restartRequired(theType, theFeature);
}



void PartSet_OperationSketchBase::activateByPreselection()
{
  if (!myPropertyPanel)
    return;
  ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
  if ((myPreSelection.size() > 0) && aActiveWgt) {
    const ModuleBase_ViewerPrs& aPrs = myPreSelection.front();
    ModuleBase_WidgetValueFeature aValue;
    aValue.setObject(aPrs.object());
    if (aActiveWgt->setValue(&aValue)) {
      myPreSelection.remove(aPrs);
      if(isValid()) {
        //myActiveWidget = NULL;
        commit();
      } else {
        myPropertyPanel->activateNextWidget();
        //emit activateNextWidget(myActiveWidget);
      }
    }
    // If preselection is enough to make a valid feature - apply it immediately
  }
}
