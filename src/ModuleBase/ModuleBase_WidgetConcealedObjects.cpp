// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetConcealedObjects.cpp
// Created:     29 Jul 2016
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetConcealedObjects.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>

#include <Config_WidgetAPI.h>

#include <QGridLayout>

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QToolButton>

ModuleBase_WidgetConcealedObjects::ModuleBase_WidgetConcealedObjects(QWidget* theParent,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  myBaseShapeAttribute = theData->getProperty("base_shape_attribute");
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
    QToolButton* aButton = dynamic_cast<QToolButton*>(myView->cellWidget(i, 0));;
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
      std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > > aRefs;
      myBaseFeature->data()->referencesToObjects(aRefs);
      std::list<std::pair<std::string, std::list<ObjectPtr> > >::const_iterator
                                                      anIt = aRefs.begin(), aLast = aRefs.end();
      for (; anIt != aLast; anIt++) {
        std::list<ObjectPtr> anObjects = (*anIt).second;
        std::list<ObjectPtr>::const_iterator anOIt = anObjects.begin(), anOLast = anObjects.end();
        for (; anOIt != anOLast; anOIt++) {
          ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(*anOIt);
          if (aResult && aResult->isConcealed()) {
            int aRowId = myView->rowCount();
            addViewRow(aResult);
            myConcealedResults[aRowId] = aResult;
          }
        }
      }
    }
  }
  
  DataPtr aData = myFeature->data();
  AttributeRefListPtr anAttributeList = aData->reflist(attributeID());
  int aSize = anAttributeList->size();
  for (int i = 0, aSize = myView->rowCount(); i < aSize; i++) {
    ResultPtr aResult = myConcealedResults[i];
    QToolButton* aButton = dynamic_cast<QToolButton*>(myView->cellWidget(i, 0));
    bool isChecked = anAttributeList->isInList(aResult);

    bool aBlocked = aButton->blockSignals(true);
    aButton->setChecked(isChecked);
    this->updateItemIcon(aButton);
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

void ModuleBase_WidgetConcealedObjects::addViewRow(const std::shared_ptr<ModelAPI_Result>& theResult)
{
  int anId = myView->rowCount();
  myView->setRowCount(anId+1);

  QToolButton* aVisibilityBtn = new QToolButton(this);
  connect(aVisibilityBtn, SIGNAL(toggled(bool)), this, SLOT(onItemToggled(bool)));
  aVisibilityBtn->setCheckable(true);
  aVisibilityBtn->setChecked(false);
  updateItemIcon(aVisibilityBtn);

  myView->setCellWidget(anId, 0, aVisibilityBtn);
  myView->setItem(anId, 1, new QTableWidgetItem(theResult->data()->name().c_str()));
}

void ModuleBase_WidgetConcealedObjects::onItemToggled(bool theState)
{
  emit valuesChanged();
  updateObject(myFeature);
}

void ModuleBase_WidgetConcealedObjects::updateItemIcon(QToolButton* theButton)
{
  bool isChecked = theButton->isChecked();
  theButton->setIcon(isChecked ? QIcon(":icons/concealed_on.png")
                               : QIcon(":icons/concealed_off.png"));
  theButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
