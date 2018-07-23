// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_Events_H_
#define Model_Events_H_

#include <Model.h>
#include <ModelAPI_Events.h>

#include <memory>

/// Allovs to create ModelAPI messages
class Model_EventCreator : public ModelAPI_EventCreator
{
 public:
  /// creates created, updated or moved messages and sends to the loop
  virtual void sendUpdated(const ObjectPtr& theObject, const Events_ID& theEvent,
                           const bool isGroupped = true) const;
  /// creates deleted message and sends to the loop
  virtual void sendDeleted(const std::shared_ptr<ModelAPI_Document>& theDoc,
                           const std::string& theGroup) const;

  /// creates reordered message and sends to the loop
  virtual void sendReordered(const std::shared_ptr<ModelAPI_Feature>& theReordered) const;

  /// must be one per application, the constructor for internal usage only
  Model_EventCreator();
};

/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class Model_ObjectUpdatedMessage : public ModelAPI_ObjectUpdatedMessage
{
  std::set<ObjectPtr> myObjects;  ///< which feature is changed

  /// Sender is not important, all information is located in the feature.
  /// Use ModelAPI for creation of this event. Used for creation, movement and edition events.
  Model_ObjectUpdatedMessage(const ObjectPtr& theObject, const Events_ID& theEvent);

  friend class Model_EventCreator;
 public:

  /// Returns the feature that has been updated
  virtual const std::set<ObjectPtr>& objects() const;

  //! Creates a new empty group (to store it in the loop before flush)
  virtual std::shared_ptr<Events_MessageGroup> newEmpty();

  //! Allows to join the given message with the current one
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined);
};

/// Message that feature was deleted (used for Object Browser update)
class Model_ObjectDeletedMessage : public ModelAPI_ObjectDeletedMessage
{
  ///< group identifiers that contained the deleted feature and document where they are deleted
  std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string> > myGroups;

  /// Use ModelAPI for creation of this event.
  Model_ObjectDeletedMessage(const std::shared_ptr<ModelAPI_Document>& theDoc,
                             const std::string& theGroup);

  friend class Model_EventCreator;
 public:
  /// Returns the group where the objects were deleted
  virtual const std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string> >&
    groups() const
  {
    return myGroups;
  }

  /// Returns the new empty message of this type
  virtual std::shared_ptr<Events_MessageGroup> newEmpty();

  /// Returns the identifier of this message
  virtual const Events_ID messageId();

  /// Appends to this message the given one
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined);
};

/// Message that feature was deleted (used for Object Browser update)
class Model_OrderUpdatedMessage : public ModelAPI_OrderUpdatedMessage
{
  std::shared_ptr<ModelAPI_Feature> myReordered;  ///< the feature that was moved

  /// Use ModelAPI for creation of this event.
  Model_OrderUpdatedMessage(FeaturePtr theReordered,
                            const void* theSender = 0);

  friend class Model_EventCreator;
 public:
  /// Returns the document that has been updated
  virtual std::shared_ptr<ModelAPI_Feature> reordered()
  {
    return myReordered;
  }

  /// Returns the identifier of this message
  virtual const Events_ID messageId();
};

#endif
