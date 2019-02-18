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

#include <XGUI_ErrorDialog.h>

#include <ModuleBase_Tools.h>
#include <Config_Translator.h>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTextCodec>

XGUI_ErrorDialog::XGUI_ErrorDialog(QWidget* parent)
    : QDialog(parent)
{
  QVBoxLayout* aDlgLay = new QVBoxLayout(this);
  setWindowTitle(tr("Application errors"));
  myErrorLog = new QTextEdit(this);
  myErrorLog->setReadOnly(true);
  aDlgLay->addWidget(myErrorLog);
  QDialogButtonBox* aButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal,
                                                      this);
  aDlgLay->addWidget(aButtonBox);
  aDlgLay->setContentsMargins(2, 2, 2, 2);
  aDlgLay->setSpacing(2);
  setLayout(aDlgLay);
  resize(420, 240);

  connect(aButtonBox, SIGNAL(accepted()), this, SLOT(clear()));
  connect(aButtonBox, SIGNAL(rejected()), this, SLOT(clear()));
}

XGUI_ErrorDialog::~XGUI_ErrorDialog()
{
}

void XGUI_ErrorDialog::refresh()
{
  myErrorLog->clear();
  foreach(QString eachError, myErrors)
  {
    myErrorLog->append(eachError);
  }
}

void XGUI_ErrorDialog::clear()
{
  myErrorLog->clear();
  myErrors.clear();
  QDialog::reject();
}

void XGUI_ErrorDialog::addError(std::shared_ptr<Events_InfoMessage> theMsg)
{
  std::string aError = Config_Translator::translate(*theMsg);
  std::string aCodec = Config_Translator::codec(*theMsg);
  QString aMsg = QTextCodec::codecForName(aCodec.c_str())->toUnicode(aError.c_str());
  myErrors.append(aMsg);
  refresh();
  if (!isVisible()) {
    show();
    ModuleBase_Tools::activateWindow(this, "XGUI_ErrorDialog::addError");
  }
}

void XGUI_ErrorDialog::removeError(const QString& theError)
{
  myErrors.removeAll(theError);
  refresh();
}
