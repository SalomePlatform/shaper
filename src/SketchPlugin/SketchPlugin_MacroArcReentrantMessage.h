// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_MacroArcReentrantMessage.h
// Created: 01 Apr 2017
// Author:  Natalia ERMOLAEVA

#ifndef SketchPlugin_MacroArcReentrantMessage_H_
#define SketchPlugin_MacroArcReentrantMessage_H_

#include <ModelAPI_EventReentrantMessage.h>
#include <Events_Loop.h>

#include <SketchPlugin.h>

#include <memory>

/// Message that style of visualization of parameter is changed.
/// It will be shown as expression or value
class SketchPlugin_MacroArcReentrantMessage : public ModelAPI_EventReentrantMessage
{
public:
  /// Creates an empty message
  SKETCHPLUGIN_EXPORT SketchPlugin_MacroArcReentrantMessage(const Events_ID theID,
                                                            const void* theSender)
  : ModelAPI_EventReentrantMessage(theID, theSender) {}
  /// The virtual destructor
  SKETCHPLUGIN_EXPORT virtual ~SketchPlugin_MacroArcReentrantMessage() {}
  /// Static. Returns EventID of the message.

  inline static Events_ID eventId()
  {
    static const char * MY_EVENT_MACRO_ARC_MESSAGE_ID("MacroArcReentrantMessage");
    return Events_Loop::eventByName(MY_EVENT_MACRO_ARC_MESSAGE_ID);
  }

  /// Stores type of creation
  /// \param the type
  SKETCHPLUGIN_EXPORT void setTypeOfCreation(const std::string& theType)
  { myTypeOfCreation = theType; }

  /// Returns type of creation
  /// \return the type
  SKETCHPLUGIN_EXPORT std::string typeOfCreation() const { return myTypeOfCreation; }

private:
  std::string myTypeOfCreation; ///< to know what parameters of new feature should be filled
};


#endif
