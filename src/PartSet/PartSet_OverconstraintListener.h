// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OverconstraintListener.h
// Created:     8 Feb 2016
// Author:      Natalia ERMOLAEVA


#ifndef PartSet_OverconstraintListener_H
#define PartSet_OverconstraintListener_H

#include <Events_Listener.h>
#include <ModelAPI_Object.h>

class ModuleBase_IWorkshop;
class PartSet_Module;

#include <QString>

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

  /// If active state is changed, update fully defined state and sketch sub-entities color
  /// \param theActive a state
  void setActive(const bool& theActive);

  /// Returns true if the object belongs to internal container of conflicting objects
  /// \param theObject an object to be checked
  /// \param theColor the output container to be filled in [red, green, blue] values
  /// \return boolean result
  void getCustomColor(const ObjectPtr& theObject, std::vector<int>& theColor);

  /// Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// Append objects to the internal container of conflicting object, redisplay necessary objects
  /// \param theObjects a list of new conflicting objects
  /// \return boolean value whether the list differs from the internal list
  bool appendConflictingObjects(const std::set<ObjectPtr>& theObjects);

  /// Removes objects from internal container of conflicting object, redisplay necessary objects
  /// \param theObjects a list of repaired objects
  /// \return boolean value whether the list differs from the internal list
  bool repairConflictingObjects(const std::set<ObjectPtr>& theObjects);

  /// Sends update object signal for each object in the container and flush it.
  /// \param theObjects a list of object to be redisplayed
  void redisplayObjects(const std::set<ObjectPtr>& theObjects);

private:
  /// Returns module
  PartSet_Module* module() const;

#ifdef _DEBUG
  /// Unite objects in one string information
  /// \param theObjects a list of objects
  /// \return a string info
  static QString getObjectsInfo(const std::set<ObjectPtr>& theObjects);
#endif

private:
  ModuleBase_IWorkshop* myWorkshop;
  bool myIsActive; /// state if sketch is active
  std::set<ObjectPtr> myConflictingObjects;
  bool myIsFullyConstrained; /// state if Solver is fully constrained, DOF = 0
};

#endif
