// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_IErrorMgr.h"

#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ModelWidget.h"

ModuleBase_IErrorMgr::ModuleBase_IErrorMgr(QObject* theParent /*= 0*/)
  : QObject(theParent)
  , myPropertyPanel(NULL)
{

}

ModuleBase_IErrorMgr::~ModuleBase_IErrorMgr()
{

}

void ModuleBase_IErrorMgr::setPropertyPanel(ModuleBase_IPropertyPanel* theProp)
{
  myPropertyPanel = theProp;

  if (myPropertyPanel) {
    foreach(const ModuleBase_ModelWidget* aWgt, myPropertyPanel->modelWidgets()) {
      connect(aWgt, SIGNAL(afterValuesChanged()), this, SLOT(onWidgetChanged()));
      connect(aWgt, SIGNAL(afterValuesRestored()), this, SLOT(onWidgetChanged()));
      connect(aWgt, SIGNAL(valueStateChanged(int)), this, SLOT(onWidgetChanged()));
    }
  }
}
