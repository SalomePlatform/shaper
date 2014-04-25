// File:        ModuleBase_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetPoint2D.h>

#include <Config_Keywords.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Point2D.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include <cfloat>
#include <climits>

ModuleBase_WidgetPoint2D::ModuleBase_WidgetPoint2D(QWidget* theParent, QString theTitle,
                                                   const std::string& theFeatureAttributeID)
: ModuleBase_WidgetCustom(theParent), myFeatureAttributeID(theFeatureAttributeID)
{
  myGroupBox = new QGroupBox(theTitle, theParent);
  QGridLayout* aGroupLay = new QGridLayout(myGroupBox);
  aGroupLay->setContentsMargins(0, 0, 0, 0);
  aGroupLay->setColumnStretch(1, 1);
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText("X");
    aLabel->setPixmap(QPixmap(":pictures/x_point.png"));
    aGroupLay->addWidget(aLabel, 0, 0);

    myXSpin = new QDoubleSpinBox(myGroupBox);
    myXSpin->setMinimum(-DBL_MAX);
    myXSpin->setMaximum(DBL_MAX);
    myXSpin->setToolTip("X");
    aGroupLay->addWidget(myXSpin, 0, 1);
    
    connect(myXSpin, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
  }
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText("Y");
    aLabel->setPixmap(QPixmap(":pictures/y_point.png"));
    aGroupLay->addWidget(aLabel, 1, 0);

    myYSpin = new QDoubleSpinBox(myGroupBox);
    myYSpin->setMinimum(-DBL_MAX);
    myYSpin->setMaximum(DBL_MAX);
    myYSpin->setToolTip("X");
    aGroupLay->addWidget(myYSpin, 1, 1);

    connect(myYSpin, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
  }
}

ModuleBase_WidgetPoint2D::~ModuleBase_WidgetPoint2D()
{
}

void ModuleBase_WidgetPoint2D::store(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(myFeatureAttributeID));

  aPoint->setValue(myXSpin->value(), myYSpin->value());
}

QWidget* ModuleBase_WidgetPoint2D::getControl() const
{
  return myGroupBox;
}
