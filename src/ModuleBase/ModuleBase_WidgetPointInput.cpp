// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "ModuleBase_WidgetPointInput.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_ParamSpinBox.h"
#include "ModuleBase_ViewerPrs.h"

#include <GeomAPI_Vertex.h>
#include <GeomAPI_Pnt.h>

#include <GeomDataAPI_Point.h>

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QFormLayout>
#include <QLabel>


#define ERR_STRING "ERROR"

ModuleBase_WidgetPointInput::ModuleBase_WidgetPointInput(QWidget* theParent,
  ModuleBase_IWorkshop* theWorkshop,
  const Config_WidgetAPI* theData)
  : ModuleBase_WidgetSelector(theParent, theWorkshop, theData)
{
  myDefaultValue[0] = 0;
  myDefaultValue[1] = 0;
  myDefaultValue[2] = 0;
  bool aAcceptVariables = theData->getBooleanAttribute(DOUBLE_WDG_ACCEPT_EXPRESSIONS, true);

  std::string aDefValuesStr = theData->getProperty(ATTR_DEFAULT);
  if (!aDefValuesStr.empty()) {
    QString aDefVal(aDefValuesStr.c_str());
    QStringList aStrArray = aDefVal.split(';', QString::SkipEmptyParts);
    if (aStrArray.length() == 3) {
      for (int i = 0; i < 3; i++)
        myDefaultValue[i] = aStrArray.at(i).toDouble();
    }
  }
  QFormLayout* aMainlayout = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aMainlayout);

  myXSpin = new ModuleBase_ParamSpinBox(this);
  myXSpin->setAcceptVariables(aAcceptVariables);
  myXSpin->setToolTip(tr("X coordinate"));
  myXSpin->setValue(myDefaultValue[0]);
  QLabel* aXLbl = new QLabel(this);
  aXLbl->setPixmap(QPixmap(":pictures/x_size.png"));
  aMainlayout->addRow(aXLbl, myXSpin);

  myYSpin = new ModuleBase_ParamSpinBox(this);
  myYSpin->setAcceptVariables(aAcceptVariables);
  myYSpin->setToolTip(tr("Y coordinate"));
  myYSpin->setValue(myDefaultValue[1]);
  QLabel* aYLbl = new QLabel(this);
  aYLbl->setPixmap(QPixmap(":pictures/y_size.png"));
  aMainlayout->addRow(aYLbl, myYSpin);

  myZSpin = new ModuleBase_ParamSpinBox(this);
  myZSpin->setAcceptVariables(aAcceptVariables);
  myZSpin->setToolTip(tr("Z coordinate"));
  myZSpin->setValue(myDefaultValue[2]);
  QLabel* aZLbl = new QLabel(this);
  aZLbl->setPixmap(QPixmap(":pictures/z_size.png"));
  aMainlayout->addRow(aZLbl, myZSpin);

  connect(myXSpin, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
  connect(myYSpin, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
  connect(myZSpin, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
}

ModuleBase_WidgetPointInput::~ModuleBase_WidgetPointInput()
{

}


//********************************************************************
QList<QWidget*> ModuleBase_WidgetPointInput::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myXSpin);
  aList.append(myYSpin);
  aList.append(myZSpin);
  return aList;
}

std::string getParmText(ModuleBase_ParamSpinBox* theSpin, FeaturePtr& theParam)
{
  QString aText = theSpin->text();
  if (aText.contains('=')) {
    if (!theParam.get()) {
      theParam = ModuleBase_Tools::createParameter(aText);
      if (!theParam.get()) {
        return ERR_STRING;
      }
    }
    else {
      ModuleBase_Tools::editParameter(theParam, aText);
    }
    aText = aText.split('=').at(0);
  }
  return aText.toStdString();
}

//********************************************************************
bool ModuleBase_WidgetPointInput::storeValueCustom()
{
  AttributePointPtr aAttr = std::dynamic_pointer_cast<GeomDataAPI_Point>(attribute());
  if (aAttr.get()) {
    if (aAttr->isInitialized()) {
      if (myXSpin->hasVariable() || myYSpin->hasVariable() || myZSpin->hasVariable()) {
        std::string aXText = getParmText(myXSpin, myXParam);
        if (aXText == ERR_STRING) {
          aAttr->setExpressionError(0, "Parameter cannot be created");
          aAttr->setExpressionInvalid(0, true);
          updateObject(myFeature);
          return false;
        }
        std::string aYText = getParmText(myYSpin, myYParam);
        if (aYText == ERR_STRING) {
          aAttr->setExpressionError(1, "Parameter cannot be created");
          aAttr->setExpressionInvalid(1, true);
          updateObject(myFeature);
          return false;
        }
        std::string aZText = getParmText(myZSpin, myZParam);
        if (aZText == ERR_STRING) {
          aAttr->setExpressionError(2, "Parameter cannot be created");
          aAttr->setExpressionInvalid(2, true);
          updateObject(myFeature);
          return false;
        }
        aAttr->setText(aXText, aYText, aZText);
      } else {
        aAttr->setText("", "", "");
        aAttr->setValue(myXSpin->value(), myYSpin->value(), myZSpin->value());
      }
    } else {
      aAttr->setValue(myDefaultValue[0], myDefaultValue[1], myDefaultValue[2]);
    }
    updateObject(myFeature);
    return true;
  }
  return false;
}

//********************************************************************
bool ModuleBase_WidgetPointInput::restoreValueCustom()
{
  AttributePointPtr aAttr = std::dynamic_pointer_cast<GeomDataAPI_Point>(attribute());
  if (aAttr.get()) {
    if (aAttr->isInitialized()) {
      std::string aXText = aAttr->textX();
      if (aXText.empty()) {
        myXSpin->setValue(aAttr->x());
      }
      else {
        myXSpin->setText(aXText.c_str());
      }
      std::string aYText = aAttr->textY();
      if (aYText.empty()) {
        myYSpin->setValue(aAttr->y());
      }
      else {
        myYSpin->setText(aYText.c_str());
      }
      std::string aZText = aAttr->textZ();
      if (aZText.empty()) {
        myZSpin->setValue(aAttr->z());
      }
      else {
        myZSpin->setText(aZText.c_str());
      }
    }
    else {
      aAttr->setValue(myDefaultValue[0], myDefaultValue[1], myDefaultValue[2]);
      myXSpin->setValue(myDefaultValue[0]);
      myYSpin->setValue(myDefaultValue[1]);
      myZSpin->setValue(myDefaultValue[2]);
    }
    setValueState(Stored);
    return true;
  }
  return false;
}

//********************************************************************
void ModuleBase_WidgetPointInput::selectionModes(int& theModuleSelectionModes, QIntList& theModes)
{
  theModuleSelectionModes = -1;
  theModes << TopAbs_VERTEX;
}

//********************************************************************
QIntList ModuleBase_WidgetPointInput::shapeTypes() const
{
  QIntList aList;
  aList << TopAbs_VERTEX;
  return aList;
}

//********************************************************************
bool ModuleBase_WidgetPointInput
::setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
  const bool theToValidate)
{
  if (theValues.size() == 1) {
    GeomShapePtr aShape = theValues.first()->shape();
    if (aShape.get() && aShape->isVertex()) {
      GeomVertexPtr aVertex(new GeomAPI_Vertex(aShape));
      GeomPointPtr aPnt = aVertex->point();
      myXSpin->setValue(aPnt->x());
      myYSpin->setValue(aPnt->y());
      myZSpin->setValue(aPnt->z());
      return true;
    }
  }
  return false;
}

//********************************************************************
bool ModuleBase_WidgetPointInput::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
  }
  return isModified;
}
