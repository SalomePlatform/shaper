// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Tools.cpp
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_Tools.h"

#include <ModuleBase_ParamIntSpinBox.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_IconFactory.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_ResultPart.h>
#include <Events_Loop.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>

#include <TopoDS_Iterator.hxx>

#include <GeomDataAPI_Point2D.h>
#include <Events_Error.h>

#include <Config_PropManager.h>

#include <QWidget>
#include <QLayout>
#include <QPainter>
#include <QBitmap>
#include <QDoubleSpinBox>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QApplication>
#include <QMessageBox>
#include <QAction>

#include <sstream>
#include <string>

const double tolerance = 1e-7;

//#define DEBUG_ACTIVATE_WINDOW
//#define DEBUG_SET_FOCUS

namespace ModuleBase_Tools {

//******************************************************************

//******************************************************************

void adjustMargins(QWidget* theWidget)
{
  if(!theWidget)
    return;
  adjustMargins(theWidget->layout());
}

void adjustMargins(QLayout* theLayout)
{
  if(!theLayout)
    return;
  theLayout->setContentsMargins(2, 5, 2, 5);
  theLayout->setSpacing(4);
}

void zeroMargins(QWidget* theWidget)
{
  if(!theWidget)
    return;
  zeroMargins(theWidget->layout());
}

void zeroMargins(QLayout* theLayout)
{
  if(!theLayout)
    return;
  theLayout->setContentsMargins(0, 0, 0, 0);
  theLayout->setSpacing(5);
}

void activateWindow(QWidget* theWidget, const QString& theInfo)
{
  theWidget->activateWindow();

#ifdef DEBUG_ACTIVATE_WINDOW
  qDebug(QString("activateWindow: %1").arg(theInfo).toStdString().c_str());
#endif
}

void setFocus(QWidget* theWidget, const QString& theInfo)
{
  theWidget->setFocus();

#ifdef DEBUG_SET_FOCUS
  qDebug(QString("setFocus: %1").arg(theInfo).toStdString().c_str());
#endif
}

void setShadowEffect(QWidget* theWidget, const bool isSetEffect)
{
  if (isSetEffect) {
    QGraphicsDropShadowEffect* aGlowEffect = new QGraphicsDropShadowEffect();
    aGlowEffect->setOffset(.0);
    aGlowEffect->setBlurRadius(10.0);
    aGlowEffect->setColor(QColor(0, 170, 255)); // Light-blue color, #00AAFF
    theWidget->setGraphicsEffect(aGlowEffect);
  }
  else {
    QGraphicsEffect* anEffect = theWidget->graphicsEffect();
    if(anEffect)
    anEffect->deleteLater();
    theWidget->setGraphicsEffect(NULL);
  }
}

QPixmap composite(const QString& theAdditionalIcon, const QString& theIcon)
{
  QImage anIcon = ModuleBase_IconFactory::loadImage(theIcon);
  QImage anAditional(theAdditionalIcon);

  if (anIcon.isNull())
    return QPixmap();

  int anAddWidth = anAditional.width();
  int anAddHeight = anAditional.height();

  int aWidth = anIcon.width();
  int aHeight = anIcon.height();

  int aStartWidthPos = aWidth - anAddWidth - 1;
  int aStartHeightPos = aHeight - anAddHeight - 1;

  for (int i = 0; i < anAddWidth && i + aStartWidthPos < aWidth; i++)
  {
    for (int j = 0; j < anAddHeight && j + aStartHeightPos < aHeight; j++)
    {
      if (qAlpha(anAditional.pixel(i, j)) > 0)
        anIcon.setPixel(i + aStartWidthPos, j + aStartHeightPos, anAditional.pixel(i, j));
    }
  }
  return QPixmap::fromImage(anIcon);
}

QPixmap lighter(const QString& theIcon, const int theLighterValue)
{
  QImage anIcon = ModuleBase_IconFactory::loadImage(theIcon);
  if (anIcon.isNull())
    return QPixmap();

  QImage aResult = ModuleBase_IconFactory::loadImage(theIcon);
  for (int i = 0; i < anIcon.width(); i++)
  {
    for (int j = 0; j < anIcon.height(); j++)
    {
      QRgb anRgb = anIcon.pixel(i, j);
      QColor aPixelColor(qRed(anRgb), qGreen(anRgb), qBlue(anRgb),
                         qAlpha(aResult.pixel(i, j)));

      QColor aLighterColor = aPixelColor.lighter(theLighterValue);
      aResult.setPixel(i, j, qRgba(aLighterColor.red(), aLighterColor.green(),
                                    aLighterColor.blue(), aLighterColor.alpha()));
    }
  }
  return QPixmap::fromImage(aResult);
}

void setSpinText(ModuleBase_ParamSpinBox* theSpin, const QString& theText)
{
  if (theSpin->text() == theText) 
    return;
  // In order to avoid extra text setting because it will
  // reset cursor position in control
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setText(theText);
  theSpin->blockSignals(isBlocked);
}

void setSpinValue(QDoubleSpinBox* theSpin, double theValue)
{
  if (fabs(theSpin->value() - theValue) < tolerance)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

void setSpinValue(ModuleBase_ParamSpinBox* theSpin, double theValue)
{
  if (fabs(theSpin->value() - theValue) < tolerance)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

void setSpinText(ModuleBase_ParamIntSpinBox* theSpin, const QString& theText)
{
  // In order to avoid extra text setting because it will
  // reset cursor position in control
  if (theSpin->text() == theText)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setText(theText);
  theSpin->blockSignals(isBlocked);
}

void setSpinValue(ModuleBase_ParamIntSpinBox* theSpin, int theValue)
{
  if (theSpin->value() == theValue)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

QAction* createAction(const QIcon& theIcon, const QString& theText,
                      QObject* theParent, const QObject* theReceiver,
                      const char* theMember, const QString& theToolTip,
                      const QString& theStatusTip)
{
  QAction* anAction = new QAction(theIcon, theText, theParent);
  anAction->setToolTip(theToolTip.isEmpty() ? theText : theToolTip);
  anAction->setStatusTip(!theStatusTip.isEmpty() ? theStatusTip :
                                                   (!theToolTip.isEmpty() ? theToolTip : theText));
  if (theReceiver)
    QObject::connect(anAction, SIGNAL(triggered(bool)), theReceiver, theMember);

  return anAction;
}

QString objectInfo(const ObjectPtr& theObj, const bool isUseAttributesInfo)
{
  QString aFeatureStr = "feature";
  if (!theObj.get())
    return aFeatureStr;

  std::ostringstream aPtrStr;
  aPtrStr << "[" << theObj.get() << "]";

  ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObj);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if(aRes.get()) {
    aFeatureStr.append(QString("(result%1)").arg(aPtrStr.str().c_str()).toStdString() .c_str());
    if (aRes->isDisabled())
      aFeatureStr.append("[disabled]");
    if (aRes->isConcealed())
      aFeatureStr.append("[concealed]");
    if (ModelAPI_Tools::hasSubResults(aRes))
      aFeatureStr.append("[hasSubResults]");

    aFeature = ModelAPI_Feature::feature(aRes);
  }
  else
    aFeatureStr.append(aPtrStr.str().c_str());

  if (aFeature.get()) {
    aFeatureStr.append(QString(": %1").arg(aFeature->getKind().c_str()).toStdString().c_str());
    if (aFeature->data()->isValid()) {
      aFeatureStr.append(QString(", name=%1").arg(aFeature->data()->name().c_str()).toStdString()
                                                                                       .c_str());
    }
    if (isUseAttributesInfo) {
      std::list<AttributePtr> anAttrs = aFeature->data()->attributes("");
      std::list<AttributePtr>::const_iterator anIt = anAttrs.begin(), aLast = anAttrs.end();
      QStringList aValues;
      for(; anIt != aLast; anIt++) {
        AttributePtr anAttr = *anIt;
        QString aValue = "not defined";
        std::string aType = anAttr->attributeType();
        if (aType == GeomDataAPI_Point2D::typeId()) {
          std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                                                         anAttr);
          if (aPoint.get())
            aValue = QString("(%1, %2)").arg(aPoint->x()).arg(aPoint->y());
        }
        else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
        }

        aValues.push_back(QString("%1: %2").arg(anAttr->id().c_str()).arg(aValue).toStdString().c_str());
      }
      if (!aValues.empty())
        aFeatureStr.append(QString(", attributes: %1").arg(aValues.join(", ").toStdString().c_str()));
    }
  }

  return aFeatureStr;
}

typedef QMap<QString, TopAbs_ShapeEnum> ShapeTypes;
static ShapeTypes myShapeTypes;

TopAbs_ShapeEnum shapeType(const QString& theType)
{
  if (myShapeTypes.count() == 0) {
    myShapeTypes["compound"]   = TopAbs_COMPOUND;
    myShapeTypes["compounds"]  = TopAbs_COMPOUND;
    myShapeTypes["compsolid"]  = TopAbs_COMPSOLID;
    myShapeTypes["compsolids"] = TopAbs_COMPSOLID;
    myShapeTypes["solid"]      = TopAbs_SOLID;
    myShapeTypes["solids"]     = TopAbs_SOLID;
    myShapeTypes["shell"]      = TopAbs_SHELL;
    myShapeTypes["shells"]     = TopAbs_SHELL;
    myShapeTypes["face"]       = TopAbs_FACE;
    myShapeTypes["faces"]      = TopAbs_FACE;
    myShapeTypes["wire"]       = TopAbs_WIRE;
    myShapeTypes["wires"]      = TopAbs_WIRE;
    myShapeTypes["edge"]       = TopAbs_EDGE;
    myShapeTypes["edges"]      = TopAbs_EDGE;
    myShapeTypes["vertex"]     = TopAbs_VERTEX;
    myShapeTypes["vertices"]   = TopAbs_VERTEX;
    myShapeTypes["objects"]    = TopAbs_SHAPE;
  }
  QString aType = theType.toLower();
  if(myShapeTypes.contains(aType))
    return myShapeTypes[aType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}

void checkObjects(const QObjectPtrList& theObjects, bool& hasResult, bool& hasFeature,
                  bool& hasParameter, bool& hasCompositeOwner)
{
  hasResult = false;
  hasFeature = false;
  hasParameter = false;
  hasCompositeOwner = false;
  foreach(ObjectPtr aObj, theObjects) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    ResultParameterPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aResult);

    hasResult |= (aResult.get() != NULL);
    hasFeature |= (aFeature.get() != NULL);
    hasParameter |= (aConstruction.get() != NULL);
    if (hasFeature) 
      hasCompositeOwner |= (ModelAPI_Tools::compositeOwner(aFeature) != NULL);
    if (hasFeature && hasResult  && hasParameter && hasCompositeOwner)
      break;
  }
}

void setDefaultDeviationCoefficient(const TopoDS_Shape& theShape,
                                    const Handle(Prs3d_Drawer)& theDrawer)
{
  if (theShape.IsNull())
    return;
  TopAbs_ShapeEnum aType = theShape.ShapeType();
  if ((aType == TopAbs_EDGE) || (aType == TopAbs_WIRE)) 
    theDrawer->SetDeviationCoefficient(1.e-4);
}

Quantity_Color color(const std::string& theSection,
                     const std::string& theName,
                     const std::string& theDefault)
{
  std::vector<int> aColor = Config_PropManager::color(theSection, theName, theDefault);
  return Quantity_Color(aColor[0] / 255., aColor[1] / 255., aColor[2] / 255., Quantity_TOC_RGB);
}

ObjectPtr getObject(const AttributePtr& theAttribute)
{
  ObjectPtr anObject;
  std::string anAttrType = theAttribute->attributeType();
  if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr != NULL && anAttr->isObject())
      anObject = anAttr->object();
  }
  if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    if (anAttr != NULL)
      anObject = anAttr->context();
  }
  if (anAttrType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    if (anAttr.get() != NULL)
      anObject = anAttr->value();
  }
  return anObject;
}

TopAbs_ShapeEnum getCompoundSubType(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();

  // for compounds check sub-shapes: it may be compound of needed type:
  // Booleans may produce compounds of Solids
  if (aShapeType == TopAbs_COMPOUND) {
    for(TopoDS_Iterator aSubs(theShape); aSubs.More(); aSubs.Next()) {
      if (!aSubs.Value().IsNull()) {
        TopAbs_ShapeEnum aSubType = aSubs.Value().ShapeType();
        if (aSubType == TopAbs_COMPOUND) { // compound of compound(s)
          aShapeType = TopAbs_COMPOUND;
          break;
        }
        if (aShapeType == TopAbs_COMPOUND) {
          aShapeType = aSubType;
        } else if (aShapeType != aSubType) { // compound of shapes of different types
          aShapeType = TopAbs_COMPOUND;
          break;
        }
      }
    }
  }
  return aShapeType;
}

void getParameters(QStringList& theParameters)
{
  theParameters.clear();

  SessionPtr aSession = ModelAPI_Session::get();
  std::list<DocumentPtr> aDocList;
  DocumentPtr anActiveDocument = aSession->activeDocument();
  DocumentPtr aRootDocument = aSession->moduleDocument();
  aDocList.push_back(anActiveDocument);
  if (anActiveDocument != aRootDocument) {
    aDocList.push_back(aRootDocument);
  }
  std::string aGroupId = ModelAPI_ResultParameter::group();
  for(std::list<DocumentPtr>::const_iterator it = aDocList.begin(); it != aDocList.end(); ++it) {
    DocumentPtr aDocument = *it;
    int aSize = aDocument->size(aGroupId);
    for (int i = 0; i < aSize; i++) {
      ObjectPtr anObject = aDocument->object(aGroupId, i);
      std::string aParameterName = anObject->data()->name();
      theParameters.append(aParameterName.c_str());
    }
  }
}

std::string findGreedAttribute(ModuleBase_IWorkshop* theWorkshop, const FeaturePtr& theFeature)
{
  std::string anAttributeId;

  std::string aXmlCfg, aDescription;
  theWorkshop->module()->getXMLRepresentation(theFeature->getKind(), aXmlCfg, aDescription);

  ModuleBase_WidgetFactory aFactory(aXmlCfg, theWorkshop);
  std::string anAttributeTitle;
  aFactory.getGreedAttribute(anAttributeId);

  return anAttributeId;
}

bool hasObject(const AttributePtr& theAttribute, const ObjectPtr& theObject,
               const std::shared_ptr<GeomAPI_Shape>& theShape,
               ModuleBase_IWorkshop* theWorkshop,
               const bool theTemporarily)
{
  bool aHasObject = false;
  if (!theAttribute.get())
    return aHasObject;

  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr aRef = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr aObject = aRef->value();
    aHasObject = aObject && aObject->isSame(theObject);
    //if (!(aObject && aObject->isSame(theObject))) {
    //  aRef->setValue(theObject);
    //}
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);
    if (anAttribute.get()) {
      //aRefAttr->setAttr(anAttribute);
    }
    else {
      ObjectPtr aObject = aRefAttr->object();
      aHasObject = aObject && aObject->isSame(theObject);
      //if (!(aObject && aObject->isSame(theObject))) {
      //  aRefAttr->setObject(theObject);
      //}
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    /*AttributeSelectionPtr aSelectAttr =
                             std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aSelectAttr.get() != NULL) {
      aSelectAttr->setValue(aResult, theShape, theTemporarily);
    }*/
  }
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    aHasObject = aSelectionListAttr->isInList(aResult, theShape, theTemporarily);
    //if (!theCheckIfAttributeHasObject || !aSelectionListAttr->isInList(aResult, theShape, theTemporarily))
    //  aSelectionListAttr->append(aResult, theShape, theTemporarily);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    aHasObject = aRefListAttr->isInList(theObject);
    //if (!theCheckIfAttributeHasObject || !aRefListAttr->isInList(theObject))
    //  aRefListAttr->append(theObject);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);

    if (anAttribute.get()) {
      aHasObject = aRefAttrListAttr->isInList(anAttribute);
      //if (!theCheckIfAttributeHasObject || !aRefAttrListAttr->isInList(anAttribute))
      //  aRefAttrListAttr->append(anAttribute);
    }
    else {
      aHasObject = aRefAttrListAttr->isInList(theObject);
      //if (!theCheckIfAttributeHasObject || !aRefAttrListAttr->isInList(theObject))
      //  aRefAttrListAttr->append(theObject);
    }
  }
  return aHasObject;
}

void setObject(const AttributePtr& theAttribute, const ObjectPtr& theObject,
               const GeomShapePtr& theShape, ModuleBase_IWorkshop* theWorkshop,
               const bool theTemporarily, const bool theCheckIfAttributeHasObject)
{
  if (!theAttribute.get())
    return;

  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr aRef = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr aObject = aRef->value();
    if (!(aObject && aObject->isSame(theObject))) {
      aRef->setValue(theObject);
    }
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);
    if (anAttribute.get())
      aRefAttr->setAttr(anAttribute);
    else {
      ObjectPtr aObject = aRefAttr->object();
      if (!(aObject && aObject->isSame(theObject))) {
        aRefAttr->setObject(theObject);
      }
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr =
                             std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aSelectAttr.get() != NULL) {
      aSelectAttr->setValue(aResult, theShape, theTemporarily);
    }
  }
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (!theCheckIfAttributeHasObject || !aSelectionListAttr->isInList(aResult, theShape, theTemporarily))
      aSelectionListAttr->append(aResult, theShape, theTemporarily);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    if (!theCheckIfAttributeHasObject || !aRefListAttr->isInList(theObject))
      aRefListAttr->append(theObject);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);

    if (anAttribute.get()) {
      if (!theCheckIfAttributeHasObject || !aRefAttrListAttr->isInList(anAttribute))
        aRefAttrListAttr->append(anAttribute);
    }
    else {
      if (!theCheckIfAttributeHasObject || !aRefAttrListAttr->isInList(theObject))
        aRefAttrListAttr->append(theObject);
    }
  }
}

GeomShapePtr getShape(const AttributePtr& theAttribute, ModuleBase_IWorkshop* theWorkshop)
{
  GeomShapePtr aShape;
  if (!theAttribute.get())
    return aShape;

  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr.get() && !aRefAttr->isObject()) {
      AttributePtr anAttribute = aRefAttr->attr();
      aShape = theWorkshop->module()->findShape(anAttribute);
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                                                                 (theAttribute);
    aShape = aSelectAttr->value();
  }
  return aShape;
}

void flushUpdated(ObjectPtr theObject)
{
  blockUpdateViewer(true);

  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  blockUpdateViewer(false);
}

void blockUpdateViewer(const bool theValue)
{
  // the viewer update should be blocked in order to avoid the temporary feature content
  // when the solver processes the feature, the redisplay message can be flushed
  // what caused the display in the viewer preliminary states of object
  // e.g. fillet feature, angle value change
  std::shared_ptr<Events_Message> aMsg;
  if (theValue) {
    aMsg = std::shared_ptr<Events_Message>(
        new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  }
  else {
    // the viewer update should be unblocked
    aMsg = std::shared_ptr<Events_Message>(
        new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));
  }
  Events_Loop::loop()->send(aMsg);
}

QString wrapTextByWords(const QString& theValue, QWidget* theWidget,
                                          int theMaxLineInPixels)
{
  static QFontMetrics tfm(theWidget ? theWidget->font() : QApplication::font());
  static qreal phi = 2.618;

  QRect aBounds = tfm.boundingRect(theValue);
  if(aBounds.width() <= theMaxLineInPixels)
    return theValue;

  qreal s = aBounds.width() * aBounds.height();
  qreal aGoldWidth = sqrt(s*phi);

  QStringList aWords = theValue.split(" ", QString::SkipEmptyParts);
  QStringList aLines;
  int n = aWords.count();
  QString aLine;
  for (int i = 0; i < n; i++) {
    QString aLineExt = aLine + " " + aWords[i];
    qreal anWidthNonExt = tfm.boundingRect(aLine).width();
    qreal anWidthExt = tfm.boundingRect(aLineExt).width();
    qreal aDeltaNonExt = fabs(anWidthNonExt-aGoldWidth);
    qreal aDeltaExt    = fabs(anWidthExt-aGoldWidth);
    if(aDeltaNonExt < aDeltaExt) {
      // new line
      aLines.append(aLine);
      aLine = aWords[i];
    }
    else
      aLine = aLineExt;
  }

  if(!aLine.isEmpty())
    aLines.append(aLine);

  QString aResult = aLines.join("\n");
  return aResult;
}

//**************************************************************
void refsToFeatureInFeatureDocument(const ObjectPtr& theObject, std::set<FeaturePtr>& theRefFeatures)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature.get()) {
    DocumentPtr aFeatureDoc = aFeature->document();
    // 1. find references in the current document
    aFeatureDoc->refsToFeature(aFeature, theRefFeatures, false);
  }
}


//**************************************************************
bool isSubOfComposite(const ObjectPtr& theObject)
{
  bool isSub = false;
  std::set<FeaturePtr> aRefFeatures;
  refsToFeatureInFeatureDocument(theObject, aRefFeatures);
  std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                       aLast = aRefFeatures.end();
  for (; anIt != aLast && !isSub; anIt++) {
    isSub = isSubOfComposite(theObject, *anIt);
  }
  return isSub;
}

//**************************************************************
bool isSubOfComposite(const ObjectPtr& theObject, const FeaturePtr& theFeature)
{
  bool isSub = false;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aComposite.get()) {
    isSub = aComposite->isSub(theObject);
    // the recursive is possible, the parameters are sketch circle and extrusion cut. They are
    // separated by composite sketch feature
    if (!isSub) {
      int aNbSubs = aComposite->numberOfSubs();
      for (int aSub = 0; aSub < aNbSubs && !isSub; aSub++) {
        isSub = isSubOfComposite(theObject, aComposite->subFeature(aSub));
      }
    }
  }
  return isSub;
}

//**************************************************************
bool isFeatureOfResult(const FeaturePtr& theFeature, const std::string& theGroupOfResult)
{
  bool isResult = false;

  if (!theFeature->data()->isValid())
    return isResult;

  ResultPtr aFirstResult = theFeature->firstResult();
  if (!aFirstResult.get())
    return isResult;

  return aFirstResult->groupName() == theGroupOfResult;
}

//**************************************************************
bool askToDelete(const std::set<FeaturePtr> theFeatures,
                 const std::map<FeaturePtr, std::set<FeaturePtr> >& theReferences,
                 QWidget* theParent,
                 std::set<FeaturePtr>& theReferencesToDelete)
{
  std::set<FeaturePtr> aFeaturesRefsTo;
  std::set<FeaturePtr> aFeaturesRefsToParameter;
  std::set<FeaturePtr> aParameterFeatures;
  std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin(),
                                       aLast = theFeatures.end();
  // separate features to references to parameter features and references to others
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (theReferences.find(aFeature) == theReferences.end())
      continue;

    std::set<FeaturePtr> aRefFeatures;
    std::set<FeaturePtr> aRefList = theReferences.at(aFeature);
    std::set<FeaturePtr>::const_iterator aRefIt = aRefList.begin(), aRefLast = aRefList.end();
    for (; aRefIt != aRefLast; aRefIt++) {
      FeaturePtr aRefFeature = *aRefIt;
      if (theFeatures.find(aRefFeature) == theFeatures.end() && // it is not selected
          aRefFeatures.find(aRefFeature) == aRefFeatures.end()) // it is not added
        aRefFeatures.insert(aRefFeature);
    }

    if (isFeatureOfResult(aFeature, ModelAPI_ResultParameter::group())) {
      aFeaturesRefsToParameter.insert(aRefFeatures.begin(), aRefFeatures.end());
      aParameterFeatures.insert(aFeature);
    }
    else {
      theReferencesToDelete.insert(aRefFeatures.begin(), aRefFeatures.end());
    }
  }

  std::set<FeaturePtr> aFeaturesRefsToParameterOnly;
  anIt = aFeaturesRefsToParameter.begin();
  aLast = aFeaturesRefsToParameter.end();
  // separate features to references to parameter features and references to others
  QStringList aParamFeatureNames;
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (theReferencesToDelete.find(aFeature) == theReferencesToDelete.end()) {
      aFeaturesRefsToParameterOnly.insert(aFeature);
      aParamFeatureNames.append(aFeature->name().c_str());
    }
  }
  aParamFeatureNames.sort();
  QStringList aPartFeatureNames, anOtherFeatureNames;
  anIt = theReferencesToDelete.begin();
  aLast = theReferencesToDelete.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (isFeatureOfResult(aFeature, ModelAPI_ResultPart::group()))
      aPartFeatureNames.append(aFeature->name().c_str());
    else
      anOtherFeatureNames.append(aFeature->name().c_str());
  }
  aPartFeatureNames.sort();
  anOtherFeatureNames.sort();

  bool aCanReplaceParameters = !aFeaturesRefsToParameterOnly.empty();

  QMessageBox aMessageBox(theParent);
  aMessageBox.setWindowTitle(QObject::tr("Delete features"));
  aMessageBox.setIcon(QMessageBox::Warning);
  aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  aMessageBox.setDefaultButton(QMessageBox::No);

  QString aText;
  QString aSep = ", ";
  if (!aPartFeatureNames.empty())
    aText += QString(QObject::tr("The following parts will be deleted: %1.\n")).arg(aPartFeatureNames.join(aSep));
  if (!anOtherFeatureNames.empty())
    aText += QString(QObject::tr("Selected features are used in the following features: %1.\nThese features will be deleted.\n"))
                     .arg(anOtherFeatureNames.join(aSep));
  if (!aParamFeatureNames.empty()) {
    aText += QString(QObject::tr("Selected parameters are used in the following features: %1.\nThese features will be deleted.\nOr parameters could be replaced by their values.\n"))
                     .arg(aParamFeatureNames.join(aSep));
    QPushButton *aReplaceButton = aMessageBox.addButton(QObject::tr("Replace"), QMessageBox::ActionRole);
  }

  if (!aText.isEmpty()) {
    aText += "Would you like to continue?";
    aMessageBox.setText(aText);
    aMessageBox.exec();
    QMessageBox::ButtonRole aButtonRole = aMessageBox.buttonRole(aMessageBox.clickedButton());

    if (aButtonRole == QMessageBox::NoRole)
      return false;

    if (aButtonRole == QMessageBox::ActionRole) {
      foreach (FeaturePtr aObj, aParameterFeatures)
        ModelAPI_ReplaceParameterMessage::send(aObj, 0);
    }
    else
      theReferencesToDelete.insert(aFeaturesRefsToParameterOnly.begin(), aFeaturesRefsToParameterOnly.end());
  }
  return true;
}

//**************************************************************
void convertToFeatures(const QObjectPtrList& theObjects, std::set<FeaturePtr>& theFeatures)
{
  QObjectPtrList::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for(; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
    // for parameter result, use the corresponded reature to be removed
    if (!aFeature.get() && anObject->groupName() == ModelAPI_ResultParameter::group()) {
      aFeature = ModelAPI_Feature::feature(anObject);
    }
    theFeatures.insert(aFeature);
  }
}

} // namespace ModuleBase_Tools


