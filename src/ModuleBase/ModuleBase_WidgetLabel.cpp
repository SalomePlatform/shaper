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

#include "ModuleBase_WidgetLabel.h"

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>
#include <Config_Translator.h>

#include <QTextCodec>
#include <QLabel>
#include <QVBoxLayout>


ModuleBase_WidgetLabel::ModuleBase_WidgetLabel(QWidget* theParent,
                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aText = translate(theData->getProperty("title"));
  QString aLabelIcon = QString::fromStdString(theData->getProperty("icon"));
  myLabel = new QLabel(aText, theParent);
  if (!aLabelIcon.isEmpty()) {
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
    myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  } else {
    myLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  }
  myLabel->setWordWrap(true);
  myLabel->setIndent(5);
  myLabel->setContentsMargins(0,0,0,4);

  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myLabel);
  setLayout(aLayout);

  std::string aStyleSheet = theData->getProperty(ATTR_STYLE_SHEET).c_str();
  if (!aStyleSheet.empty())
    myLabel->setStyleSheet(QString("QLabel {%1}").arg(aStyleSheet.c_str()));
}

ModuleBase_WidgetLabel::~ModuleBase_WidgetLabel()
{
}

QList<QWidget*> ModuleBase_WidgetLabel::getControls() const
{
  return QList<QWidget*>();
}

bool ModuleBase_WidgetLabel::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStrAttr = aData->string(attributeID());
  if (aStrAttr.get()) {
    QString aText;
    if (aStrAttr.get()) {
      if (aStrAttr->isUValue()) { // already translated text
        char16_t* aStr = aStrAttr->valueU();
        std::wstring aWStr((wchar_t*)aStr);
        static const int aBufSize = 1000;
        static char aMBStr[aBufSize];
        size_t aLen = wcstombs(aMBStr, aWStr.c_str(), aBufSize);
        std::string aCodec = Config_Translator::codec("");
        aText = QTextCodec::codecForName(aCodec.c_str())->toUnicode(aMBStr);
      } else {
        std::string aMsg = aStrAttr->value();
        aText = ModuleBase_Tools::translate(myFeature->getKind(), aMsg);
      }
    }
    myLabel->setText(aText);
  }
  return true;
}

bool ModuleBase_WidgetLabel::focusTo()
{
  restoreValue();
  return false;
}
