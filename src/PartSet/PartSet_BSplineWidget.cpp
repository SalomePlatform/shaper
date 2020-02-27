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

#include <PartSet_BSplineWidget.h>

#include <SketchPlugin_BSpline.h>

#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeDoubleArray.h>

#include <GeomDataAPI_Point2DArray.h>

#include <GeomAPI_Pnt2d.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolButton>


static const double THE_MIN_WEIGHT = 1.e-7;

PartSet_BSplineWidget::PartSet_BSplineWidget(
    QWidget* theParent,
    const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  aMainLayout->setContentsMargins(0, 0, 0, 0);

  // GroupBox to keep widgets for B-spline poles and weights
  myPolesGroupBox = new QGroupBox(tr("Poles and weights"), this);
  aMainLayout->addWidget(myPolesGroupBox);

  QVBoxLayout* aLayout = new QVBoxLayout(myPolesGroupBox);
  ModuleBase_Tools::adjustMargins(aLayout);

  myScrollArea = new QScrollArea(myPolesGroupBox);
  myScrollArea->setWidgetResizable(true);
  myScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  myScrollArea->setFrameStyle(QFrame::NoFrame);
  aLayout->addWidget(myScrollArea);

  QWidget* aContainer = new QWidget(myScrollArea);
  QVBoxLayout* aBoxLay = new QVBoxLayout(aContainer);
  aBoxLay->setContentsMargins(0, 0, 0, 0);

  // layout of GroupBox
  myPolesWgt = new QWidget(aContainer);
  QGridLayout* aGroupLayout = new QGridLayout(myPolesWgt);
  aGroupLayout->setColumnStretch(1, 1);
  ModuleBase_Tools::adjustMargins(aGroupLayout);

  restoreValueCustom();
  aBoxLay->addWidget(myPolesWgt);
  aBoxLay->addStretch(1);
  myScrollArea->setWidget(aContainer);
}

void PartSet_BSplineWidget::setFeature(const FeaturePtr& theFeature,
                                            const bool theToStoreValue,
                                            const bool isUpdateFlushed)
{
  ModuleBase_ModelWidget::setFeature(theFeature, theToStoreValue, isUpdateFlushed);
  restoreValueCustom();
}

void PartSet_BSplineWidget::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  storeValueCustom();
}


QList<QWidget*> PartSet_BSplineWidget::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myScrollArea);
  return aControls;
}

void PartSet_BSplineWidget::storePolesAndWeights() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  AttributeDoubleArrayPtr aWeightsArray = aData->realArray(SketchPlugin_BSpline::WEIGHTS_ID());

  std::list<BSplinePoleWidgets>::const_iterator anIt = myPoles.begin();
  for (int anIndex = 0; anIt != myPoles.end(); ++anIndex, ++anIt) {
    aWeightsArray->setValue(anIndex, anIt->myWeight->value());
  }
}

bool PartSet_BSplineWidget::storeValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData || !aData->isValid()) // can be on abort of sketcher element
    return false;

  AttributeDoubleArrayPtr aWeights = aData->realArray(SketchPlugin_BSpline::WEIGHTS_ID());

  bool isBlocked = blockSignals(true);
  storePolesAndWeights();
  ModuleBase_Tools::flushUpdated(myFeature);
  blockSignals(isBlocked);

  updateObject(myFeature);
  return true;
}

bool PartSet_BSplineWidget::restoreValueCustom()
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
    bool isBlocked = anIt->myWeight->blockSignals(true);
    anIt->myWeight->setValue(aWeights->value(anIndex));
    anIt->myWeight->blockSignals(isBlocked);
  }

  return true;
}

void PartSet_BSplineWidget::addPoleWidget()
{
  QGridLayout* aGroupLay = dynamic_cast<QGridLayout*>(myPolesWgt->layout());
  int aNbPoles = (int)myPoles.size();
  QString aPoleStr = tr("Pole %1").arg(aNbPoles + 1);

  myPoles.push_back(BSplinePoleWidgets());
  BSplinePoleWidgets& aPole = myPoles.back();
  aGroupLay->addWidget(createPoleWidget(aPole, aPoleStr, myPolesWgt), aNbPoles, 1);
}

QGroupBox* PartSet_BSplineWidget::createPoleWidget(BSplinePoleWidgets& thePole,
  const QString& theName, QWidget* theParent)
{
  QGroupBox* aPoleGroupBox = new QGroupBox(theName, theParent);
  QGridLayout* aPoleLay = new QGridLayout(aPoleGroupBox);
  aPoleLay->setSpacing(0);
  ModuleBase_Tools::zeroMargins(aPoleLay);

  thePole.myX = new ModuleBase_LabelValue(aPoleGroupBox, tr("X"));
  aPoleLay->addWidget(thePole.myX, 0, 0, 1, 3);
  thePole.myY = new ModuleBase_LabelValue(aPoleGroupBox, tr("Y"));
  aPoleLay->addWidget(thePole.myY, 1, 0, 1, 3);
  thePole.myWeight = new ModuleBase_ParamSpinBox(aPoleGroupBox);
  thePole.myWeight->setMinimum(THE_MIN_WEIGHT);

  aPoleLay->addWidget(new QLabel(tr("Weight :"), aPoleGroupBox), 2, 0);
  aPoleLay->addWidget(thePole.myWeight, 2, 1);
  // we should listen textChanged signal as valueChanged do not send when text is modified
  connect(thePole.myWeight, SIGNAL(textChanged(const QString&)),
    this, SIGNAL(valuesChanged()));

  thePole.myAddBtn = new QToolButton(aPoleGroupBox);
  thePole.myAddBtn->setIcon(QIcon(":pictures/add.png"));
  thePole.myAddBtn->setToolTip(tr("Add a new pole after the current"));
  aPoleLay->addWidget(thePole.myAddBtn, 2, 2);
  connect(thePole.myAddBtn, SIGNAL(clicked(bool)), SLOT(onAddPole()));

  return aPoleGroupBox;
}


void PartSet_BSplineWidget::onAddPole()
{
  QObject* aObj = sender();
  std::list<BSplinePoleWidgets>::const_iterator aIt;
  int aId = 0;
  bool aFound = false;
  for (aIt = myPoles.cbegin(); aIt != myPoles.cend(); aIt++, aId++) {
    if ((*aIt).myAddBtn == aObj) {
      aFound = true;
      break;
    }
  }
  if (aFound) {
    // add a new pole after found Id
    std::ostringstream anActionName;
    anActionName << SketchPlugin_BSplineBase::ADD_POLE_ACTION_ID() << "#" << aId;
    if (feature()->customAction(anActionName.str()))
      updateObject(feature());

    restoreValueCustom();
  }
}
