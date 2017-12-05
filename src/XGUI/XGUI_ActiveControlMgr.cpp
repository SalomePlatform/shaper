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

#include <XGUI_ActiveControlMgr.h>
#include <XGUI_ActiveControlSelector.h>

#include <ModuleBase_IModule.h>
#include <ModuleBase_IWorkshop.h>

//********************************************************************
XGUI_ActiveControlMgr::XGUI_ActiveControlMgr(ModuleBase_IWorkshop* theWorkshop)
: myWorkshop(theWorkshop), myActiveSelector(0)
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

//********************************************************************
void XGUI_ActiveControlMgr::addSelector(XGUI_ActiveControlSelector* theSelector)
{
  mySelectors.append(theSelector);
  connect(theSelector, SIGNAL(activated()), this, SLOT(onSelectorActivated()));
  connect(theSelector, SIGNAL(deactivated()), this, SLOT(onSelectorDeactivated()));
}

//********************************************************************
XGUI_ActiveControlSelector* XGUI_ActiveControlMgr::getSelector(const QString& theType)
{
  XGUI_ActiveControlSelector* aSelector;
  for (int i = 0, aCount = mySelectors.count(); i < aCount; i++)
  {
    if (mySelectors[i]->getType() != theType)
      continue;
    aSelector = mySelectors[i];
    break;
  }
  return aSelector;
}

//********************************************************************
void XGUI_ActiveControlMgr::onSelectorActivated()
{
  XGUI_ActiveControlSelector* aSelector = qobject_cast<XGUI_ActiveControlSelector*>(sender());
  if (!aSelector || aSelector == myActiveSelector)
    return;

  if (myActiveSelector)
    myActiveSelector->setActive(false);

  activateSelector(aSelector);
}

//********************************************************************
void XGUI_ActiveControlMgr::onSelectorDeactivated()
{
  XGUI_ActiveControlSelector* aSelector = qobject_cast<XGUI_ActiveControlSelector*>(sender());
  if (!aSelector)
    return;

  myActiveSelector = NULL;

  aSelector->setActive(false);
  myWorkshop->module()->updateActiveSelectionFilters();

  XGUI_ActiveControlSelector* aSelectorToBeActivated = 0;
  for (int i = 0, aCount = mySelectors.count(); i < aCount; i++)
  {
    if (!mySelectors[i]->needToBeActiated())
      continue;
    aSelectorToBeActivated = mySelectors[i];
    break;
  }
  if (aSelectorToBeActivated)
    activateSelector(aSelectorToBeActivated);
}

//********************************************************************
void XGUI_ActiveControlMgr::onSelectionChanged()
{
  if (!myActiveSelector)
    return;

  myActiveSelector->processSelection();
}

//********************************************************************
void XGUI_ActiveControlMgr::activateSelector(XGUI_ActiveControlSelector* theSelector)
{
  myActiveSelector = theSelector;
  theSelector->setActive(true);

  myWorkshop->module()->updateActiveSelectionFilters();
}
