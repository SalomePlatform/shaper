/*
 * ModuleBase_WidgetFactory.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetFactory.h>

#include <ModuleBase_MetaWidget.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetPoint2D.h>
#include <ModuleBase_WidgetSwitch.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpinBox>
#include <QMetaProperty>
#include <QLabel>
#include <QPixmap>
#include <QGroupBox>
#include <QToolBox>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <cfloat>
#include <climits>

ModuleBase_WidgetFactory::ModuleBase_WidgetFactory(ModuleBase_Operation* theOperation)
 : myOperation(theOperation)
{
  QString aXml = myOperation->getDescription()->xmlRepresentation();
  myWidgetApi = new Config_WidgetAPI(aXml.toStdString());
}

ModuleBase_WidgetFactory::~ModuleBase_WidgetFactory()
{
}

void ModuleBase_WidgetFactory::createWidget(QWidget* theParent)
{
  if (!myWidgetApi->toChildWidget())
    return;

  QVBoxLayout* aWidgetLay = new QVBoxLayout(theParent);
  aWidgetLay->setContentsMargins(2, 2, 2, 2);
  do { //Iterate over each node
    std::string aWdgType = myWidgetApi->widgetType();
    //Create a widget (doublevalue, groupbox, toolbox, etc.
    QWidget* aWidget = createWidgetByType(aWdgType, theParent);
    if (aWidget) {
      aWidgetLay->addWidget(aWidget);
    }
    if (myWidgetApi->isContainerWidget()) {
      //if current widget is groupbox (container) process it's children recursively
      QString aGroupName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
      createWidget(aWidget);
      QGroupBox* aGrBox = qobject_cast<QGroupBox*>(aWidget);
      aGrBox->setTitle(aGroupName);
    }
    if (myWidgetApi->isPagedWidget()) {
      //If current widget is toolbox or switch-casebox then fetch all
      //it's pages recursively and setup into the widget.
      myWidgetApi->toChildWidget();
      do {
        QString aPageName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
        QWidget* aPage = new QWidget(aWidget);
        createWidget(aPage);
        if (aWdgType == WDG_SWITCH) {
          ModuleBase_WidgetSwitch* aSwitch = qobject_cast<ModuleBase_WidgetSwitch*>(aWidget);
          aSwitch->addPage(aPage, aPageName);
        } else if (aWdgType == WDG_TOOLBOX){
          QToolBox* aToolbox = qobject_cast<QToolBox*>(aWidget);
          aToolbox->addItem(aPage, aPageName);
        }
      } while(myWidgetApi->toNextWidget());
    }
  } while(myWidgetApi->toNextWidget());
  theParent->setLayout(aWidgetLay);
}

QWidget* ModuleBase_WidgetFactory::labelControl(QWidget* theParent)
{
  QWidget* result = new QWidget(theParent);
  QVBoxLayout* aLabelLay = new QVBoxLayout(result);
  QLabel* aLabel = new QLabel(result);
  aLabel->setText(qs(myWidgetApi->getProperty(INFO_WDG_TEXT)));
  aLabel->setToolTip(qs(myWidgetApi->getProperty(INFO_WDG_TOOLTIP)));
  aLabelLay->addWidget(aLabel);
  aLabelLay->addStretch(1);
  result->setLayout(aLabelLay);
  return result;
}

QWidget* ModuleBase_WidgetFactory::createWidgetByType(const std::string& theType, QWidget* theParent)
{
  QWidget* result = NULL;
  if (theType == WDG_DOUBLEVALUE) {
    result = doubleSpinBoxControl();

  } else if (theType == WDG_INFO) {
    result = labelControl(theParent);

  } else if (theType == WDG_SELECTOR) {
    result = selectorControl(theParent);

  } else if (theType == WDG_BOOLVALUE) {
    result = booleanControl(theParent);

  } else if (theType == WDG_POINT_SELECTOR) {
    result = pointSelectorControl(theParent);

  } else if (myWidgetApi->isContainerWidget() || myWidgetApi->isPagedWidget()) {
    result = createContainer(theType, theParent);
  }
#ifdef _DEBUG
  else { qDebug() << "ModuleBase_WidgetFactory::fillWidget: find bad widget type"; }
#endif
  return result;
}

QWidget* ModuleBase_WidgetFactory::createContainer(const std::string& theType, QWidget* theParent)
{
  QWidget* result = NULL;
  if (theType == WDG_GROUP || theType == WDG_CHECK_GROUP) {
    QGroupBox* aGroupBox = new QGroupBox(theParent);
    aGroupBox->setCheckable(theType == WDG_CHECK_GROUP);
    result = aGroupBox;
  } else if (theType == WDG_TOOLBOX) {
    result = new QToolBox(theParent);
  } else if (theType == WDG_SWITCH) {
    result = new ModuleBase_WidgetSwitch(theParent);
  } else if (theType == WDG_TOOLBOX_BOX || theType == WDG_SWITCH_CASE) {
    result = NULL;
  }
#ifdef _DEBUG
  else { qDebug() << "ModuleBase_WidgetFactory::fillWidget: find bad container type"; }
#endif
  return result;
}

QWidget* ModuleBase_WidgetFactory::doubleSpinBoxControl()
{
  QWidget* result = new QWidget();
  QHBoxLayout* aControlLay = new QHBoxLayout(result);
  aControlLay->setContentsMargins(0, 0, 0, 0);
  QString aLabelText = qs(myWidgetApi->widgetLabel());
  QString aLabelIcon = qs(myWidgetApi->widgetIcon());
  QLabel* aLabel = new QLabel(aLabelText);
  aLabel->setPixmap(QPixmap(aLabelIcon));

  aControlLay->addWidget(aLabel);
  QDoubleSpinBox* aBox = new QDoubleSpinBox(result);
  QString anObjName = QString::fromStdString(myWidgetApi->widgetId());
  aBox->setObjectName(anObjName);
  bool isOk = false;
  std::string aProp = myWidgetApi->getProperty(DOUBLE_WDG_MIN);
  double aMinVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setMinimum(aMinVal);
  } else {
    aBox->setMinimum(-DBL_MAX);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_MAX);
  double aMaxVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setMaximum(aMaxVal);
  } else {
    aBox->setMaximum(DBL_MAX);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_STEP);
  double aStepVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setSingleStep(aStepVal);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_DFLT);
  double aDefVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setValue(aDefVal);
  }
  QString aTTip = qs(myWidgetApi->widgetTooltip());
  aBox->setToolTip(aTTip);
  aControlLay->addWidget(aBox);
  aControlLay->setStretch(1, 1);
  result->setLayout(aControlLay);
  connectWidget(aBox, WDG_DOUBLEVALUE);
  return result;
}

QWidget* ModuleBase_WidgetFactory::pointSelectorControl(QWidget* theParent)
{
  ModuleBase_WidgetPoint2D* aWidget = new ModuleBase_WidgetPoint2D(theParent,
                       qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME)),
                       myWidgetApi->widgetId());
  connectWidget(aWidget, WDG_POINT_SELECTOR);
  myModelWidgets.append(aWidget);
  return aWidget->getControl();
}

bool ModuleBase_WidgetFactory::connectWidget(QObject* theWidget,  const QString& theType)
{
  bool result = false;
  if (theType == WDG_DOUBLEVALUE) {
    result = QObject::connect(theWidget, SIGNAL(valueChanged(double)), 
                              myOperation, SLOT(storeReal(double)));
  }
  if (theType == WDG_POINT_SELECTOR) {
    result = QObject::connect(theWidget, SIGNAL(valuesChanged()),
                              myOperation, SLOT(storeCustomValue()));
  }
  return result;
}

QString ModuleBase_WidgetFactory::qs(const std::string& theStdString) const
{
  return QString::fromStdString(theStdString);
}


QWidget* ModuleBase_WidgetFactory::selectorControl(QWidget* theParent)
{
  QWidget* aRes = new QWidget();
  QHBoxLayout* aLayout = new QHBoxLayout(aRes);

  aLayout->setContentsMargins(0, 0, 0, 0);
  QString aLabelText = qs(myWidgetApi->widgetLabel());
  QString aLabelIcon = qs(myWidgetApi->widgetIcon());
  QLabel* aLabel = new QLabel(aLabelText, aRes);
  aLabel->setPixmap(QPixmap(aLabelIcon));

  aLayout->addWidget(aLabel);

  QLineEdit* aTextLine = new QLineEdit(aRes);
  aTextLine->setReadOnly(true);

  aLayout->addWidget(aTextLine);

  QToolButton* aActivateBtn = new QToolButton(aRes);
  aActivateBtn->setIcon(QIcon(":icons/hand_point.png"));
  aActivateBtn->setCheckable(true);

  aLayout->addWidget(aActivateBtn);

  return aRes;
}


QWidget* ModuleBase_WidgetFactory::booleanControl(QWidget* theParent)
{
  QString aText = qs(myWidgetApi->widgetLabel());
  QString aToolTip = qs(myWidgetApi->widgetTooltip());
  QString aDefault = qs(myWidgetApi->getProperty("default"));

  QCheckBox* aRes = new QCheckBox(aText, theParent);
  aRes->setToolTip(aToolTip);
  aRes->setChecked(aDefault == "true");
  return aRes;
}