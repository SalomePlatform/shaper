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

#ifndef ModelAPI_EventReentrantMessage_H_
#define ModelAPI_EventReentrantMessage_H_

#include <Events_Message.h>
#include <Events_Loop.h>

#include <ModelAPI.h>

#include <memory>

class ModelAPI_Object;
class ModelAPI_Feature;
class ModelAPI_Attribute;
class GeomAPI_Pnt2d;

/// Message that style of visualization of parameter is changed.
/// It will be shown as expression or value
class ModelAPI_EventReentrantMessage : public Events_Message
{
public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_EventReentrantMessage(const Events_ID theID,
                                                 const void* theSender);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_EventReentrantMessage() {}
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID eventId()
  {
    static const char * MY_EVENT_REENTRANT_MESSAGE_ID("EventReentrantMessage");
    return Events_Loop::eventByName(MY_EVENT_REENTRANT_MESSAGE_ID);
  }

  /// Fills previous feature parameter
  MODELAPI_EXPORT void setCreatedFeature(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  { myCreatedFeature = theFeature; }

  /// Returns previous feature parameter
  MODELAPI_EXPORT const std::shared_ptr<ModelAPI_Feature>& createdFeature() const
  { return myCreatedFeature; }

  /// Fills selected object parameter
  /// \theObject a feature or result
  MODELAPI_EXPORT void setSelectedObject(const std::shared_ptr<ModelAPI_Object>& theObject)
  { mySelectedObject = theObject; }

  /// Returns selected object parameter
  MODELAPI_EXPORT const std::shared_ptr<ModelAPI_Object>& selectedObject() const
  { return mySelectedObject; }

  /// Fills selected attribute parameter
  /// \theAttribute
  MODELAPI_EXPORT void setSelectedAttribute
                                  (const std::shared_ptr<ModelAPI_Attribute>& theAttribute)
  { mySelectedAttribute = theAttribute; }

  /// Returns selected attribute parameter
  MODELAPI_EXPORT const std::shared_ptr<ModelAPI_Attribute>& selectedAttribute()
  { return mySelectedAttribute; }

  /// Fills clicked point
  /// \thePoint
  MODELAPI_EXPORT void setClickedPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
  { myClickedPoint = thePoint; }

  /// Returns clicked point
  MODELAPI_EXPORT const std::shared_ptr<GeomAPI_Pnt2d>& clickedPoint()
  { return myClickedPoint; }

private:
  std::shared_ptr<ModelAPI_Feature> myCreatedFeature; ///< previous object
  std::shared_ptr<ModelAPI_Object> mySelectedObject; ///< selected object
  std::shared_ptr<ModelAPI_Attribute> mySelectedAttribute; ///< selected attribute
  std::shared_ptr<GeomAPI_Pnt2d> myClickedPoint; ///< clicked point
};

typedef std::shared_ptr<ModelAPI_EventReentrantMessage> ReentrantMessagePtr;


#endif
