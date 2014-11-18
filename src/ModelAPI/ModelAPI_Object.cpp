// File:        ModelAPI_Object.cpp
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Object.h"


bool ModelAPI_Object::isInHistory() {
  return true;
}

boost::shared_ptr<ModelAPI_Data> ModelAPI_Object::data() const {
  return myData;
}

bool ModelAPI_Object::isSame(const boost::shared_ptr<ModelAPI_Object>& theObject) {
  return theObject.get() == this;
}

boost::shared_ptr<ModelAPI_Document> ModelAPI_Object::document() const {
  return myDoc;
}

void ModelAPI_Object::attributeChanged() {
}

ModelAPI_Object::~ModelAPI_Object() {
}

void ModelAPI_Object::setData(boost::shared_ptr<ModelAPI_Data> theData) {
  myData = theData;
}

void ModelAPI_Object::setDoc(boost::shared_ptr<ModelAPI_Document> theDoc) {
  myDoc = theDoc;
}

void ModelAPI_Object::erase() {
  if (myData) myData->erase();
  setData(DataPtr());
}
