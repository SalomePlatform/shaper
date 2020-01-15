// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <SketchPlugin_BSplineWidget.h>

#include <SketchPlugin_BSpline.h>

#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeDoubleArray.h>

#include <GeomDataAPI_Point2DArray.h>

#include <GeomAPI_Pnt2d.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>


SketchPlugin_BSplineWidget::SketchPlugin_BSplineWidget(
    QWidget* theParent,
    const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);

  // GroupBox to keep widgets for B-spline poles and weights
  myPolesGroupBox = new QGroupBox(tr("Poles and weights"), theParent);
  aMainLayout->addWidget(myPolesGroupBox);
  // layout of GroupBox
  QGridLayout* aGroupLayout = new QGridLayout(myPolesGroupBox);
  aGroupLayout->setSpacing(4);
  aGroupLayout->setColumnStretch(1, 1);

  restoreValueCustom();
}

void SketchPlugin_BSplineWidget::setFeature(const FeaturePtr& theFeature,
                                            const bool theToStoreValue,
                                            const bool isUpdateFlushed)
{
  ModuleBase_ModelWidget::setFeature(theFeature, theToStoreValue, isUpdateFlushed);
  restoreValueCustom();
}

void SketchPlugin_BSplineWidget::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  storeValueCustom();
}


QList<QWidget*> SketchPlugin_BSplineWidget::getControls() const
{
  QList<QWidget*> aControls;
  std::list<BSplinePoleWidgets>::const_iterator anIt = myPoles.begin();
  for (; anIt != myPoles.end(); ++anIt) {
    aControls.append(anIt->myX);
    aControls.append(anIt->myY);
    aControls.append(anIt->myWeight);
  }
  return aControls;
}

void SketchPlugin_BSplineWidget::storePolesAndWeights() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  AttributePoint2DArrayPtr aPointArray = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aData->attribute(SketchPlugin_BSpline::POLES_ID()));
  AttributeDoubleArrayPtr aWeightsArray = aData->realArray(SketchPlugin_BSpline::WEIGHTS_ID());

  aPointArray->setSize((int)myPoles.size());
  aWeightsArray->setSize((int)myPoles.size());

  std::list<BSplinePoleWidgets>::const_iterator anIt = myPoles.begin();
  for (int anIndex = 0; anIt != myPoles.end(); ++anIndex, ++anIt) {
    aPointArray->setPnt(anIndex, anIt->myX->value(), anIt->myY->value());
    aWeightsArray->setValue(anIndex, anIt->myWeight->value());
  }
}

bool SketchPlugin_BSplineWidget::storeValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData || !aData->isValid()) // can be on abort of sketcher element
    return false;

  AttributePoint2DArrayPtr aPoles = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aData->attribute(SketchPlugin_BSpline::POLES_ID()));
  AttributeDoubleArrayPtr aWeights = aData->realArray(SketchPlugin_BSpline::WEIGHTS_ID());

  bool isBlocked = blockSignals(true);
  bool isImmutable = aPoles->setImmutable(true);

  storePolesAndWeights();
  ModuleBase_Tools::flushUpdated(myFeature);

  aPoles->setImmutable(isImmutable);
  blockSignals(isBlocked);

  return true;
}

bool SketchPlugin_BSplineWidget::restoreValueCustom()
{
  if (!myFeature)
    return false;

  DataPtr aData = myFeature->data();

  AttributePoint2DArrayPtr aPoles = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aData->attribute(SketchPlugin_BSpline::POLES_ID()));
  AttributeDoubleArrayPtr aWeights = aData->realArray(SketchPlugin_BSpline::WEIGHTS_ID());

  while (myPoles.size() < aPoles->size())
    addPoleWidget();

  std::list<BSplinePoleWidgets>::iterator anIt = myPoles.begin();
  for (int anIndex = 0; anIt != myPoles.end(); ++anIt, ++anIndex) {
    GeomPnt2dPtr aPoint = aPoles->pnt(anIndex);
    anIt->myX->setValue(aPoint->x());
    anIt->myY->setValue(aPoint->y());
    anIt->myWeight->setValue(aWeights->value(anIndex));
  }

  return true;
}

void SketchPlugin_BSplineWidget::addPoleWidget()
{
  QGridLayout* aGroupLay = dynamic_cast<QGridLayout*>(myPolesGroupBox->layout());

  int aNbPoles = (int)myPoles.size();

  QString aPoleStr = tr("Pole %1");
  aPoleStr = aPoleStr.arg(aNbPoles + 1);

  QGroupBox* aPoleGroupBox = new QGroupBox(aPoleStr, myPolesGroupBox);
  QFormLayout* aPoleLay = new QFormLayout(aPoleGroupBox);
  ModuleBase_Tools::adjustMargins(aPoleLay);
  aPoleLay->setSpacing(2);

  myPoles.push_back(BSplinePoleWidgets());
  BSplinePoleWidgets& aPoleWidgets = myPoles.back();

  aPoleWidgets.myX = new ModuleBase_LabelValue(aPoleGroupBox, tr("X"));
  aPoleLay->addRow(aPoleWidgets.myX);
  aPoleWidgets.myY = new ModuleBase_LabelValue(aPoleGroupBox, tr("Y"));
  aPoleLay->addRow(aPoleWidgets.myY);
  aPoleWidgets.myWeight = new ModuleBase_ParamSpinBox(aPoleGroupBox);
  aPoleWidgets.myWeight->setMinimum(0.0);
  aPoleLay->addRow(tr("Weight") + " : ", aPoleWidgets.myWeight);

  aGroupLay->addWidget(aPoleGroupBox, aNbPoles, 1);

  // we should listen textChanged signal as valueChanged do not send when text is modified
  connect(aPoleWidgets.myWeight, SIGNAL(textChanged(const QString&)),
          this, SIGNAL(valuesModified()));
}
