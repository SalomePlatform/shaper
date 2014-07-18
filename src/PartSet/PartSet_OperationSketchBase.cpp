// File:        PartSet_OperationSketchBase.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchBase.h>
#include <PartSet_Tools.h>
#include <ModelAPI_ResultBody.h>

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

PartSet_OperationSketchBase::PartSet_OperationSketchBase(const QString& theId,
	                                                     QObject* theParent)
: ModuleBase_Operation(theId, theParent)
{
}

PartSet_OperationSketchBase::~PartSet_OperationSketchBase()
{
}

boost::shared_ptr<GeomAPI_Shape> PartSet_OperationSketchBase::preview(
                                      FeaturePtr theFeature)
{
  boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                              boost::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
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

std::list<int> PartSet_OperationSketchBase::getSelectionModes(ObjectPtr theFeature) const
{
  std::list<int> aModes;
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theFeature);
  if (aFeature) {
    if (PartSet_Tools::isConstraintFeature(aFeature->getKind())) {
        aModes.clear();
        aModes.push_back(AIS_DSM_Text);
        aModes.push_back(AIS_DSM_Line);
        return aModes;
    }
  }
  aModes.push_back(AIS_Shape::SelectionMode((TopAbs_ShapeEnum)TopAbs_VERTEX));
  aModes.push_back(AIS_Shape::SelectionMode((TopAbs_ShapeEnum)TopAbs_EDGE));
  return aModes;
}
FeaturePtr PartSet_OperationSketchBase::createFeature(const bool theFlushMessage)
{
  FeaturePtr aFeature = ModuleBase_Operation::createFeature(theFlushMessage);
  if (aFeature)
    emit featureConstructed(aFeature, FM_Activation);
  return aFeature;
}


void PartSet_OperationSketchBase::mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                                               const std::list<ModuleBase_ViewerPrs>& theSelected,
                                               const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}
void PartSet_OperationSketchBase::mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                                                const std::list<ModuleBase_ViewerPrs>& theSelected,
                                                const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}
void PartSet_OperationSketchBase::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
}
void PartSet_OperationSketchBase::mouseDoubleClick(QMouseEvent* theEvent, Handle_V3d_View theView,
                                                   const std::list<ModuleBase_ViewerPrs>& theSelected,
                                                   const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}

void PartSet_OperationSketchBase::keyReleased(const int theKey)
{
  switch (theKey) {
    case Qt::Key_Escape: {
      bool toAbort = true;
      if (isModified()) {
        int anAnswer = QMessageBox::question(qApp->activeWindow(), tr("Cancel operation"),
                                  tr("Operation %1 will be cancelled. Continue?").arg(id()),
                                  QMessageBox::Yes, QMessageBox::No);
        toAbort = (anAnswer == QMessageBox::Yes);
      }
      if (toAbort)
        abort();
    }
    break;
    default:
    break;
  }
}

void PartSet_OperationSketchBase::keyReleased(std::string theName, QKeyEvent* theEvent)
{
  keyReleased(theEvent->key());
}

void PartSet_OperationSketchBase::restartOperation(const std::string& theType,
                                                   ObjectPtr theFeature)
{
  emit launchOperation(theType, theFeature);
}
