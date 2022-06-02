// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "ModuleBase_Tools.h"

#include <ModuleBase_ParamIntSpinBox.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Preferences.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Expression.h>
#include <ModelAPI_ResultField.h>
#include <Events_Loop.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Folder.h>

#include <ModelGeomAlgo_Point2D.h>
#include <SUIT_ResourceMgr.h>

#ifdef HAVE_SALOME
#include <SUIT_Application.h>
#include <SUIT_Session.h>
#endif

#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Iterator.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>

#include <GeomDataAPI_Point2D.h>
#include <Events_InfoMessage.h>
#include <GeomAPI_ShapeExplorer.h>

#include <Config_PropManager.h>
#include <Config_Translator.h>

#include <Prs3d_PointAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

#include <Image_AlienPixMap.hxx>

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
#include <QTextCodec>
#include <QWindow>
#include <QScreen>
#include <QCheckBox>

#include <sstream>
#include <string>

#ifdef WIN32
#pragma warning(disable : 4996) // for getenv
#endif

const double tolerance = 1e-7;
const double DEFAULT_DEVIATION_COEFFICIENT = 1.e-4;

//#define DEBUG_ACTIVATE_WINDOW
//#define DEBUG_SET_FOCUS

#ifdef WIN32
# define FSEP "\\"
#else
# define FSEP "/"
#endif

namespace ModuleBase_Tools {

//******************************************************************

  //! Waits for REDISPLAY message and set the Visible flag to the entities
  //! according to Preferences choice.
  class ModuleBase_RedisplayListener : public Events_Listener
  {
  public:
    static std::shared_ptr<ModuleBase_RedisplayListener> instance()
    {
      static std::shared_ptr<ModuleBase_RedisplayListener>
          anInstance(new ModuleBase_RedisplayListener);
      return anInstance;
    }

    void processEvent(const std::shared_ptr<Events_Message>& theMessage)
    {
      if (theMessage->eventID() == Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY))
      {
#if HAVE_SALOME
        // If the python script is being loaded now, the preferences should be used
        // to display the required object
        SUIT_Session* aSession = SUIT_Session::session();
        if (!aSession)
          return;
        SUIT_Application * anApp = aSession->activeApplication();
        if (!anApp)
          return;
        QVariant aVar = anApp->property("IsLoadedScript");
        if (!aVar.isNull() && aVar.toBool()) {
          DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
          int aSize = aRootDoc->size(ModelAPI_ResultPart::group());
          if (aSize > 0) {
            ObjectPtr anPartObject = aRootDoc->object(ModelAPI_ResultPart::group(), aSize - 1);
            ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(anPartObject);
            ModuleBase_Tools::setDisplaying(aPart, true);
          }
        }
#endif
      }
    }

  private:
    ModuleBase_RedisplayListener()
    {
      Events_Loop::loop()->registerListener(this,
          Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
    }
  };

  static std::shared_ptr<ModuleBase_RedisplayListener>
      RL = ModuleBase_RedisplayListener::instance();

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
  if (theWidget) {
    theWidget->activateWindow();
    theWidget->raise();
  }

#ifdef DEBUG_ACTIVATE_WINDOW
  qDebug(QString("activateWindow: %1").arg(theInfo).toStdString().c_str());
#endif
}

void setFocus(QWidget* theWidget, const QString& theInfo)
{
  activateWindow(theWidget);
  theWidget->setFocus();
  // rectangle of focus is not visible on tool button widgets
  theWidget->update();
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
  return composite(anAditional, anIcon);
}

QPixmap composite(const QImage& theAdditionalIcon, QImage& theIcon)
{
  if (theIcon.isNull())
    return QPixmap();

  int anAddWidth = theAdditionalIcon.width();
  int anAddHeight = theAdditionalIcon.height();

  int aWidth = theIcon.width();
  int aHeight = theIcon.height();

  int aStartWidthPos = aWidth - anAddWidth;
  int aStartHeightPos = aHeight - anAddHeight;

  for (int i = 0; i < anAddWidth && i + aStartWidthPos < aWidth; i++)
  {
    for (int j = 0; j < anAddHeight && j + aStartHeightPos < aHeight; j++)
    {
      if (qAlpha(theAdditionalIcon.pixel(i, j)) > 0)
        theIcon.setPixel(i + aStartWidthPos, j + aStartHeightPos, theAdditionalIcon.pixel(i, j));
    }
  }
  return QPixmap::fromImage(theIcon);
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
  if (!theSpin->text().isEmpty() && fabs(theSpin->value() - theValue) < tolerance)
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

#ifdef _DEBUG
QString objectName(const ObjectPtr& theObj)
{
  if (!theObj.get())
    return "";

  return QString::fromStdWString(theObj->data()->name());
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
      aFeatureStr.append(QString(", name=%1")
        .arg(QString::fromStdWString(theObj->data()->name())).toStdString().c_str());
    }
    if (isUseAttributesInfo) {
      std::set<std::shared_ptr<ModelAPI_Attribute> > anAttributes;
      std::string aPointsInfo = ModelGeomAlgo_Point2D::getPontAttributesInfo(aFeature,
                                                                          anAttributes).c_str();
      if (!aPointsInfo.empty())
        aFeatureStr.append(QString(", attributes: %1")
          .arg(aPointsInfo.c_str()).toStdString().c_str());
    }
  }

  return aFeatureStr;
}
#endif

typedef QMap<QString, int> ShapeTypes;
static ShapeTypes myShapeTypes;

int shapeType(const QString& theType)
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
    myShapeTypes["object"]     = ModuleBase_ResultPrs::Sel_Result;
    myShapeTypes["objects"]    = ModuleBase_ResultPrs::Sel_Result;
  }
  QString aType = theType.toLower();
  if(myShapeTypes.contains(aType))
    return myShapeTypes[aType];
  Events_InfoMessage("ModuleBase_Tools", "Shape type defined in XML is not implemented!").send();
  return TopAbs_SHAPE;
}

void checkObjects(const QObjectPtrList& theObjects, bool& hasResult, bool& hasFeature,
                  bool& hasParameter, bool& hasCompositeOwner, bool& hasResultInHistory,
                  bool& hasFolder)
{
  hasResult = false;
  hasFeature = false;
  hasParameter = false;
  hasCompositeOwner = false;
  hasResultInHistory = false;
  hasFolder = false;
  foreach(ObjectPtr aObj, theObjects) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(aObj);
    ResultParameterPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aResult);
    FieldStepPtr aStep = std::dynamic_pointer_cast<ModelAPI_ResultField::ModelAPI_FieldStep>(aObj);

    hasResult |= ((aResult.get() != NULL) || (aStep.get() != NULL));
    hasFeature |= (aFeature.get() != NULL);
    hasFolder |= (aFolder.get() != NULL);
    hasParameter |= (aConstruction.get() != NULL);
    if (hasFeature)
      hasCompositeOwner |= (ModelAPI_Tools::compositeOwner(aFeature) != NULL);
    else if (aResult.get())
      hasCompositeOwner |= (ModelAPI_Tools::bodyOwner(aResult) != NULL);

    if (!hasResultInHistory && aResult.get()) {
      aFeature = ModelAPI_Feature::feature(aResult);
      hasResultInHistory = aFeature.get() && aFeature->isInHistory();
    }

    if (hasFeature && hasResult  && hasParameter && hasCompositeOwner)
      break;
  }
}

/*bool setDefaultDeviationCoefficient(std::shared_ptr<GeomAPI_Shape> theGeomShape)
{
  if (!theGeomShape.get())
    return false;
  // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
  // correction of deviation for them should not influence to the application performance
  GeomAPI_ShapeExplorer anExp(theGeomShape, GeomAPI_Shape::FACE);
  bool anEmpty = anExp.empty();
  return !anExp.more();
}*/

/*void setDefaultDeviationCoefficient(const std::shared_ptr<ModelAPI_Result>& theResult,
                                    const Handle(Prs3d_Drawer)& theDrawer)
{
  if (!theResult.get())
    return;
  bool aUseDeviation = false;

  std::string aResultGroup = theResult->groupName();
  if (aResultGroup == ModelAPI_ResultConstruction::group())
    aUseDeviation = true;
  else if (aResultGroup == ModelAPI_ResultBody::group()) {
    GeomShapePtr aGeomShape = theResult->shape();
    if (aGeomShape.get())
      aUseDeviation = setDefaultDeviationCoefficient(aGeomShape);
  }
  if (aUseDeviation)
    theDrawer->SetDeviationCoefficient(DEFAULT_DEVIATION_COEFFICIENT);
}
*/
void setDefaultDeviationCoefficient(const TopoDS_Shape& theShape,
                                    const Handle(Prs3d_Drawer)& theDrawer)
{
  if (theShape.IsNull())
    return;
  if (theDrawer.IsNull())
    return;

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape());
  aGeomShape->setImpl(new TopoDS_Shape(theShape));

  // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
  // correction of deviation for them should not influence to the application performance
  GeomAPI_ShapeExplorer anExp(aGeomShape, GeomAPI_Shape::FACE);
  bool isConstruction = !anExp.more();

  double aDeflection;
  if (isConstruction)
    aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
  else
    aDeflection = Config_PropManager::real("Visualization", "body_deflection");

  theDrawer->SetDeviationCoefficient(aDeflection);
}

Quantity_Color color(const std::string& theSection,
                     const std::string& theName)
{
  std::vector<int> aColor = Config_PropManager::color(theSection, theName);
  return Quantity_Color(aColor[0] / 255., aColor[1] / 255., aColor[2] / 255., Quantity_TOC_RGB);
}

ObjectPtr getObject(const AttributePtr& theAttribute)
{
  ObjectPtr anObject;
  std::string anAttrType = theAttribute->attributeType();
  if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr != NULL && anAttr->isObject())
      anObject = anAttr->object();
  }
  if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    if (anAttr != NULL)
      anObject = anAttr->context();
  }
  if (anAttrType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
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

TopoDS_Shape getSelectedShape(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs)
{
  if (thePrs->shape().get())
    return thePrs->shape()->impl<TopoDS_Shape>();

  Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(thePrs->owner());
  if (!anOwner.IsNull())
    return anOwner->Shape();

  return TopoDS_Shape();
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
      std::wstring aParameterName = anObject->data()->name();
      theParameters.append(QString::fromStdWString(aParameterName));
    }
  }
}

std::string findGreedAttribute(ModuleBase_IWorkshop* theWorkshop,
                               const FeaturePtr& theFeature)
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
    AttributeReferencePtr aRef =
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr aObject = aRef->value();
    aHasObject = aObject && aObject->isSame(theObject);
    //if (!(aObject && aObject->isSame(theObject))) {
    //  aRef->setValue(theObject);
    //}
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

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
    aHasObject = aSelectionListAttr->isInList(theObject, theShape, theTemporarily);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    aHasObject = aRefListAttr->isInList(theObject);
    //if (!theCheckIfAttributeHasObject || !aRefListAttr->isInList(theObject))
    //  aRefListAttr->append(theObject);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
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

bool setObject(const AttributePtr& theAttribute, const ObjectPtr& theObject,
               const GeomShapePtr& theShape, ModuleBase_IWorkshop* theWorkshop,
               const bool theTemporarily, const bool theCheckIfAttributeHasObject)
{
  if (!theAttribute.get())
    return false;

  bool isDone = true;
  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr aRef =
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr aObject = aRef->value();
    if (!(aObject && aObject->isSame(theObject))) {
      aRef->setValue(theObject);
    }
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

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
    if (aSelectAttr.get() != NULL) {
      aSelectAttr->setValue(theObject, theShape, theTemporarily);
    }
  }
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    if (!theCheckIfAttributeHasObject ||
      !aSelectionListAttr->isInList(theObject, theShape, theTemporarily))
      aSelectionListAttr->append(theObject, theShape, theTemporarily);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    if (!theCheckIfAttributeHasObject || !aRefListAttr->isInList(theObject)) {
      if (theObject.get())
        aRefListAttr->append(theObject);
      else
        isDone = false;
    }
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);

    if (anAttribute.get()) {
      if (!theCheckIfAttributeHasObject || !aRefAttrListAttr->isInList(anAttribute))
        aRefAttrListAttr->append(anAttribute);
    }
    else {
      if (!theCheckIfAttributeHasObject || !aRefAttrListAttr->isInList(theObject)) {
        if (theObject.get())
          aRefAttrListAttr->append(theObject);
        else
          isDone = false;
      }
    }
  }
  return isDone;
}

GeomShapePtr getShape(const AttributePtr& theAttribute, ModuleBase_IWorkshop* theWorkshop)
{
  GeomShapePtr aShape;
  if (!theAttribute.get())
    return aShape;

  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr.get() && !aRefAttr->isObject()) {
      AttributePtr anAttribute = aRefAttr->attr();
      aShape = theWorkshop->module()->findShape(anAttribute);
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                                                                 (theAttribute);
    aShape = aSelectAttr->value();
  }
  else // Geom2D point processing
    aShape = theWorkshop->module()->findShape(theAttribute);
  return aShape;
}

void flushUpdated(ObjectPtr theObject)
{
  blockUpdateViewer(true);

  // Fix the problem of not previewed results of constraints applied. Flush Create/Delete
  // (for the sketch result) to start processing of the sketch in the solver.
  // TODO: these flushes should be moved in a separate method provided by Model
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_VISUAL_ATTRIBUTES));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_UPDATE_SELECTION));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

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
    QString aLineExt =  i == 0 ? aWords[i] : aLine + " " + aWords[i];
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
QLocale doubleLocale()
{
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid inconsistency of double-2-string and string-2-double conversion)
  QLocale aLocale;
  aLocale.setNumberOptions(aLocale.numberOptions() |
                           QLocale::OmitGroupSeparator |
                           QLocale::RejectGroupSeparator);
  return aLocale;
}

//**************************************************************
void refsToFeatureInFeatureDocument(const ObjectPtr& theObject,
                                    std::set<FeaturePtr>& theRefFeatures)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature.get()) {
    DocumentPtr aFeatureDoc = aFeature->document();
    // 1. find references in the current document
    aFeatureDoc->refsToFeature(aFeature, theRefFeatures, false);
  }
}


//**************************************************************
/*bool isSubOfComposite(const ObjectPtr& theObject)
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
}*/

//**************************************************************
/*bool isSubOfComposite(const ObjectPtr& theObject, const FeaturePtr& theFeature)
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
}*/

//**************************************************************
ResultPtr firstResult(const ObjectPtr& theObject)
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (!aResult.get()) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    if (aFeature.get())
      aResult = aFeature->firstResult();
  }
  return aResult;
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
bool hasModuleDocumentFeature(const std::set<FeaturePtr>& theFeatures)
{
  bool aFoundModuleDocumentObject = false;
  DocumentPtr aModuleDoc = ModelAPI_Session::get()->moduleDocument();

  std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin(), aLast = theFeatures.end();
  for (; anIt != aLast && !aFoundModuleDocumentObject; anIt++) {
    FeaturePtr aFeature = *anIt;
    ResultPtr aResult = ModuleBase_Tools::firstResult(aFeature);
    if (aResult.get() && aResult->groupName() == ModelAPI_ResultPart::group())
      continue;
    aFoundModuleDocumentObject = aFeature->document() == aModuleDoc;
  }

  return aFoundModuleDocumentObject;
}

//**************************************************************
bool askToDelete(const std::set<FeaturePtr> theFeatures,
                 const std::map<FeaturePtr, std::set<FeaturePtr> >& theReferences,
                 QWidget* theParent,
                 std::set<FeaturePtr>& theReferencesToDelete,
                 const std::string& thePrefixInfo)
{
  QString aNotActivatedDocWrn;
  std::wstring aNotActivatedNames;
  if (!ModelAPI_Tools::allDocumentsActivated(aNotActivatedNames)) {
    if (ModuleBase_Tools::hasModuleDocumentFeature(theFeatures))
      aNotActivatedDocWrn =
        QObject::tr("Selected objects can be used in Part documents which are not loaded: %1.\n")
                            .arg(QString::fromStdWString(aNotActivatedNames));
  }

  std::set<FeaturePtr> aFeaturesRefsTo;
  std::set<FeaturePtr> aFeaturesRefsToParameter;
  std::set<FeaturePtr> aParameterFeatures;
  QStringList aPartFeatureNames;
  std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin(),
                                       aLast = theFeatures.end();
  // separate features to references to parameter features and references to others
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (theReferences.find(aFeature) == theReferences.end())
      continue;

    if (isFeatureOfResult(aFeature, ModelAPI_ResultPart::group()))
      aPartFeatureNames.append(QString::fromStdWString(aFeature->name()));

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
      aParamFeatureNames.append(QString::fromStdWString(aFeature->name()));
    }
  }
  aParamFeatureNames.sort();
  QStringList anOtherFeatureNames;
  anIt = theReferencesToDelete.begin();
  aLast = theReferencesToDelete.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (aFeature->getKind() == "RemoveResults")
      continue; // skip the remove results feature mentioning: result will be removed anyway
    if (isFeatureOfResult(aFeature, ModelAPI_ResultPart::group()))
      aPartFeatureNames.append(QString::fromStdWString(aFeature->name()));
    else
      anOtherFeatureNames.append(QString::fromStdWString(aFeature->name()));
  }
  aPartFeatureNames.sort();
  anOtherFeatureNames.sort();

  QMessageBox aMessageBox(theParent);
  aMessageBox.setWindowTitle(QObject::tr("Delete features"));
  aMessageBox.setIcon(QMessageBox::Warning);
  aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  aMessageBox.setDefaultButton(QMessageBox::No);

  QString aText, aDetailedText;
  if (!thePrefixInfo.empty())
    aText = thePrefixInfo.c_str();
  QString aSep = ", ";
  if (!aPartFeatureNames.empty()) {
    aText += QString(QObject::tr("The following parts will be deleted: %1.\n"))
             .arg(aPartFeatureNames.join(aSep));
  }
  if (!aNotActivatedDocWrn.isEmpty())
    aText += aNotActivatedDocWrn;
  if (!anOtherFeatureNames.empty()) {
    const char* aMsg = "The selected features are used in some\n"
                       "other features, which will also be deleted.\n";
    const char* aMsgDetails = "The selected features are used"
                              " in the following features: %1.\n";
    aText += QString(QObject::tr(aMsg));
    aDetailedText += QString(QObject::tr(aMsgDetails))
                     .arg(anOtherFeatureNames.join(aSep));
  }
  if (!aParamFeatureNames.empty()) {
    const char* aMsg = "The selected parameters are used directly or through\n"
                       "a sequence of dependencies in some features.\n"
                       "These features will be deleted.\n"
                       "Or parameters could be replaced by their values.\n";
    const char* aMsgDetails = "Parameters are used in the following features: %1.\n";
    aText += QString(QObject::tr(aMsg));
    aDetailedText += QString(QObject::tr(aMsgDetails))
                     .arg(aParamFeatureNames.join(aSep));
    aMessageBox.addButton(QObject::tr("Replace"), QMessageBox::ActionRole);
  }

  if (!aText.isEmpty()) {
    aMessageBox.setText(aText);
    aMessageBox.setInformativeText(QObject::tr("Would you like to continue?"));
    if (!aDetailedText.isEmpty())
      aMessageBox.setDetailedText(aDetailedText);
    aMessageBox.exec();
    QMessageBox::ButtonRole aButtonRole = aMessageBox.buttonRole(aMessageBox.clickedButton());

    if (aButtonRole == QMessageBox::NoRole)
      return false;

    if (aButtonRole == QMessageBox::ActionRole) {
      foreach (FeaturePtr aObj, aParameterFeatures)
        ModelAPI_ReplaceParameterMessage::send(aObj, 0);
    }
    else
      theReferencesToDelete.insert(aFeaturesRefsToParameterOnly.begin(),
                                   aFeaturesRefsToParameterOnly.end());
  }
  return true;
}

//**************************************************************
bool warningAboutConflict(QWidget* theParent, const std::string& theWarningText)
{
  QMessageBox aMessageBox(theParent);
  aMessageBox.setWindowTitle(QObject::tr("Conflicts in constraint"));
  aMessageBox.setIcon(QMessageBox::Warning);
  aMessageBox.setText((theWarningText + "\nConstraints will be removed or substituted").c_str());

  QCheckBox* aCheckBox = new QCheckBox;

  aCheckBox->setTristate(false);
  aCheckBox->setText("switch off the notifications.");

  aMessageBox.setCheckBox(aCheckBox);
  aMessageBox.setStandardButtons(QMessageBox::Ok);

  aMessageBox.exec();

  if (aCheckBox->isChecked())
  {
    ModuleBase_Preferences::resourceMgr()->setValue(SKETCH_TAB_NAME,
                                                    "notify_change_constraint", false);
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
    if (aFeature.get())
      theFeatures.insert(aFeature);
  }
}

//**************************************************************
void convertToFolders(const QObjectPtrList& theObjects,
                                         std::set<FolderPtr>& theFolders)
{
  QObjectPtrList::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for(; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    FolderPtr aFeature = std::dynamic_pointer_cast<ModelAPI_Folder>(anObject);
    if (aFeature.get())
      theFolders.insert(aFeature);
  }
}


//**************************************************************
QString translate(const Events_InfoMessage& theMessage)
{
  QString aMessage;

  if (!theMessage.empty()) {
    std::string aStr = Config_Translator::translate(theMessage);
    if (!aStr.empty()) {
      std::string aCodec = Config_Translator::codec(theMessage);
      aMessage = QTextCodec::codecForName(aCodec.c_str())->toUnicode(aStr.c_str());
    }
  }

  return aMessage;
}

QString translate(const std::string& theContext, const std::string& theMessage)
{
  QString aMessage;

  if (!theMessage.empty()) {
    std::string aStr = Config_Translator::translate(theContext, theMessage);
    if (!aStr.empty()) {
      std::string aCodec = Config_Translator::codec(theContext);
      aMessage = QTextCodec::codecForName(aCodec.c_str())->toUnicode(aStr.c_str());
    }
  }

  return aMessage;
}

void setPointBallHighlighting(AIS_InteractiveObject* theAIS)
{
  static Handle(Image_AlienPixMap) aPixMap;
  if(aPixMap.IsNull()) {
    // Load icon for the presentation
    std::string aFile;
    char* anEnv = getenv("SHAPER_ROOT_DIR");
    if(anEnv) {
      aFile = std::string(anEnv) +
        FSEP + "share" + FSEP + "salome" + FSEP + "resources" + FSEP + "shaper";
    } else {
      anEnv = getenv("CADBUILDER_ROOT_DIR");
      if (anEnv)
        aFile = std::string(anEnv) + FSEP + "resources";
    }

    aFile += FSEP;
    static const std::string aMarkerName = "marker_dot.png";
    aFile += aMarkerName;
    aPixMap = new Image_AlienPixMap();
    if(!aPixMap->Load(aFile.c_str())) {
      // The icon for constraint is not found
      static const std::string aMsg =
        "Error: Point market not found by path: \"" + aFile + "\". Falling back.";
      //Events_InfoMessage("ModuleBase_Tools::setPointBallHighlighting", aMsg).send();
    }
  }

  Handle(Graphic3d_AspectMarker3d) anAspect;
  Handle(Prs3d_Drawer) aDrawer = theAIS->DynamicHilightAttributes();
  if (aDrawer.IsNull()) {
    if (ModuleBase_IViewer::DefaultHighlightDrawer.IsNull())
      return;
    aDrawer = new Prs3d_Drawer(*ModuleBase_IViewer::DefaultHighlightDrawer);
    if (!aDrawer->HasOwnPointAspect()) {
      aDrawer->SetPointAspect(new Prs3d_PointAspect(Aspect_TOM_BALL, Quantity_NOC_BLACK, 2.0));
    }
  }
  if(aDrawer->HasOwnPointAspect()) {
    Handle(Prs3d_PointAspect) aPntAspect = aDrawer->PointAspect();
    if(aPixMap->IsEmpty()) {
      anAspect = aPntAspect->Aspect();
      anAspect->SetType(Aspect_TOM_BALL);
    } else {
      if(aPixMap->Format() == Image_PixMap::ImgGray) {
        aPixMap->SetFormat (Image_PixMap::ImgAlpha);
      } else if(aPixMap->Format() == Image_PixMap::ImgGrayF) {
        aPixMap->SetFormat (Image_PixMap::ImgAlphaF);
      }
      anAspect = new Graphic3d_AspectMarker3d(aPixMap);
      aPntAspect->SetAspect(anAspect);
    }
    aDrawer->SetPointAspect(aPntAspect);
	  theAIS->SetDynamicHilightAttributes(aDrawer);
  }
}

FeaturePtr createParameter(const QString& theText)
{
  FeaturePtr aParameter;
  QStringList aList = theText.split("=");
  if (aList.count() != 2) {
    return aParameter;
  }
  QString aParamName = aList.at(0).trimmed();

  if (isNameExist(aParamName, FeaturePtr())) {
    return aParameter;
  }

  if (!ModelAPI_Expression::isVariable(aParamName.toStdString())) {
    return aParameter;
  }

  QString aExpression = aList.at(1).trimmed();
  if (aExpression.isEmpty()) {
    return aParameter;
  }

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  aParameter = aDoc->addFeature("Parameter", false);
  if (aParameter.get()) {
    AttributeStringPtr aNameAttr = aParameter->string("variable");
    aNameAttr->setValue(aParamName.toStdString());

    AttributeStringPtr aExprAttr = aParameter->string("expression");
    aExprAttr->setValue(aExpression.toStdString());
    aParameter->execute();

    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aParameter;
}

void editParameter(FeaturePtr theParam, const QString& theText)
{
  QStringList aList = theText.split("=");
  QString aParamName = aList.at(0).trimmed();

  QString aExpression = aList.at(1).trimmed();
  if (aExpression.isEmpty()) {
    return;
  }

  if (isNameExist(aParamName, theParam)) {
    return;
  }
  AttributeStringPtr aNameAttr = theParam->string("variable");
  aNameAttr->setValue(aParamName.toStdString());

  AttributeStringPtr aExprAttr = theParam->string("expression");
  aExprAttr->setValue(aExpression.toStdString());
  theParam->execute();

  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

bool isNameExist(const QString& theName, FeaturePtr theIgnoreParameter)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();
  FeaturePtr aParamFeature;
  int aNbFeatures = aDoc->numInternalFeatures();
  std::wstring aName = theName.toStdWString();
  for (int i = 0; i < aNbFeatures; i++) {
    aParamFeature = aDoc->internalFeature(i);
    if (aParamFeature && aParamFeature->getKind() == "Parameter") {
      if ((theIgnoreParameter != aParamFeature) && (aParamFeature->name() == aName))
        return true;
    }
  }
  return false;
}

FeaturePtr findParameter(const QString& theName)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();
  FeaturePtr aParamFeature;
  int aNbFeatures = aDoc->numInternalFeatures();
  std::wstring aName = theName.toStdWString();
  for (int i = 0; i < aNbFeatures; i++) {
    aParamFeature = aDoc->internalFeature(i);
    if (aParamFeature && aParamFeature->getKind() == "Parameter") {
      if (aParamFeature->name() == aName)
        return aParamFeature;
    }
  }
  return FeaturePtr();
}


//********************************************************************
std::wstring generateName(const AttributePtr& theAttribute,
  ModuleBase_IWorkshop* theWorkshop)
{
  std::wstring aName;
  if (theAttribute.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
    if (aFeature.get()) {
      std::string aXmlCfg, aDescription;
      theWorkshop->module()->getXMLRepresentation(aFeature->getKind(), aXmlCfg, aDescription);

      ModuleBase_WidgetFactory aFactory(aXmlCfg, theWorkshop);
      std::string anAttributeTitle;
      aFactory.getAttributeTitle(theAttribute->id(), anAttributeTitle);

      std::wstringstream aStreamName;
      aStreamName << theAttribute->owner()->data()->name() << "/" << anAttributeTitle.c_str();
      aName = aStreamName.str();
    }
  }
  return aName;
}

bool isSameShape(const TopoDS_Shape& theShape1, const TopoDS_Shape& theShape2)
{
  // In case of compound we cannot rely on simple comparison method.
  // If the compound is generated by Group feature then this compound is alwais new.
  // So, we have to compare content of these compounds
  if (theShape1.ShapeType() != theShape2.ShapeType())
    return false;

  if (theShape1.ShapeType() != TopAbs_COMPOUND)
    return theShape1.IsSame(theShape2);

  TopoDS_Iterator aIt1(theShape1);
  TopoDS_Iterator aIt2(theShape2);

  for (; aIt1.More() && aIt2.More(); aIt1.Next(), aIt2.Next()) {
    if (!(aIt1.Value()).IsSame(aIt2.Value()))
      return false;
  }
  return true;
}

qreal currentPixelRatio()
{
  QWindowList aWnds = qApp->topLevelWindows();
  if (aWnds.size() > 0)
    return aWnds.first()->devicePixelRatio();
  return qApp->primaryScreen()->devicePixelRatio();
}


// Set displaying status to every element on group
static void setDisplayingByLoop(DocumentPtr theDoc, int theSize,
  std::string theGroup, bool theDisplayFromScript, int theDisplayingId)
{
  for (int anIndex = theSize - 1; anIndex >= 0; --anIndex) {
    ObjectPtr anObject = theDoc->object(theGroup, anIndex);
    anObject->setDisplayed((theDisplayingId == 1 && anIndex == theSize - 1) ||
                           theDisplayingId == 2);
  }
}

void setDisplaying(ResultPartPtr thePart, bool theDisplayFromScript)
{
  static bool isDoingDisplay = false;

  if (isDoingDisplay)
    return;

  isDoingDisplay = true;
  DocumentPtr aDoc = thePart->partDoc();
  int aConstructionSize = aDoc->size(ModelAPI_ResultConstruction::group());
  int aGroupSize = aDoc->size(ModelAPI_ResultGroup::group());
  int aFieldSize = aDoc->size(ModelAPI_ResultField::group());
  int aResultSize = aDoc->size(ModelAPI_ResultBody::group());

  int aDisplayingId = -1;
  if (theDisplayFromScript) {
    aDisplayingId = ModuleBase_Preferences::resourceMgr()->integerValue("General",
      "part_visualization_script", -1);
    // Increase ID to prevert using "As stored in HDF"
    ++aDisplayingId;
  }
  else {
    aDisplayingId = ModuleBase_Preferences::resourceMgr()->integerValue("General",
      "part_visualization_study", -1);

    // if chosen "As stored in HDF" then don't change displaying
    if (aDisplayingId == 0)
      return;
  }

  setDisplayingByLoop(aDoc, aConstructionSize, ModelAPI_ResultConstruction::group(),
                      theDisplayFromScript, aDisplayingId);
  setDisplayingByLoop(aDoc, aGroupSize, ModelAPI_ResultGroup::group(),
                      theDisplayFromScript, aDisplayingId);
  setDisplayingByLoop(aDoc, aFieldSize, ModelAPI_ResultField::group(),
                      theDisplayFromScript, aDisplayingId);
  setDisplayingByLoop(aDoc, aResultSize, ModelAPI_ResultBody::group(),
                      theDisplayFromScript, aDisplayingId);
  isDoingDisplay = false;
}

} // namespace ModuleBase_Tools


