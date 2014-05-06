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
  Events_ID aFeatureUpdatedId = aLoop->eventByName(EVENT_FEATURE_UPDATED);
  aLoop->registerListener(this, aFeatureUpdatedId);
}

PartSet_Listener::~PartSet_Listener()
{
}

//******************************************************
void PartSet_Listener::processEvent(const Events_Message* theMessage)
{
  if (QString(theMessage->eventID().eventText()) == EVENT_FEATURE_UPDATED)
  {
    const Model_FeatureUpdatedMessage* aUpdMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Feature> aFeature = aUpdMsg->feature();
    if (myModule->workshop()->displayer()->IsVisible(aFeature))
      myModule->visualizePreview(aFeature, true);
  }
}
