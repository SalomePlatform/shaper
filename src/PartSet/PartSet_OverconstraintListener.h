// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OverconstraintListener.h
// Created:     8 Feb 2016
// Author:      Natalia ERMOLAEVA


#ifndef PartSet_OverconstraintListener_H
#define PartSet_OverconstraintListener_H

#include <Events_Listener.h>
#include <ModelAPI_Object.h>

class ModuleBase_IWorkshop;
class XGUI_Workshop;

#include <vector>
#include <set>

/**
* \ingroup GUI
* A class for processing overconstraint situation. It contains is a list of sketch constraints with
* conflicts.
*/
class PartSet_OverconstraintListener : public Events_Listener
{
public:
  /// Constructor
  /// \param theWorkshop a current workshop to obtain AIS presentation from displayer
  PartSet_OverconstraintListener(ModuleBase_IWorkshop* theWorkshop);

  virtual ~PartSet_OverconstraintListener() {};

  /// Set erroneous color for the presentation of object if the object is in the conflicting list
  /// \param theObject an object to be settled
  /// \param theUpdateViewer a boolean state whether the current viewer should be updated
  //bool customizeObject(ObjectPtr theObject, const bool theUpdateViewer);

  /// Returns true if the object belongs to internal container of conflicting objects
  /// \param theObject an object to be checked
  /// \return boolean result
  bool isConflictingObject(const ObjectPtr& theObject);
  
  /// Returns values of conflicting color
  /// \param theColor the output container to be filled in [red, green, blue] values
  void getConflictingColor(std::vector<int>& theColor);

  /// Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// Updates the internal container of conflicting object, redisplay necessary objects
  /// \param theObjects a list of new conflicting objects
  /// \return boolean value whether the list differs from the internal list
  bool updateConflictingObjects(const std::set<ObjectPtr>& theObjects);

  /// Obtains the object custom AIS presentation and change conflicting state if it exists
  /// \param theObject the object which presentation error state should be changed
  /// \param theConflicting if true, the object state is erroneous, else correct
  void setConflictingObject(const ObjectPtr& theObject, const bool theConflicting);

  /// Sends update object signal for each object in the container and flush it.
  /// \param theObjects a list of object to be redisplayed
  void redisplayObjects(const std::set<ObjectPtr>& theObjects);

private:
  //! Returns workshop
  XGUI_Workshop* workshop() const;

private:
  std::set<ObjectPtr> myConflictingObjects;
  ModuleBase_IWorkshop* myWorkshop;
};

#endif
