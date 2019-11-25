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

#include "ModuleBase_WidgetUndoLabel.h"
#include "ModuleBase_IWorkshop.h"

#include <QPushButton>
#include <QLayout>
#include <QString>
#include <QLabel>

ModuleBase_WidgetUndoLabel::ModuleBase_WidgetUndoLabel(QWidget* theParent,
  ModuleBase_IWorkshop* theWorkshop,
  const Config_WidgetAPI* theData)
  : ModuleBase_WidgetLabel(theParent, theData),
  myWorkshop(theWorkshop)
{
  myUndoBtn = new QPushButton(tr("Undo"), this);
  myUndoBtn->hide();
  layout()->addWidget(myUndoBtn);
  connect(myUndoBtn, SIGNAL(clicked(bool)), SLOT(onUndo()));
}


bool ModuleBase_WidgetUndoLabel::restoreValueCustom()
{
  bool aRes = ModuleBase_WidgetLabel::restoreValueCustom();
  bool aError = myLabel->text().length() > 0;
  myUndoBtn->setVisible(aError);
  myWorkshop->setCancelEnabled(!aError);
  return aRes;
}


void ModuleBase_WidgetUndoLabel::onUndo()
{
  myWorkshop->undo();
}