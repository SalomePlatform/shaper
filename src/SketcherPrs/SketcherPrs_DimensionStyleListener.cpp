// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_DimensionStyleListener.cpp
// Created:     31 March 2016
// Author:      Natalia ERMOLAEVA

#include "SketcherPrs_DimensionStyleListener.h"
#include "SketcherPrs_Tools.h"

#include <Events_Loop.h>

#include <AIS_Dimension.hxx>
#include <TCollection_ExtendedString.hxx>

#define COMPILATION_CORRECTION

// it is not possible to use 0x2211 as summ symbol because it is not supported by
// debian Linux platform
static const Standard_ExtCharacter MyEmptySymbol(' ');
static const Standard_ExtCharacter MySigmaSymbol('=');//0x03A3); // using equal instead of sigma

SketcherPrs_DimensionStyleListener::DimensionValue::DimensionValue(double theDoubleValue,
                                     bool theHasParameters, const std::string& theTextValue)
: myDoubleValue(theDoubleValue),
  myHasParameters(theHasParameters),
  myTextValue(theTextValue)
{
}

void SketcherPrs_DimensionStyleListener::DimensionValue::init(
                                                const AttributeDoublePtr& theAttributeValue)
{
  myDoubleValue = theAttributeValue->value();
  myHasParameters = theAttributeValue->usedParameters().size() > 0;
  myTextValue = theAttributeValue->text();
}

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
          const SketcherPrs_DimensionStyleListener::DimensionValue& theDimensionValue)
{
  if (!theDimension)
    return;
  updateDimensions(theDimension, theDimensionValue.myHasParameters,
                   theDimensionValue.myTextValue, theDimensionValue.myDoubleValue);
}

void SketcherPrs_DimensionStyleListener::updateDimensions(AIS_Dimension* theDimension,
                                                          const bool theHasParameters,
                                                          const std::string& theTextValue,
                                                          const double theDoubleValue)
{
  if (!theDimension)
    return;

  /// do not show special symbols of dimension: previous implementation did not allow to unite them
  theDimension->SetSpecialSymbol(MyEmptySymbol);
  theDimension->SetDisplaySpecialSymbol(AIS_DSS_No);

  TCollection_ExtendedString aCustomValue;
  if (theHasParameters) {
    bool isParameterTextStyle = myStyle == SketcherPrs_ParameterStyleMessage::ParameterText;

    if (isParameterTextStyle)
      aCustomValue = theTextValue.c_str();
    else {
      // format value string using "sprintf"
      TCollection_AsciiString aFormatStr = theDimension->Attributes()->DimensionAspect()->ValueStringFormat();
      char aFmtBuffer[256];
      sprintf (aFmtBuffer, aFormatStr.ToCString(), theDoubleValue);
      aCustomValue = TCollection_ExtendedString (aFmtBuffer);

      aCustomValue.Insert (1, MySigmaSymbol);
    }
  }
  else {
    // format value string using "sprintf"
    TCollection_AsciiString aFormatStr = theDimension->Attributes()->DimensionAspect()->ValueStringFormat();
    char aFmtBuffer[256];
    sprintf (aFmtBuffer, aFormatStr.ToCString(), theDoubleValue);
    aCustomValue = TCollection_ExtendedString (aFmtBuffer);
  }
#ifdef COMPILATION_CORRECTION
  theDimension->SetCustomValue(theDoubleValue);
#else
  theDimension->SetCustomValue(aCustomValue);
#endif
}

