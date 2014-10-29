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

#include <QGridLayout>
#include <QFileDialog>
#include <QLineEdit>
#include <QList>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QLabel>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>

ModuleBase_WidgetFileSelector::ModuleBase_WidgetFileSelector(QWidget* theParent,
                                                             const Config_WidgetAPI* theData,
                                                             const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  myTitle = QString::fromStdString(theData->getProperty("title"));
  myDefaultPath = QString::fromStdString(theData->getProperty("path"));

  myMainWidget = new QWidget(theParent);
  QGridLayout* aMainLay = new QGridLayout(myMainWidget);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QLabel* aTitleLabel = new QLabel(myTitle, myMainWidget);
  aTitleLabel->setIndent(1);
  aMainLay->addWidget(aTitleLabel, 0, 0);
  myPathField = new QLineEdit(myMainWidget);
  aMainLay->addWidget(myPathField, 1, 0);
  QPushButton* aSelectPathBtn = new QPushButton("...", myMainWidget);
  aSelectPathBtn->setMaximumWidth(20);
  aSelectPathBtn->setMaximumHeight(20);
  aMainLay->addWidget(aSelectPathBtn, 1, 1);
  aMainLay->setColumnStretch(0, 1);
  myPathField->setMinimumHeight(20);
  aMainLay->setHorizontalSpacing(1);
  myMainWidget->setLayout(aMainLay);

  connect(myPathField, SIGNAL(textChanged(const QString&)),
          this,        SLOT(onPathChanged()));
  connect(aSelectPathBtn, SIGNAL(clicked()),
          this,        SLOT(onPathSelectionBtn()));
}

ModuleBase_WidgetFileSelector::~ModuleBase_WidgetFileSelector()
{
}

bool ModuleBase_WidgetFileSelector::storeValue() const
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
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
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myPathField->blockSignals(true);
  myPathField->setText(QString::fromStdString(aStringAttr->value()));
  myPathField->blockSignals(isBlocked);

  return true;
}

QWidget* ModuleBase_WidgetFileSelector::getControl() const
{
  return myMainWidget;
}

QList<QWidget*> ModuleBase_WidgetFileSelector::getControls() const
{
  QList<QWidget*> result;
  QPushButton * aButton = myMainWidget->findChild<QPushButton *>();
  result << aButton;
  result << myPathField;
  return result;
}

bool ModuleBase_WidgetFileSelector::isCurrentPathValid()
{
  QFileInfo aFile (myPathField->text());
  return aFile.exists();
}


void ModuleBase_WidgetFileSelector::onPathSelectionBtn()
{
  QString aFilter = formatsString();
  QString aFileName = QFileDialog::getOpenFileName(myMainWidget, myTitle, myDefaultPath, aFilter);
  if (!aFileName.isEmpty()) {
    myPathField->setText(aFileName);
  }
}

void ModuleBase_WidgetFileSelector::onPathChanged()
{
  if(!isCurrentPathValid())
    return;
  storeValue();
  emit valuesChanged();
}

QString ModuleBase_WidgetFileSelector::formatsString() const
{
  QStringList aResult;
  QStringList aValidatorFormats = getValidatorFormats();

  foreach(QString eachFormat, aValidatorFormats)  {
    aResult << QString("%1 files (*.%1)").arg(eachFormat);
  }
  aResult << QString("All files (*.*)");
  return aResult.join(";;");
}

QStringList ModuleBase_WidgetFileSelector::getValidatorFormats() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> allValidators;
  std::list<std::list<std::string> > allArguments;
  aFactory->validators(myFeature->getKind(), myAttributeID, allValidators, allArguments);
  //TODO(sbh): extract as separate method
  if(allArguments.empty())
    return QStringList();
  std::list<std::string> anArgumentList = allArguments.front();
  std::list<std::string>::const_iterator it = anArgumentList.begin();
  QStringList aResult;
  for (; it != anArgumentList.end(); ++it) {
    std::string anArg = *it;
    int aSepPos = anArg.find(":");
    if (aSepPos == std::string::npos) {
      continue;
    }
    QString aFormat = QString::fromStdString(anArg.substr(0, aSepPos));
    aFormat = aFormat.toUpper();
    aResult.append(aFormat);
  }
  return aResult;
}
