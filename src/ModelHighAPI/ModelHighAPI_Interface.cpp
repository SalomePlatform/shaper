// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_Interface.cpp
// Purpose:
//
// History:
// 17/05/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
#include <Events_InfoMessage.h>

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include "ModelHighAPI_Selection.h"
//--------------------------------------------------------------------------------------
ModelHighAPI_Interface::ModelHighAPI_Interface(
  const std::shared_ptr<ModelAPI_Feature> & theFeature)
: myFeature(theFeature)
{

}

ModelHighAPI_Interface::~ModelHighAPI_Interface()
{

}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Feature> ModelHighAPI_Interface::feature() const
{
  return myFeature;
}

std::shared_ptr<ModelHighAPI_Interface> ModelHighAPI_Interface::subFeature(const int theIndex) const
{
  CompositeFeaturePtr aCompositeFeature =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if(!aCompositeFeature.get()) {
    return InterfacePtr();
  }

  FeaturePtr aSubFeature = aCompositeFeature->subFeature(theIndex);
  if(!aSubFeature.get()) {
    return InterfacePtr();
  }

  return InterfacePtr(new ModelHighAPI_Interface(aSubFeature));
}

const std::string& ModelHighAPI_Interface::getKind() const
{
  return feature()->getKind();
}

void ModelHighAPI_Interface::execute(bool isForce)
{
  if (isForce) {
    SessionPtr aMgr = ModelAPI_Session::get();
    ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
    FeaturePtr aFeature = feature();
    if(aFactory->validate(aFeature))
      aFeature->execute();
  }

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  //aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  //aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
}

void ModelHighAPI_Interface::setName(const std::string& theName)
{
  feature()->data()->setName(theName);
}

ModelHighAPI_Selection ModelHighAPI_Interface::result() const
{
  const_cast<ModelHighAPI_Interface*>(this)->execute();

  return ModelHighAPI_Selection(feature()->firstResult());
}

std::list<ModelHighAPI_Selection> ModelHighAPI_Interface::results() const
{
  const_cast<ModelHighAPI_Interface*>(this)->execute();

  std::list<ModelHighAPI_Selection> aSelectionList;

  std::list<std::shared_ptr<ModelAPI_Result> > aResults = feature()->results();
  for (auto it = aResults.begin(), end = aResults.end(); it != end; ++it) {
    aSelectionList.push_back(ModelHighAPI_Selection(*it));
  }

  return aSelectionList;
}

std::shared_ptr<ModelAPI_Result> ModelHighAPI_Interface::defaultResult() const
{
  const_cast<ModelHighAPI_Interface*>(this)->execute();

  return feature()->lastResult();
}

void ModelHighAPI_Interface::throwException(const std::string & theDescription)
{
  Events_InfoMessage("ModelHighAPI_Interface", theDescription).send();
}

const std::string& ModelHighAPI_Interface::attributeGetter(const std::string& theAttrName)
{
  return myAttrGetter[theAttrName];
}
