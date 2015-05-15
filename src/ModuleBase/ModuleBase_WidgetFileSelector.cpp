// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetFileSelector.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModuleBase_WidgetFileSelector.h>
#include <ModuleBase_Tools.h>

#include <Config_WidgetAPI.h>

#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QObject>
#include <QPushButton>
#include <QRegExp>
#include <QString>

#include <memory>
#include <string>

ModuleBase_WidgetFileSelector::ModuleBase_WidgetFileSelector(QWidget* theParent,
                                                             const Config_WidgetAPI* theData,
                                                             const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  myTitle = QString::fromStdString(theData->getProperty("title"));
  myType = (theData->getProperty("type") == "save") ? WFS_SAVE : WFS_OPEN;
  myDefaultPath = QString::fromStdString(theData->getProperty("path"));

  QGridLayout* aMainLay = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QLabel* aTitleLabel = new QLabel(myTitle, this);
  aTitleLabel->setIndent(1);
  aMainLay->addWidget(aTitleLabel, 0, 0);
  myPathField = new QLineEdit(this);
  aMainLay->addWidget(myPathField, 1, 0);
  QPushButton* aSelectPathBtn = new QPushButton("...", this);
  aSelectPathBtn->setToolTip(tr("Select file..."));
  aSelectPathBtn->setMaximumWidth(20);
  aSelectPathBtn->setMaximumHeight(20);
  aMainLay->addWidget(aSelectPathBtn, 1, 1);
  aMainLay->setColumnStretch(0, 1);
  myPathField->setMinimumHeight(20);
  aMainLay->setHorizontalSpacing(1);
  this->setLayout(aMainLay);

  connect(myPathField, SIGNAL(textChanged(const QString&)),
          this,        SLOT(onPathChanged()));
  connect(aSelectPathBtn, SIGNAL(clicked()),
          this,        SLOT(onPathSelectionBtn()));
}

ModuleBase_WidgetFileSelector::~ModuleBase_WidgetFileSelector()
{
}

bool ModuleBase_WidgetFileSelector::storeValueCustom() const
{
  // A rare case when plugin was not loaded. 
  if (!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myPathField->text();
  aStringAttr->setValue(aWidgetValue.toStdString());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetFileSelector::restoreValue()
{
  // A rare case when plugin was not loaded. 
  if (!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myPathField->blockSignals(true);
  myPathField->setText(QString::fromStdString(aStringAttr->value()));
  myPathField->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetFileSelector::getControls() const
{
  QList<QWidget*> result;
  //QPushButton * aButton = this->findChild<QPushButton *>();
  //result << aButton;
  result << myPathField;
  return result;
}

bool ModuleBase_WidgetFileSelector::isCurrentPathValid()
{
  QFileInfo aFile(myPathField->text());
  return aFile.exists();
}

void ModuleBase_WidgetFileSelector::onPathSelectionBtn()
{
  QString aDefaultPath = myPathField->text().isEmpty()
      ? myDefaultPath
      : QFileInfo(myPathField->text()).absolutePath();
  QString aFilter = filterString();
  QString aFileName = (myType == WFS_SAVE)
      ? QFileDialog::getSaveFileName(this, myTitle, aDefaultPath, aFilter, &mySelectedFilter)
      : QFileDialog::getOpenFileName(this, myTitle, aDefaultPath, aFilter, &mySelectedFilter);
  if (!aFileName.isEmpty()) {
    myPathField->setText(aFileName);
  }
}

void ModuleBase_WidgetFileSelector::onPathChanged()
{
  if (myType == WFS_OPEN && !isCurrentPathValid())
    return;
  storeValue();
  emit valuesChanged();
}

QString ModuleBase_WidgetFileSelector::formatToFilter( const QString & theFormat )
{
  if (theFormat.isEmpty() && !theFormat.contains(":"))
    return QString();

  QStringList aExtesionList = theFormat.section(':', 0, 0).split("|");
  QString aFormat = theFormat.section(':', 1, 1);
  return QString("%1 files (%2)").arg(aFormat)
      .arg(QStringList(aExtesionList).replaceInStrings(QRegExp("^(.*)$"), "*.\\1").join(" "));
}

QStringList ModuleBase_WidgetFileSelector::getValidatorFormats() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> allValidators;
  std::list<std::list<std::string> > allArguments;
  aFactory->validators(myFeature->getKind(), myAttributeID, allValidators, allArguments);
  QStringList aResult;
  std::list<std::string> anArgumentList = allArguments.front();
  std::list<std::string>::const_iterator it = anArgumentList.begin();
  for (; it != anArgumentList.end(); ++it) {
    QString aFormat = QString::fromStdString(*it);
    if (!aFormat.isEmpty())
      aResult << aFormat;
  }
  return aResult;
}

QString ModuleBase_WidgetFileSelector::filterString() const
{
  QStringList aResult;
  QStringList aValidatorFormats = getValidatorFormats();

  foreach(const QString & eachFormat, aValidatorFormats) {
    aResult << formatToFilter(eachFormat);
  }
  if (myType == WFS_OPEN)
    aResult << QString("All files (*.*)");
  return aResult.join(";;");
}
