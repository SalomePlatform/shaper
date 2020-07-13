// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
#include <Events_InfoMessage.h>

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Result.h>

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

void ModelHighAPI_Interface::setName(const std::wstring& theName)
{
  if (feature().get() && feature()->data()->isValid())
    feature()->data()->setName(theName);
  else {
    std::cout<<"Error: set name "<<theName.c_str()<<" for an invalid feature"<<std::endl;
  }
}

std::wstring ModelHighAPI_Interface::name() const
{
  return feature()->data()->name();
}

ModelHighAPI_Selection ModelHighAPI_Interface::result() const
{
  std::list<ModelHighAPI_Selection> aResults = results();
  if (aResults.empty())
    return ModelHighAPI_Selection(std::shared_ptr<ModelAPI_Result>());
  return aResults.front();
}

std::list<ModelHighAPI_Selection> ModelHighAPI_Interface::results() const
{
  const_cast<ModelHighAPI_Interface*>(this)->execute();

  std::list<ModelHighAPI_Selection> aSelectionList;

  if (feature().get())  {
    std::list<std::shared_ptr<ModelAPI_Result> > aResults = feature()->results();
    for (auto it = aResults.begin(), end = aResults.end(); it != end; ++it) {
      if (!(*it)->isDisabled())
        aSelectionList.push_back(ModelHighAPI_Selection(*it));
    }
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
