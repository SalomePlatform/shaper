// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultPart.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultPart.h>
#include <ModelAPI_Data.h>
#include <Model_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <Model_Document.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

void Model_ResultPart::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
}

std::shared_ptr<ModelAPI_Document> Model_ResultPart::partDoc()
{
  return data()->document("PartDocument")->value();
}

std::shared_ptr<ModelAPI_Feature> Model_ResultPart::owner()
{
  return std::shared_ptr<ModelAPI_Feature>();  // return empty pointer
}

Model_ResultPart::Model_ResultPart()
{
  myIsDisabled = true; // by default it is not initialized and false to be after created
  setIsConcealed(false);
}

void Model_ResultPart::setData(std::shared_ptr<ModelAPI_Data> theData)
{
  ModelAPI_Result::setData(theData);
  if (theData) {
    data()->addAttribute(DOC_REF(), ModelAPI_AttributeDocRef::typeId());
  }
}

void Model_ResultPart::activate()
{
  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());
  
  if (!aDocRef->value().get()) {  // create (or open) a document if it is not yet created
    std::shared_ptr<ModelAPI_Document> aDoc = document()->subDocument(data()->name());
    if (aDoc) {
      aDocRef->setValue(aDoc);
    }
  }
  if (aDocRef->value().get()) {
    SessionPtr aMgr = ModelAPI_Session::get();
    bool isNewTransaction = !aMgr->isOperation();
    // activation may cause changes in current features in document, so it must be in transaction
    if (isNewTransaction) {
      aMgr->startOperation("Activation");
    }
    ModelAPI_Session::get()->setActiveDocument(aDocRef->value());
    if (isNewTransaction) {
      aMgr->finishOperation();
    }
  }
}

bool Model_ResultPart::isActivated() 
{
  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());
  return aDocRef->value().get();
}

bool Model_ResultPart::setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag)
{
  if (ModelAPI_ResultPart::setDisabled(theThis, theFlag)) {
    DocumentPtr aDoc = Model_ResultPart::partDoc();
    if (aDoc.get() && aDoc->isOpened()) {
      // make the current feature the last in any case: to update shapes defore deactivation too
      FeaturePtr aLastFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aDoc->object(
        ModelAPI_Feature::group(), aDoc->size(ModelAPI_Feature::group()) - 1));
      aDoc->setCurrentFeature(aLastFeature, false);
      if (theFlag) { // disable, so make all features disabled too
        // update the shape just before the deactivation: it will be used outside of part
        myShape.Nullify();
        shape();
        aDoc->setCurrentFeature(FeaturePtr(), false);
      }
    }
    return true;
  }
  return false;
}

std::shared_ptr<GeomAPI_Shape> Model_ResultPart::shape()
{
  if (myShape.IsNull()) {
    DocumentPtr aDoc = Model_ResultPart::partDoc();
    if (aDoc.get() && aDoc->isOpened()) {
      const std::string& aBodyGroup = ModelAPI_ResultBody::group();
      TopoDS_Compound aResultComp;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound(aResultComp);
      int aNumSubs = 0;
      for(int a = aDoc->size(aBodyGroup) - 1; a >= 0; a--) {
        ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(aDoc->object(aBodyGroup, a));
        if (aBody.get() && aBody->shape().get() && !aBody->isDisabled()) {
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
  if (myShape.IsNull())
    return std::shared_ptr<GeomAPI_Shape>();
  std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape);
  aResult->setImpl(new TopoDS_Shape(myShape));
  return aResult;
}

std::string Model_ResultPart::nameInPart(const std::shared_ptr<GeomAPI_Shape>& theShape,
  int& theIndex)
{
  theIndex = 0; // not initialized
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return "";
  if (data()->isOwner(this)) { // if this is moved copy of part => return the name of original shape
    FeaturePtr anOrigFeature = 
      std::dynamic_pointer_cast<ModelAPI_Feature>(data()->attribute(COLOR_ID())->owner());
    if (anOrigFeature.get()) {
      if (anOrigFeature->firstResult().get() && anOrigFeature->firstResult()->shape().get()) {
        TopoDS_Shape anOrigShape = anOrigFeature->firstResult()->shape()->impl<TopoDS_Shape>();
        if (!anOrigShape.IsNull()) {
          TopExp_Explorer anExp(anOrigShape, aShape.ShapeType());
          for(; anExp.More(); anExp.Next()) {
            if (aShape.IsPartner(anExp.Current())) {
              std::shared_ptr<GeomAPI_Shape> anOrigGeomShape(new GeomAPI_Shape);
              anOrigGeomShape->setImpl(new TopoDS_Shape(anExp.Current()));

              return std::dynamic_pointer_cast<Model_ResultPart>(anOrigFeature->firstResult())->
                nameInPart(theShape, theIndex);
            }
          }
        }
      }
    }
  }
  // getting an access to the document of part
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(partDoc());
  if (!aDoc.get()) // the part document is not presented for the moment
    return "";
  TDF_Label anAccessLabel = aDoc->generalLabel();

  std::string aName;
  // check if the subShape is already in DF
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aShape, anAccessLabel);
  Handle(TDataStd_Name) anAttr;
  if(!aNS.IsNull() && !aNS->IsEmpty()) { // in the document    
    if(aNS->Label().FindAttribute(TDataStd_Name::GetID(), anAttr)) {
      aName = TCollection_AsciiString(anAttr->Get()).ToCString();
      if(!aName.empty()) {	    
        const TDF_Label& aLabel = aDoc->findNamingName(aName);

        static const std::string aPostFix("_");
        TNaming_Iterator anItL(aNS);
        for(int i = 1; anItL.More(); anItL.Next(), i++) {
          if(anItL.NewShape() == aShape) {
            aName += aPostFix;
            aName += TCollection_AsciiString (i).ToCString();
            break;
          }
        }
      }	
    }
  }
  if (aName.empty()) { // not found, so use the selection mechanism
    // for this the context result is needed
    ResultPtr aContext;
    const std::string& aBodyGroup = ModelAPI_ResultBody::group();
    for(int a = aDoc->size(aBodyGroup) - 1; a >= 0; a--) {
      ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(aDoc->object(aBodyGroup, a));
      if (aBody.get() && aBody->shape().get() && !aBody->isDisabled()) {
        TopoDS_Shape aBodyShape = *(aBody->shape()->implPtr<TopoDS_Shape>());
        // check is body contain the selected sub-shape
        for(TopExp_Explorer anExp(aBodyShape, aShape.ShapeType()); anExp.More(); anExp.Next()) {
          if (aShape.IsEqual(anExp.Current())) {
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
      return aThisAttr->update();
    }
  }
  return false; // something is wrong
}

std::shared_ptr<GeomAPI_Shape> Model_ResultPart::shapeInPart(const std::string& theName)
{
  /// TODO: not implemented yet
  return std::shared_ptr<GeomAPI_Shape>();
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
}

void Model_ResultPart::setShape(std::shared_ptr<ModelAPI_Result> theThis, 
    const std::shared_ptr<GeomAPI_Shape>& theTransformed)
{
  myShape = theTransformed->impl<TopoDS_Shape>();
  // the result must be explicitly updated
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(theThis, EVENT_DISP); // flush is in preview-update
}
