// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <Model_ResultPart.h>
#include <ModelAPI_Data.h>
#include <Model_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeDouble.h>
#include <Model_Document.h>
#include <Model_Application.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Trsf.h>

#include <GeomAlgoAPI_Transform.h>

#include <Locale_Convert.h>

#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

#define baseRef() \
  std::dynamic_pointer_cast<Model_ResultPart>(data()->reference(BASE_REF_ID())->value())

void Model_ResultPart::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  AttributeDocRefPtr aDocRef = std::dynamic_pointer_cast<ModelAPI_AttributeDocRef>(
    data()->addAttribute(DOC_REF(), ModelAPI_AttributeDocRef::typeId()));
  data()->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
  data()->addAttribute(BASE_REF_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(DEFLECTION_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(TRANSPARENCY_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ISO_LINES_ID(), ModelAPI_AttributeIntArray::typeId());

  if (aDocRef->isInitialized() && // initialized immediately means already exist and will be loaded
      !Model_Application::getApplication()->hasDocument(aDocRef->docId()))
    Model_Application::getApplication()->setLoadByDemand(data()->name(), aDocRef->docId());
}

std::shared_ptr<ModelAPI_Document> Model_ResultPart::partDoc()
{
  if (myTrsf.get() && baseRef().get()) { // the second condition is due to #2035
    return baseRef()->partDoc();
  }
  if (!data()->isValid())
    return DocumentPtr();
  DocumentPtr aRes = data()->document(DOC_REF())->value();
  return aRes;
}

Model_ResultPart::Model_ResultPart()
{
}

void Model_ResultPart::activate()
{
  if (myTrsf.get()) {
    baseRef()->activate();
    return;
  }

  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());

  // activation may cause changes in current features in document, so it must be in transaction
  bool isNewTransaction = false;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (!aMgr->isOperation()) {
    // open transaction even document is not created to set current docs in setActiveDocument
    std::string aMsg = "Activation " + Locale::Convert::toString(data()->name());
    aMgr->startOperation(aMsg);
    isNewTransaction = true;
  }
  if (!aDocRef->value().get()) {  // create (or open) a document if it is not yet created
    Handle(Model_Application) anApp = Model_Application::getApplication();
    if (anApp->isLoadByDemand(data()->name(), aDocRef->docId())) {
      anApp->loadDocument(data()->name(), aDocRef->docId()); // if it is just new part, load fails
    } else {
      anApp->createDocument(aDocRef->docId());
    }
    std::shared_ptr<ModelAPI_Document> aDoc = aDocRef->value();
    if (aDoc.get()) {
      aDoc->synchronizeTransactions();
      aDocRef->setValue(aDoc);
    }
  }
  if (aDocRef->value().get()) {
    ModelAPI_Session::get()->setActiveDocument(aDocRef->value());
  }
  if (isNewTransaction) {
    aMgr->finishOperation();
  }
}


void Model_ResultPart::loadPart()
{
  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());
  if (!aDocRef->value().get()) {  // create (or open) a document if it is not yet created
    Handle(Model_Application) anApp = Model_Application::getApplication();
    if (anApp->isLoadByDemand(data()->name(), aDocRef->docId())) {
      anApp->loadDocument(data()->name(), aDocRef->docId()); // if it is just new part, load fails
    }
    else {
      anApp->createDocument(aDocRef->docId());
    }
  }
}


std::shared_ptr<ModelAPI_ResultPart> Model_ResultPart::original()
{
  if (myTrsf.get() && baseRef().get()) {  // the second condition is due to #2035
    return baseRef()->original();
  }
  return std::dynamic_pointer_cast<ModelAPI_ResultPart>(data()->owner());
}

bool Model_ResultPart::isActivated()
{
  if (myTrsf.get()) {
    if (!baseRef().get()) // may be on close
      return false;
    return baseRef()->isActivated();
  }

  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());
  return aDocRef->value().get() != NULL;
}

bool Model_ResultPart::setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag)
{
  if (ModelAPI_ResultPart::setDisabled(theThis, theFlag)) {
    if (!myTrsf.get()) { // disable of base result part
      DocumentPtr aDoc = Model_ResultPart::partDoc();
      if (aDoc.get() && aDoc->isOpened()) {
        // make the current feature the last in any case: to update shapes before deactivation too
        int aSize = aDoc->size(ModelAPI_Feature::group());
        FeaturePtr aLastFeature;
        if (aSize)
          aLastFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aDoc->object(
            ModelAPI_Feature::group(), aSize - 1));

        aDoc->setCurrentFeature(aLastFeature, false);
        if (theFlag) { // disable, so make all features disabled too
          // update the shape just before the deactivation: it will be used outside of part
          updateShape();
          shape();
          aDoc->setCurrentFeature(FeaturePtr(), false);
          // in order to update OB sub-elements of document before the document closing
          Events_Loop* aLoop = Events_Loop::loop();
          aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
          aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
          aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
        }
      }
    }
    return true;
  }
  return false;
}

static GeomShapePtr transformShape(const GeomShapePtr theShape, const gp_Trsf& theTrsf)
{
  GeomShapePtr aResult(new GeomAPI_Shape);
  if (theTrsf.ScaleFactor() > 0) {
    // just update the location of the shape in case of affine transformation
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (!aShape.IsNull()) {
      aShape.Move(theTrsf);
      aResult->setImpl(new TopoDS_Shape(aShape));
    }
  }
  else {
    // all other transformations will modify the shape
    GeomTrsfPtr aTrsf = std::make_shared<GeomAPI_Trsf>(new gp_Trsf(theTrsf));
    GeomAlgoAPI_Transform aTransform(theShape, aTrsf);
    aResult = aTransform.shape();
  }
  return aResult;
}

std::shared_ptr<GeomAPI_Shape> Model_ResultPart::shape()
{
  std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape);
  if (myShape.IsNull()) { // shape is not produced yet, create it
    SessionPtr aMgr = ModelAPI_Session::get();
    bool aToSendUpdate = aMgr->isOperation(); // inside of operation may send an update event
    if (myTrsf.get()) { // get shape of the base result and apply the transformation
      ResultPtr anOrigResult = baseRef();
      std::shared_ptr<GeomAPI_Shape> anOrigShape = anOrigResult->shape();
      if (anOrigShape.get()) {
        aResult = transformShape(anOrigShape, *myTrsf);
        myShape = aResult->impl<TopoDS_Shape>();
      }
      if (!myShape.IsNull() && aToSendUpdate) {
        static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), anEvent);
      }
      return aResult;
    } else {
      DocumentPtr aDoc = Model_ResultPart::partDoc();
      if (aDoc.get() && aDoc->isOpened()) {
        const std::string& aBodyGroup = ModelAPI_ResultBody::group();
        TopoDS_Compound aResultComp;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound(aResultComp);
        int aNumSubs = 0;
        for(int a = aDoc->size(aBodyGroup) - 1; a >= 0; a--) {
          ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(aDoc->object(aBodyGroup, a));
          // "object" method filters out disabled and concealed anyway, so don't check
          if (aBody.get() && aBody->data()->isValid() && aBody->shape().get()) {
            TopoDS_Shape aShape = *(aBody->shape()->implPtr<TopoDS_Shape>());
            if (!aShape.IsNull()) {
              aBuilder.Add(aResultComp, aShape);
              aNumSubs++;
            }
          }
        }
        if (aNumSubs) {
          myShape = aResultComp;
        }
      }
    }
    if (!myShape.IsNull() && aToSendUpdate) {
      static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
      ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), anEvent);
    }
  }
  if (!myShape.IsNull()) {
    aResult->setImpl(new TopoDS_Shape(myShape));
  }
  return aResult;
}

// Returns true is transformation matrices are equal
static bool IsEqualTrsf(gp_Trsf& theT1, gp_Trsf theT2) {
  for(int aRow = 1; aRow < 4; aRow++) {
    for(int aCol = 1; aCol < 5; aCol++) {
      double aDiff = theT1.Value(aRow, aCol) - theT2.Value(aRow, aCol);
      if (Abs(aDiff) > 1.e-9)
        return false;
    }
  }
  return true;
}

std::wstring Model_ResultPart::nameInPart(const std::shared_ptr<GeomAPI_Shape>& theShape,
  int& theIndex)
{
  theIndex = 0; // not initialized

  if (myTrsf.get()) { // if this is moved copy of part => return the name of original shape
    ResultPartPtr anOrigResult = baseRef();
    // searching in the origin the shape equal to the given but with myTrsf
    TopoDS_Shape aSelection = theShape->impl<TopoDS_Shape>();
    gp_Trsf aSelTrsf = aSelection.Location().Transformation();
    TopoDS_Shape anOrigMain = anOrigResult->shape()->impl<TopoDS_Shape>();
    if (!aSelection.IsNull() && !anOrigMain.IsNull()) {
      TopExp_Explorer anExp(anOrigMain, aSelection.ShapeType());
      for(; anExp.More(); anExp.Next()) {
        if (anExp.Current().IsPartner(aSelection)) {
          TopoDS_Shape anOrigMoved = anExp.Current().Moved(*(myTrsf.get()));
          //if (anOrigMoved.IsSame(aSelection)) {
          if (IsEqualTrsf(aSelTrsf, anOrigMoved.Location().Transformation())) {
            std::shared_ptr<GeomAPI_Shape> anOrigSel(new GeomAPI_Shape);
            anOrigSel->setImpl(new TopoDS_Shape(anExp.Current()));
            return anOrigResult->nameInPart(anOrigSel, theIndex);
          }
        }
      }
    }
    // something is not right
    return L"";
  }

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return L"";

  // getting an access to the document of part
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(partDoc());
  if (!aDoc.get()) // the part document is not presented for the moment
    return L"";
  MAYBE_UNUSED TDF_Label anAccessLabel = aDoc->generalLabel();
  // make the selection attribute anyway:
  // otherwise just by name it is not stable to search the result
  std::wstring aName;
  // for this the context result is needed
  ResultPtr aContext;
  const std::string& aBodyGroup = ModelAPI_ResultBody::group();
  for(int a = aDoc->size(aBodyGroup) - 1; a >= 0; a--) {
    ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(aDoc->object(aBodyGroup, a));
    if (aBody.get() && aBody->shape().get() && !aBody->isDisabled()) {
      TopoDS_Shape aBodyShape = *(aBody->shape()->implPtr<TopoDS_Shape>());
      // check is body contain the selected sub-shape
      for(TopExp_Explorer anExp(aBodyShape, aShape.ShapeType()); anExp.More(); anExp.Next()) {
        if (aShape.IsSame(anExp.Current())) {
          aContext = aBody;
          break;
        }
      }
    }
  }
  if (aContext.get()) {
    AttributeSelectionListPtr aSelAttr = aDoc->selectionInPartFeature();
    aSelAttr->append(aContext, theShape);
    theIndex = aSelAttr->size();
    AttributeSelectionPtr aNewAttr = aSelAttr->value(theIndex - 1);
    return aNewAttr->namingName();
  }
  return aName;
}

bool Model_ResultPart::updateInPart(const int theIndex)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(partDoc());
  if (aDoc.get()) {
    AttributeSelectionListPtr aSelAttr = aDoc->selectionInPartFeature();
    AttributeSelectionPtr aThisAttr = aSelAttr->value(theIndex - 1);
    if (aThisAttr.get()) {
      if (aThisAttr->update()) {
        bool aRemove = false;
        aThisAttr->updateInHistory(aRemove);
        return true; // it was updated
      }
    }
  }
  return false; // something is wrong
}

gp_Trsf Model_ResultPart::sumTrsf() {
  gp_Trsf aResult;
  if (myTrsf) {
    aResult = *myTrsf;
    aResult = aResult * baseRef()->sumTrsf();
  }
  return aResult;
}

bool Model_ResultPart::combineGeometrical(const int theIndex, std::wstring& theNewName)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(partDoc());
  if (aDoc.get()) {
    AttributeSelectionListPtr aSelAttr = aDoc->selectionInPartFeature();
    AttributeSelectionPtr aThisAttr = aSelAttr->value(theIndex - 1);
    if (aThisAttr.get()) {
      aThisAttr->combineGeometrical();
      if (aThisAttr->value().get()) {
        int anIndex;
        theNewName = nameInPart(aThisAttr->value(), anIndex);
        return true;
      }
    }
  }
  return false; // something is wrong
}

std::shared_ptr<GeomAPI_Shape> Model_ResultPart::shapeInPart(
  const std::wstring& theName, const std::string& theType, int& theIndex)
{
  theIndex = 0; // not found yet
  std::shared_ptr<GeomAPI_Shape> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(partDoc());
  if (!aDoc.get()) // the part document is not presented for the moment
    return aResult;

  AttributeSelectionListPtr aSelAttr = aDoc->selectionInPartFeature();
  // check this selection is already there: reuse it
  int aSize = aSelAttr->size();
  for(int a = 0; a < aSize; a++) {
    if (aSelAttr->value(a)->namingName() == theName) {
      theIndex = a;
      return aSelAttr->value(a)->value();
    }
  }

  aSelAttr->append(theName, theType);
  theIndex = aSelAttr->size();
  aResult = aSelAttr->value(theIndex - 1)->value();
  if (myTrsf.get() && aResult.get() && !aResult->isNull()) {
    gp_Trsf aSumTrsf = sumTrsf();
    TopoDS_Shape anOrigMoved = aResult->impl<TopoDS_Shape>().Moved(aSumTrsf);
    aResult->setImpl(new TopoDS_Shape(anOrigMoved));
  }
  return aResult;
}

std::shared_ptr<GeomAPI_Shape> Model_ResultPart::selectionValue(const int theIndex)
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(partDoc());
  if (!aDoc.get()) // the part document is not presented for the moment
    return aResult;

  AttributeSelectionListPtr aSelAttr = aDoc->selectionInPartFeature();
  aResult = aSelAttr->value(theIndex - 1)->value();
  if (myTrsf.get() && aResult.get() && !aResult->isNull()) {
    gp_Trsf aSumTrsf = sumTrsf();
    TopoDS_Shape anOrigMoved = aResult->impl<TopoDS_Shape>().Moved(aSumTrsf);
    aResult->setImpl(new TopoDS_Shape(anOrigMoved));
  }
  return aResult;
}

void Model_ResultPart::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_part_color";
  theDefault = DEFAULT_COLOR();
}

void Model_ResultPart::updateShape()
{
  myShape.Nullify();
  myTrsf.reset();
}

void Model_ResultPart::setTrsf(std::shared_ptr<ModelAPI_Result> theThis,
    const std::shared_ptr<GeomAPI_Trsf>& theTransformation)
{
  updateShape();
  if (theTransformation.get()) {
    myTrsf = std::shared_ptr<gp_Trsf>(new gp_Trsf(theTransformation->impl<gp_Trsf>()));
  }
  // the result must be explicitly updated
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(theThis, EVENT_DISP); // flush is in preview-update
}

std::shared_ptr<GeomAPI_Trsf> Model_ResultPart::summaryTrsf()
{
  GeomTrsfPtr aResult(new GeomAPI_Trsf);
  aResult->setImpl<gp_Trsf>(new gp_Trsf(sumTrsf()));
  return aResult;
}
