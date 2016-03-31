// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_DimensionStyleListener.cpp
// Created:     31 March 2016
// Author:      Natalia ERMOLAEVA

#include "SketcherPrs_DimensionStyleListener.h"
#include "SketcherPrs_Tools.h"

#include <Events_Error.h>
#include <Events_Loop.h>

SketcherPrs_DimensionStyleListener::SketcherPrs_DimensionStyleListener()
{
  Events_Loop* aLoop = Events_Loop::loop();
  const Events_ID kDocCreatedEvent =
                SketcherPrs_ParameterStyleMessage::eventId();
  aLoop->registerListener(this, kDocCreatedEvent, NULL, false);
}

SketcherPrs_DimensionStyleListener::~SketcherPrs_DimensionStyleListener()
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->removeListener(this);
}

void SketcherPrs_DimensionStyleListener::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  const Events_ID kParameterStyleEvent = SketcherPrs_ParameterStyleMessage::eventId();
  if (theMessage->eventID() == kParameterStyleEvent) {
    std::shared_ptr<SketcherPrs_ParameterStyleMessage> aMessage = std::dynamic_pointer_cast<
                                            SketcherPrs_ParameterStyleMessage>(theMessage);
    myStyle = aMessage->style();
  }
}

void SketcherPrs_DimensionStyleListener::updateDimensions(AIS_Dimension* theDimension,
                                        const AttributeDoublePtr& theAttributeValue)
{
  if (!theDimension || !theAttributeValue.get())
    return;

  bool aHasParameters = theAttributeValue->usedParameters().size() > 0;
  if (aHasParameters) {
    bool isParameterValueStyle = myStyle == SketcherPrs_ParameterStyleMessage::ParameterValue;
    SketcherPrs_Tools::setDisplaySpecialSymbol(theDimension, isParameterValueStyle);
    SketcherPrs_Tools::setDisplayParameter(theDimension, theAttributeValue->text(), !isParameterValueStyle);
  }
  else {
    SketcherPrs_Tools::setDisplaySpecialSymbol(theDimension, false);
    SketcherPrs_Tools::setDisplayParameter(theDimension, theAttributeValue->text(), false);
  }
}
