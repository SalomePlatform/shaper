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

#include "ModuleBase_Dialog.h"
#include "ModuleBase_WidgetFactory.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_PageWidget.h"
#include "ModuleBase_ModelDialogWidget.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <Events_Loop.h>

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QMainWindow>
#include <QLayout>
#include <QDialogButtonBox>
#include <QPushButton>


ModuleBase_Dialog::ModuleBase_Dialog(ModuleBase_IWorkshop* theParent,
  const std::string& theDescription) :
  QDialog(theParent->desktop(),
    Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
  myDescription(theDescription),
  myWorkshop(theParent),
  myActiveWidget(0)
{
  Config_WidgetAPI aApi(myDescription, "");
  myId = aApi.getProperty("id");

  std::shared_ptr<Config_FeatureMessage> aFeatureInfo = myWorkshop->featureInfo(myId.c_str());
  myHelpPage = aFeatureInfo->helpFileName();

  ModuleBase_WidgetFactory aFactory(myDescription, myWorkshop);
  QString aTitle = ModuleBase_Tools::translate("ModuleBase_Dialog",
      aFactory.widgetAPI()->getProperty(FEATURE_TEXT));

  setWindowTitle(aTitle);

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  myFeature = aDoc->addFeature(myId);
  if (!myFeature.get())
    return;
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(1);

  ModuleBase_PageWidget* aPage = new ModuleBase_PageWidget(this);
  aLayout->addWidget(aPage);

  aFactory.createWidget(aPage, false);
  myWidgets = aFactory.getModelWidgets();

  QFrame* aFrame = new QFrame(this);
  aFrame->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
  aLayout->addWidget(aFrame);

  QHBoxLayout* aBtnLayout = new QHBoxLayout(aFrame);
  ModuleBase_Tools::adjustMargins(aBtnLayout);

  myButtonsBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help,
    Qt::Horizontal, aFrame);
  aBtnLayout->addWidget(myButtonsBox);

  myButtonsBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":pictures/button_ok.png"));
  myButtonsBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":pictures/button_cancel.png"));
  myButtonsBox->button(QDialogButtonBox::Help)->setIcon(QIcon(":pictures/button_help.png"));

  connect(myButtonsBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(myButtonsBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(myButtonsBox, SIGNAL(helpRequested()), this, SLOT(onHelpRequest()));

  foreach (ModuleBase_ModelWidget* aWidget, myWidgets) {
    initializeWidget(aWidget);
  }
}

void ModuleBase_Dialog::initializeWidget(ModuleBase_ModelWidget* theWidget)
{
  ModuleBase_ModelDialogWidget* aDlgWgt = dynamic_cast<ModuleBase_ModelDialogWidget*>(theWidget);
  if (aDlgWgt)
    aDlgWgt->setDialogButtons(myButtonsBox);

  theWidget->setFeature(myFeature);
  theWidget->restoreValue();
}

void ModuleBase_Dialog::showEvent(QShowEvent* theEvent)
{
  QDialog::showEvent(theEvent);
  if (myWidgets.length() > 0) {
    myActiveWidget = myWidgets.first();
    myActiveWidget->activate();
  }
}

void ModuleBase_Dialog::accept()
{
  if (myActiveWidget) {
    if (!myActiveWidget->storeValue())
      return;
  }
  QDialog::accept();
}

void ModuleBase_Dialog::onHelpRequest()
{
  myWorkshop->showHelpPage(myHelpPage.c_str());
}
