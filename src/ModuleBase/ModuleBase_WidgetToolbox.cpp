/*
 * ModuleBase_WidgetToolbox.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: sbh
 */

#include <ModuleBase_WidgetToolbox.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ToolBox.h>

#include <ModelAPI_AttributeString.h>

#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QIcon>

ModuleBase_WidgetToolbox::ModuleBase_WidgetToolbox(QWidget* theParent, const Config_WidgetAPI* theData,
                                                   const std::string& theParentId)
: ModuleBase_PagedContainer(theParent, theData, theParentId)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aMainLayout);
  myToolBox = new ModuleBase_ToolBox(this);
  // Dark-grey rounded tabs with button-like border #and bold font
  // TODO: apply style to custom widget
  QString css = "QToolBox::tab{background-color:#c8c8c8;"
                              "border-radius:5px;"
                              "border:1px inset;"
                              //"font-weight:700;"
                              "border-color:#fff #505050 #505050 #fff;}";
  myToolBox->setStyleSheet(css);
  // default vertical size policy is preferred
  aMainLayout->addWidget(myToolBox);

  connect(myToolBox, SIGNAL(currentChanged(int)), this, SLOT(onPageChanged()));
}

ModuleBase_WidgetToolbox::~ModuleBase_WidgetToolbox()
{
}

int ModuleBase_WidgetToolbox::addPage(ModuleBase_PageBase* thePage,
                                      const QString& theName,
                                      const QString& theCaseId,
                                      const QPixmap& theIcon )
{
  ModuleBase_PagedContainer::addPage(thePage, theName, theCaseId, theIcon);
  QFrame* aFrame = dynamic_cast<QFrame*>(thePage);
  aFrame->setFrameShape(QFrame::Box);
  aFrame->setFrameStyle(QFrame::Sunken);
  myToolBox->addItem(aFrame, theName, theIcon );
  return myToolBox->count();
}

int ModuleBase_WidgetToolbox::currentPageIndex() const
{
  return myToolBox->currentIndex();
}

void ModuleBase_WidgetToolbox::setCurrentPageIndex(int theIndex)
{
  bool isSignalsBlocked = myToolBox->blockSignals(true);
  myToolBox->setCurrentIndex(theIndex);
  myToolBox->blockSignals(isSignalsBlocked);
}


