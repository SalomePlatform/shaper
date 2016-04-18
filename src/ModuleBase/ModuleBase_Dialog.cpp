// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "ModuleBase_Dialog.h"
#include "ModuleBase_WidgetFactory.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_PageWidget.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <Events_Loop.h>

#include <QMainWindow>
#include <QLayout>
#include <QDialogButtonBox>
#include <QPushButton>


ModuleBase_Dialog::ModuleBase_Dialog(ModuleBase_IWorkshop* theParent, const QString& theId, 
                                     const std::string& theDescription) : 
                                     QDialog(theParent->desktop()), 
                                     myId(theId), 
                                     myDescription(theDescription), 
                                     myWorkshop(theParent),
                                     myActiveWidget(0)
{
  ModuleBase_WidgetFactory aFactory(myDescription, myWorkshop);

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  aMgr->startOperation(myId.toStdString());
  myFeature = aDoc->addFeature(myId.toStdString());
  if (!myFeature.get())
    return;
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(1);
  //setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

  ModuleBase_PageWidget* aPage = new ModuleBase_PageWidget(this);
  aLayout->addWidget(aPage);

  aFactory.createWidget(aPage, false);
  myWidgets = aFactory.getModelWidgets();
  foreach (ModuleBase_ModelWidget* aWidget, myWidgets) {
    initializeWidget(aWidget);
  }

  QFrame* aFrame = new QFrame(this);
  aFrame->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
  aLayout->addWidget(aFrame);

  QVBoxLayout* aBtnLayout = new QVBoxLayout(aFrame);
  ModuleBase_Tools::adjustMargins(aBtnLayout);

  QDialogButtonBox* aBtnBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, aFrame);
  aBtnLayout->addWidget(aBtnBox);

  aBtnBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":pictures/button_ok.png"));
  aBtnBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":pictures/button_cancel.png"));

  connect(aBtnBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aBtnBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void ModuleBase_Dialog::initializeWidget(ModuleBase_ModelWidget* theWidget)
{
  theWidget->setFeature(myFeature);
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
