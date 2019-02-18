// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
