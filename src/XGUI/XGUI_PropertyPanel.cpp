/*
 * XGUI_PropertyPanel.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#include <XGUI_Constants.h>
#include <XGUI_PropertyPanel.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QEvent>
#include <QKeyEvent>

#ifdef _DEBUG
#include <iostream>
#endif

XGUI_PropertyPanel::XGUI_PropertyPanel(QWidget* theParent)
{
  this->setWindowTitle(tr("Property Panel"));
  QAction* aViewAct = this->toggleViewAction();
  this->setObjectName(XGUI::PROP_PANEL);
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  QWidget* aContent = new QWidget(this);
  QVBoxLayout* aMainLay = new QVBoxLayout(aContent);
  aMainLay->setContentsMargins(3, 3, 3, 3);
  this->setWidget(aContent);

  QFrame* aFrm = new QFrame(aContent);
  aFrm->setFrameStyle(QFrame::Sunken);
  aFrm->setFrameShape(QFrame::Panel);
  QHBoxLayout* aBtnLay = new QHBoxLayout(aFrm);
  aBtnLay->setContentsMargins(0, 0, 0, 0);
  aMainLay->addWidget(aFrm);

  QPushButton* aBtn = new QPushButton(QIcon(":pictures/button_help.png"), "", aFrm);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);
  aBtnLay->addStretch(1);
  aBtn = new QPushButton(QIcon(":pictures/button_ok.png"), "", aFrm);
  aBtn->setObjectName(XGUI::PROP_PANEL_OK);
  aBtn->setToolTip(tr("Ok"));
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);
  aBtn->installEventFilter(this);

  aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
  aBtn->setToolTip(tr("Cancel"));
  aBtn->setObjectName(XGUI::PROP_PANEL_CANCEL);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);

  myCustomWidget = new QWidget(aContent);
  aMainLay->addWidget(myCustomWidget);
  aMainLay->addStretch(1);

  aBtn->installEventFilter(this);
}

XGUI_PropertyPanel::~XGUI_PropertyPanel()
{
}

void XGUI_PropertyPanel::setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets = theWidgets;

  if (!theWidgets.empty()) {
    QList<ModuleBase_ModelWidget*>::const_iterator anIt = theWidgets.begin(), aLast = theWidgets.end();
    for (; anIt != aLast; anIt++) {
      connect(*anIt, SIGNAL(keyReleased(const std::string&, QKeyEvent*)),
              this, SIGNAL(keyReleased(const std::string&, QKeyEvent*)));
    }
    ModuleBase_ModelWidget* aLastWidget = theWidgets.last();
    if (aLastWidget) {
      QList<QWidget*> aControls = aLastWidget->getControls();
      if (!aControls.empty()) {
        QWidget* aLastControl = aControls.last();

        QPushButton* anOkBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
        QPushButton* aCancelBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);

        setTabOrder(aLastControl, anOkBtn);
        setTabOrder(anOkBtn, aCancelBtn);
      }
    }
    ModuleBase_ModelWidget* aWidget = theWidgets.first();
    if (aWidget)
      aWidget->focusTo();
  }
}

bool XGUI_PropertyPanel::eventFilter(QObject *theObject, QEvent *theEvent)
{
  QPushButton* anOkBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
  QPushButton* aCancelBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);
  if (theObject == anOkBtn || theObject == aCancelBtn) {
    if (theEvent->type() == QEvent::KeyRelease) {
      QKeyEvent* aKeyEvent = (QKeyEvent*)theEvent;
      if (aKeyEvent && aKeyEvent->key() == Qt::Key_Return) {
        // TODO: this is enter button processing when the focus is on "Apply" or "Cancel" buttons
        emit keyReleased("", (QKeyEvent*) theEvent);
        return true;
      }
    }
  }
  return QDockWidget::eventFilter(theObject, theEvent);
}

QWidget* XGUI_PropertyPanel::contentWidget()
{
  return myCustomWidget;
}

void XGUI_PropertyPanel::updateContentWidget(FeaturePtr theFeature)
{
  foreach(ModuleBase_ModelWidget* eachWidget, myWidgets) {
    eachWidget->restoreValue(theFeature);
  }
  // the repaint is used here to immediatelly react in GUI to the values change.
  repaint();
}

void XGUI_PropertyPanel::onFocusActivated(const std::string& theAttributeName)
{
  if (theAttributeName == XGUI::PROP_PANEL_OK) {
    QPushButton* aBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
    aBtn->setFocus();
  }
  if (theAttributeName == XGUI::PROP_PANEL_CANCEL) {
    QPushButton* aBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);
    aBtn->setFocus();
  }
  else {
    foreach(ModuleBase_ModelWidget* eachWidget, myWidgets) {
      if (eachWidget->canFocusTo(theAttributeName)) {
        eachWidget->focusTo();
        break;
      }
    }
  }
}
