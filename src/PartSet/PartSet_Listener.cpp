// File:        PartSet_Listener.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Listener.h>

#include <PartSet_Module.h>

#include <XGUI_Displayer.h>

#include <Events_Loop.h>
#include <Model_Events.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_Listener::PartSet_Listener(PartSet_Module* theModule)
: myModule(theModule)
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, aLoop->eventByName(EVENT_FEATURE_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_DELETED));
}

PartSet_Listener::~PartSet_Listener()
{
}

//******************************************************
void PartSet_Listener::processEvent(const Events_Message* theMessage)
{
  QString aType = QString(theMessage->eventID().eventText());
  if (aType == EVENT_FEATURE_UPDATED ||
      aType == EVENT_FEATURE_CREATED)
  {
    const Model_FeatureUpdatedMessage* aUpdMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Feature> aFeature = aUpdMsg->feature();
    if (myModule->workshop()->displayer()->IsVisible(aFeature) ||
        aType == EVENT_FEATURE_CREATED)
      myModule->visualizePreview(aFeature, true);
  }
  if (aType == EVENT_FEATURE_DELETED)
  {
    const Model_FeatureDeletedMessage* aDelMsg = dynamic_cast<const Model_FeatureDeletedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Document> aDoc = aDelMsg->document();

    std::string aGroup = aDelMsg->group();
    if (aDelMsg->group().compare("Sketch") == 0) { // Update only Sketch group
      myModule->workshop()->displayer()->EraseDeletedFeatures();
      myModule->updateCurrentPreview(aGroup);
    }
  }
}
