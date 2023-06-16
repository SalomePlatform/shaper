// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include <ModuleBase_WidgetLineEdit.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IconFactory.h>

#include <Locale_Convert.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>

#include <Config_WidgetAPI.h>

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QPainter>
#include <QResizeEvent>

#include <memory>
#include <string>

/**
* Customization of Line edit control
*/
class CustomLineEdit : public QLineEdit
{
public:
  /// Constructor
  /// \param theParent a parent widget
  /// \param thePlaceHolder a string which is shown when text is empty
  CustomLineEdit( QWidget* theParent, const QString& thePlaceHolder )
    : QLineEdit( theParent ), myPlaceHolder( thePlaceHolder )
  {
  }

  virtual ~CustomLineEdit()
  {
  }

  /// Redefiniotion of virtual method
  /// \param theEvent a paint event
  virtual void paintEvent( QPaintEvent* theEvent )
  {
    QLineEdit::paintEvent( theEvent );
    if( text().isEmpty() && !myPlaceHolder.isEmpty() )
    {
      QPainter aPainter( this );
      QRect aRect = rect();
      int aHorMargin = 5;
      aRect.adjust( aHorMargin, 0, 0, 0 );

      QColor aColor = palette().text().color();
      aColor.setAlpha( 128 );
      QPen anOldpen = aPainter.pen();
      aPainter.setPen( aColor );
      QFontMetrics aFontMetrics = fontMetrics();
      QString elidedText = aFontMetrics.elidedText( myPlaceHolder, Qt::ElideRight, aRect.width() );
      aPainter.drawText( aRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText );
      aPainter.setPen( anOldpen );
    }
  }

private:
  QString myPlaceHolder;
};

ModuleBase_WidgetLineEdit::ModuleBase_WidgetLineEdit(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& thePlaceHolder )
: ModuleBase_ModelWidget(theParent, theData)
{
  QFormLayout* aMainLay = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QString aLabelText = translate(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  QLabel* aLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    aLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));

  myLineEdit = new CustomLineEdit( this, translate( thePlaceHolder ) );
  // Here we do not use the Qt's standard method setPlaceHolderText() since it
  // draws the place holder only if there is no focus on widget;
  // we would like to see the place holder in the case of empty text
  // even if the widget is focused.
  // The corresponding patch appears in Qt only since version 5.x

  myLineEdit->setMinimumHeight(20);

  aMainLay->addRow(aLabel, myLineEdit);
  this->setLayout(aMainLay);

  connect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
}

ModuleBase_WidgetLineEdit::~ModuleBase_WidgetLineEdit()
{
}

bool ModuleBase_WidgetLineEdit::storeValueCustom()
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myLineEdit->text();
  aStringAttr->setValue(aWidgetValue.toStdWString());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetLineEdit::restoreValueCustom()
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myLineEdit->blockSignals(true);
  QString aText;
  if (aStringAttr->isUValue())
    aText = QString::fromStdWString(Locale::Convert::toWString(aStringAttr->valueU()));
  else
    aText = QString::fromStdString(aStringAttr->value());
  myLineEdit->setText(aText);
  myLineEdit->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetLineEdit::getControls() const
{
  QList<QWidget*> result;
  result << myLineEdit;
  return result;
}

bool ModuleBase_WidgetLineEdit::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    myLineEdit->selectAll();
  }
  return isModified;
}

bool ModuleBase_WidgetLineEdit::isModified() const
{
  return !myLineEdit->text().isEmpty();
}