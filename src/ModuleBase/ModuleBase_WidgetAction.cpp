// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <ModuleBase_WidgetAction.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IconFactory.h>

#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>

#define DEFAULT_TOOL_TIP_WIDTH 300

ModuleBase_WidgetAction::ModuleBase_WidgetAction(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData),
  myActionID(attributeID())
{
  setAttributeID(""); // To prevent errors. Action not stored as attribtue in feature.
  QHBoxLayout* aControlLay = new QHBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aControlLay);

  myButton = new QToolButton(this);
  QString aText = translate(theData->widgetLabel());
  QString aToolTip = ModuleBase_Tools::wrapTextByWords(translate(theData->widgetTooltip()),
                                                       myButton,
                                                       DEFAULT_TOOL_TIP_WIDTH);

  myButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  aControlLay->addWidget(myButton);

  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  if (!aLabelIcon.isEmpty())
    myButton->setIcon(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
  else
    myButton->setText(aText);
  myButton->setToolTip(aToolTip);

  connect(myButton, SIGNAL(clicked(bool)), this, SLOT(onActionClicked()));
}

ModuleBase_WidgetAction::~ModuleBase_WidgetAction()
{
}

bool ModuleBase_WidgetAction::focusTo()
{
  return false;
}

QList<QWidget*> ModuleBase_WidgetAction::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myButton);
  return aList;
}

bool ModuleBase_WidgetAction::storeValueCustom()
{
  return true;
}

bool ModuleBase_WidgetAction::restoreValueCustom()
{
  return true;
}

void ModuleBase_WidgetAction::onActionClicked()
{
  if (myFeature->customAction(myActionID))
    updateObject(myFeature);
}
