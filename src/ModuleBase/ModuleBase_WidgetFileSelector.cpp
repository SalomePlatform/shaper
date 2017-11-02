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

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModuleBase_WidgetFileSelector.h>
#include <ModuleBase_Tools.h>

#include <Config_PropManager.h>
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
                                                             const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  myTitle = translate(theData->getProperty("title"));
  myType = (theData->getProperty("type") == "save") ? WFS_SAVE : WFS_OPEN;
  myDefaultPath = QString::fromStdString(theData->getProperty("path"));

  if (myDefaultPath.isEmpty())
    myDefaultPath = Config_PropManager::string("Plugins", "import_initial_path").c_str();

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

bool ModuleBase_WidgetFileSelector::storeValueCustom()
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

bool ModuleBase_WidgetFileSelector::restoreValueCustom()
{
  // A rare case when plugin was not loaded.
  if (!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myPathField->blockSignals(true);
  QString aNewText = QString::fromStdString(aStringAttr->value());
  if( myPathField->text() != aNewText )
    myPathField->setText( aNewText );
  myPathField->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetFileSelector::getControls() const
{
  QList<QWidget*> result;
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
  // use Option prohibited native dialog using to have both lower/upper extensions of files
  // satisfied to dialog filter on Linux(Calibre) Issue #2055
  QString aFileName = (myType == WFS_SAVE)
      ? QFileDialog::getSaveFileName(this, myTitle, aDefaultPath, aFilter, &mySelectedFilter,
                                     QFileDialog::DontUseNativeDialog)
      : QFileDialog::getOpenFileName(this, myTitle, aDefaultPath, aFilter, &mySelectedFilter,
                                     QFileDialog::DontUseNativeDialog);
  if (!aFileName.isEmpty()) {
    if (myType == WFS_SAVE)
      aFileName = applyExtension(aFileName, mySelectedFilter);
    myPathField->setText(aFileName);
    emit focusOutWidget(this);
  }
}

void ModuleBase_WidgetFileSelector::onPathChanged()
{
  if (myType == WFS_OPEN && !isCurrentPathValid())
    return;
  storeValue();
  emit valuesChanged();
}

QString ModuleBase_WidgetFileSelector::formatToFilter(const QString & theFormat)
{
  if (theFormat.isEmpty() && !theFormat.contains(":"))
    return QString();

  QStringList aExtesionList = theFormat.section(':', 0, 0).split("|");
  QString aFormat = theFormat.section(':', 1, 1);
  return QString("%1 files (%2)").arg(aFormat)
      .arg(QStringList(aExtesionList).replaceInStrings(QRegExp("^(.*)$"), "*.\\1").join(" "));
}

QString ModuleBase_WidgetFileSelector::filterToShortFormat(const QString & theFilter)
{
  // Simplified implementation.
  // It relies on theFilter was made by formatToFilter() function.
  return theFilter.section(' ', 0, 0);
}

QStringList ModuleBase_WidgetFileSelector::filterToExtensions(const QString & theFilter)
{
  // Simplified implementation.
  // It relies on theFilter was made by formatToFilter() function.
  QStringList anExtensions = theFilter.section("(", 1, 1).section(")", 0, 0).split(" ");
  return anExtensions;
}

QStringList ModuleBase_WidgetFileSelector::getValidatorFormats() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  ModelAPI_ValidatorsFactory::Validators allValidators;
  aFactory->validators(myFeature->getKind(), myAttributeID, allValidators);

  QStringList aResult;
  std::list<std::string> anArgumentList = allValidators.front().second;
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

QString ModuleBase_WidgetFileSelector::applyExtension(const QString& theFileName,
                                                      const QString& theFilter)
{
  QString aResult = theFileName;
  bool hasExtension = false;
  QStringList anExtensions = filterToExtensions(theFilter);
  foreach(const QString& anExtension, anExtensions) {
    if (theFileName.endsWith(anExtension.section(".", 1, 1), Qt::CaseInsensitive)) {
      hasExtension = true;
      break;
    }
  }
  if (!hasExtension && !anExtensions.isEmpty())
    aResult = QString("%1.%2").arg(theFileName).arg(anExtensions[0].section(".", 1, 1));
  return aResult;
}
