// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <ModuleBase_WidgetConcealedObjects.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <Config_WidgetAPI.h>

#include <QGridLayout>

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QVBoxLayout>

#include <algorithm>

const int DEFAULT_NAME_COLUMN_WIDTH = 200;

ModuleBase_WidgetConcealedObjects::ModuleBase_WidgetConcealedObjects(QWidget* theParent,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  myBaseShapeAttribute = theData->getProperty("base_shape_attribute");
  std::string aPickParents = theData->getProperty("pick_concealed_parents");
  std::transform(aPickParents.begin(), aPickParents.end(), aPickParents.begin(), ::tolower);
  myPickConcealedParents = aPickParents == "1" || aPickParents == "true" || aPickParents == "yes";

  QGridLayout* aMainLay = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);

  myView = new QTableWidget(this);
  aMainLay->addWidget(myView);

  myView->setColumnCount(2);
  myView->horizontalHeader()->setVisible(false);
  myView->verticalHeader()->setVisible(false);
}

ModuleBase_WidgetConcealedObjects::~ModuleBase_WidgetConcealedObjects()
{
}

bool ModuleBase_WidgetConcealedObjects::storeValueCustom()
{
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeRefListPtr anAttributeList = aData->reflist(attributeID());
  anAttributeList->clear();
  int aSize1 = anAttributeList->size(false);
  for (int i = 0, aSize = myView->rowCount(); i < aSize; i++) {
    QCheckBox* aButton =
        dynamic_cast<QCheckBox*>(myView->cellWidget(i, 0)->findChild<QCheckBox*>());
    if (aButton->isChecked())
      anAttributeList->append(myConcealedResults[i]);
  }
  int aSize = anAttributeList->size(false);
  return true;
}

bool ModuleBase_WidgetConcealedObjects::restoreValueCustom()
{
  FeaturePtr aBaseFeature;
  ObjectPtr anObject;
  if (myFeature) {
    anObject = ModuleBase_Tools::getObject(myFeature->attribute(myBaseShapeAttribute));
    if (anObject.get() != NULL)
      aBaseFeature = ModelAPI_Feature::feature(anObject);
  }
  if (myBaseFeature != aBaseFeature) {
    myView->setRowCount(0);
    myConcealedResults.clear();
    myBaseFeature = aBaseFeature;
    if (myBaseFeature.get()) {
      std::list<std::shared_ptr<ModelAPI_Result> > aResults;
      std::set<ResultBodyPtr> aParents;
      ModelAPI_Tools::getConcealedResults(myBaseFeature, aResults);
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator anIt = aResults.begin(),
                                                                   aLast = aResults.end();
      for (; anIt != aLast; anIt++) {
        ResultPtr aResult = *anIt;
        if (myPickConcealedParents) {
          // pick the parent result of the concealed object
          ResultBodyPtr aRootParent = ModelAPI_Tools::bodyOwner(aResult, true);
          if (aRootParent) {
            if (aParents.find(aRootParent) == aParents.end()) {
              aResult = aRootParent;
              aParents.insert(aRootParent);
            }
            else // do not add parent compound once again
              continue;
          }
        }

        int aRowId = myView->rowCount();
        addViewRow(aResult);
        myConcealedResults[aRowId] = aResult;
      }
    }
  }

  DataPtr aData = myFeature->data();
  AttributeRefListPtr anAttributeList = aData->reflist(attributeID());
  int aSize = anAttributeList->size();
  for (int i = 0, aSize = myView->rowCount(); i < aSize; i++) {
    ResultPtr aResult = myConcealedResults[i];
    QCheckBox* aButton =
        dynamic_cast<QCheckBox*>(myView->cellWidget(i, 0)->findChild<QCheckBox*>());
    bool isChecked = anAttributeList->isInList(aResult);

    bool aBlocked = aButton->blockSignals(true);
    aButton->setChecked(isChecked);
    aButton->blockSignals(aBlocked);
  }
  return true;
}

QList<QWidget*> ModuleBase_WidgetConcealedObjects::getControls() const
{
  QList<QWidget*> result;
  result << myView;
  return result;
}

void ModuleBase_WidgetConcealedObjects::addViewRow(
                                const std::shared_ptr<ModelAPI_Result>& theResult)
{
  int anId = myView->rowCount();
  myView->setRowCount(anId+1);

  QWidget* aVisibilityWdg = new QWidget(this);
  QVBoxLayout* aVisibilityLay = new QVBoxLayout(aVisibilityWdg);
  aVisibilityLay->setContentsMargins(2, 2, 2, 2);
  QCheckBox* aVisibilityBtn = new QCheckBox(aVisibilityWdg);
  aVisibilityLay->addWidget(aVisibilityBtn, 0, Qt::AlignHCenter);
  connect(aVisibilityBtn, SIGNAL(toggled(bool)), this, SLOT(onItemToggled(bool)));
  aVisibilityBtn->setChecked(false);

  myView->setCellWidget(anId, 0, aVisibilityWdg);
  myView->setItem(anId, 1,
    new QTableWidgetItem(QString::fromStdWString(theResult->data()->name())));

  if (anId == 1) {
    myView->setColumnWidth(0, myView->verticalHeader()->defaultSectionSize());
    myView->setColumnWidth(1, DEFAULT_NAME_COLUMN_WIDTH);
  }
}

void ModuleBase_WidgetConcealedObjects::onItemToggled(bool theState)
{
  emit valuesChanged();
  updateObject(myFeature);
}

