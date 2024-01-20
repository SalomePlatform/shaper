// Copyright (C) 2014-2024  CEA, EDF
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

#include <ModuleBase_WidgetExprEditor.h>
#include <ModuleBase_Tools.h>

#include <Locale_Convert.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>

#include <Config_WidgetAPI.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QStringListModel>
#include <QCompleter>
#include <QSize>
#include <QShortcut>
#include <QScrollBar>
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>
#include <QAbstractItemView>

#include <memory>
#include <string>

ExpressionEditor::ExpressionEditor(QWidget* theParent)
: QPlainTextEdit(theParent), myCompletedAndSelected(false)
{
  myCompleter = new QCompleter(this);
  myCompleter->setWidget(this);
  myCompleter->setCompletionMode(QCompleter::PopupCompletion);

  myCompleterModel = new QStringListModel(this);
  myCompleter->setModel(myCompleterModel);
  // Use sorted model to accelerate completion (QCompleter will use binary search)
  myCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
  myCompleter->setCaseSensitivity(Qt::CaseInsensitive);

  connect(myCompleter, SIGNAL(activated(const QString&)),
          this,        SLOT(insertCompletion(const QString&)));
  (void) new QShortcut(QKeySequence(tr("Ctrl+Space", "Complete")),
                       this, SLOT(performCompletion()));

  connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

  setTabChangesFocus(true);
}

ExpressionEditor::~ExpressionEditor()
{

}

void ExpressionEditor::setCompletionList(QStringList& theList)
{
  theList.sort();
  theList.removeDuplicates();
  myCompleterModel->setStringList(theList);
}

void ExpressionEditor::insertCompletion(const QString& theCompletion, bool isSingleWord)
{
  QTextCursor aCursor = textCursor();
  int numberOfCharsToComplete = theCompletion.length() -
      myCompleter->completionPrefix().length();
  int insertionPosition = aCursor.position();
  aCursor.insertText(theCompletion.right(numberOfCharsToComplete));
  if (isSingleWord) {
    aCursor.setPosition(insertionPosition);
    aCursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    myCompletedAndSelected = true;
  }
  setTextCursor(aCursor);
}

void ExpressionEditor::performCompletion()
{
  QTextCursor aCursor = textCursor();
  aCursor.select(QTextCursor::WordUnderCursor);
  const QString aPrefix = aCursor.selectedText();
  performCompletion(aPrefix);
}

void ExpressionEditor::performCompletion(const QString& theCompletionPrefix)
{
  //populate model?
  if (theCompletionPrefix != myCompleter->completionPrefix()) {
    myCompleter->setCompletionPrefix(theCompletionPrefix);
    myCompleter->popup()->setCurrentIndex(myCompleter->completionModel()->index(0, 0));
  }
  if (myCompleter->completionCount() == 1) {
    insertCompletion(myCompleter->currentCompletion(), true);
  } else {
    QRect aRect = cursorRect();
    aRect.setWidth(myCompleter->popup()->sizeHintForColumn(0)
                  + myCompleter->popup()->verticalScrollBar()->sizeHint().width());
    myCompleter->complete(aRect);
  }
}

void ExpressionEditor::keyPressEvent(QKeyEvent* theEvent)
{
  if (myCompletedAndSelected && handledCompletedAndSelected(theEvent))
    return;
  myCompletedAndSelected = false;
  if (myCompleter->popup()->isVisible()) {
    switch (theEvent->key()) {
      case Qt::Key_Up:
      case Qt::Key_Down:
      case Qt::Key_Escape:
      case Qt::Key_Enter:
      case Qt::Key_Return:
        theEvent->ignore();
      return;
      default:
        myCompleter->popup()->hide();
        break;
    }
  }
  else {
    switch (theEvent->key()) {
      case Qt::Key_Enter:
      case Qt::Key_Return:
        emit keyReleased(this, theEvent);
        // do not react to the Enter key, the property panel processes it
        return;
      break;
      default:
        break;
    }
  }
  QPlainTextEdit::keyPressEvent(theEvent);
}

bool ExpressionEditor::handledCompletedAndSelected(QKeyEvent* theEvent)
{
  myCompletedAndSelected = false;
  QTextCursor aCursor = textCursor();
  switch (theEvent->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return: aCursor.clearSelection(); break;
    case Qt::Key_Escape: aCursor.removeSelectedText(); break;
    default: return false;
  }
  setTextCursor(aCursor);
  theEvent->accept();
  return true;
}

void ExpressionEditor::setPlaceHolderText( const QString& thePlaceHolderText )
{
  myPlaceHolderText = thePlaceHolderText;
}

QString ExpressionEditor::placeHolderText() const
{
  return myPlaceHolderText;
}

void ExpressionEditor::paintEvent( QPaintEvent* theEvent )
{
  QPlainTextEdit::paintEvent( theEvent );

  if( toPlainText().isEmpty() )
  {
    QPainter aPainter( viewport() );
    QFontMetrics aFontMetrics = fontMetrics();

    QPointF offset(contentOffset());
    QRect r = rect();
    int m = (int)document()->documentMargin();
    QRect lineRect( r.x() + m + offset.x(), offset.y(),
                    r.width() - 2*m, aFontMetrics.height() );

    Qt::Alignment va = QStyle::visualAlignment( layoutDirection(), Qt::AlignLeft );
    int minLB = qMax( 0, -aFontMetrics.minLeftBearing() );

    QColor aColor = palette().text().color();
    aColor.setAlpha( 128 );
    QPen anOldpen = aPainter.pen();
    aPainter.setPen( aColor );
    lineRect.adjust(minLB, 0, 0, 0);
    QString elidedText =
      aFontMetrics.elidedText( myPlaceHolderText, Qt::ElideRight, lineRect.width() );
    aPainter.drawText( lineRect, va, elidedText );
    aPainter.setPen( anOldpen );
  }
}

void ExpressionEditor::onTextChanged()
{
  emit valueModified();
}


ModuleBase_WidgetExprEditor::ModuleBase_WidgetExprEditor( QWidget* theParent,
                                                          const Config_WidgetAPI* theData,
                                                          const std::string& thePlaceHolder )
: ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aMainLay = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);

  myResultLabel = new QLabel(this);
  myResultLabel->setWordWrap(true);
  QFontMetrics fm(myResultLabel->font());
  myResultLabel->setMinimumHeight(fm.height() * 2); // set 2 line height as minimum
  myResultLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
  aMainLay->addWidget(myResultLabel);
  myEditor = new ExpressionEditor(this);
  myEditor->setMinimumHeight(20);
  myEditor->setPlaceHolderText( translate( thePlaceHolder ) );
  aMainLay->addWidget(myEditor);
  this->setLayout(aMainLay);

  connect(myEditor, SIGNAL(valueModified()), this, SIGNAL(valuesModified()));
  connect(myEditor, SIGNAL(keyReleased(QObject*, QKeyEvent*)),
          this, SIGNAL(keyReleased(QObject*, QKeyEvent*)));
}

ModuleBase_WidgetExprEditor::~ModuleBase_WidgetExprEditor()
{
}

void ModuleBase_WidgetExprEditor::activateCustom()
{
  ModuleBase_ModelWidget::activateCustom();

  QStringList aParameters;
  ModuleBase_Tools::getParameters(aParameters);
  myEditor->setCompletionList(aParameters);
}

void ModuleBase_WidgetExprEditor::initializeValueByActivate()
{
}

bool ModuleBase_WidgetExprEditor::storeValueCustom()
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  QString aWidgetValue = myEditor->toPlainText();
  aStringAttr->setValue(aWidgetValue.toStdWString());
  updateObject(myFeature);

  // Try to get the value
  QString aStateMsg;
  std::string anErrorMessage = myFeature->string("ExpressionError")->value();
  if (anErrorMessage.empty()) {
    ResultParameterPtr aParam =
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(myFeature->firstResult());
    if(aParam.get()) {
      AttributeDoublePtr aValueAttr =
        aParam->data()->real(ModelAPI_ResultParameter::VALUE());
      if (aValueAttr.get()) {
        double aValue = aValueAttr->value();
        aStateMsg = "Result: " + QString::number(aValue);
      }
    }
  } else {
    aStateMsg = "Error: " + QString::fromStdString(anErrorMessage);
  }
  myResultLabel->setText(aStateMsg);
  return true;
}

bool ModuleBase_WidgetExprEditor::restoreValueCustom()
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myEditor->blockSignals(true);
  QTextCursor aCursor = myEditor->textCursor();
  int pos = aCursor.position();
  QString aRestoredStr;
  if (aStringAttr->isUValue())
    aRestoredStr = QString::fromStdWString(Locale::Convert::toWString(aStringAttr->valueU()));
  else
    aRestoredStr = QString::fromStdString(aStringAttr->value());
  myEditor->setPlainText(aRestoredStr);
  aCursor.setPosition(pos);
  myEditor->setTextCursor(aCursor);
  myEditor->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetExprEditor::getControls() const
{
  QList<QWidget*> result;
  result << myEditor;
  return result;
}

bool ModuleBase_WidgetExprEditor::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    myEditor->selectAll();
  }
  return isModified;
}

void ModuleBase_WidgetExprEditor::onTextChanged()
{
  emit valuesChanged();
}
