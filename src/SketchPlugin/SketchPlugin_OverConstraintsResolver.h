// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_OverConstraintsResolver_H_
#define SketchPlugin_OverConstraintsResolver_H_

#include "SketchPlugin.h"

#include <Events_Listener.h>

#include <ModelAPI_Object.h>

#include <set>

class SketchPlugin_OverConstraintsResolver : public Events_Listener
{
public:
  SketchPlugin_OverConstraintsResolver();

  /// Redefinition of Events_Listener method
  void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  virtual bool groupMessages() { return true; }

protected:
  /// Perform algorithm
  bool perform();

  /// Set set of constraints to check
  void setConstraints(const std::set<ObjectPtr>& theConstraints);

  /// Check arcs in sketch about tangential conflict
  bool checkArcsAboutTangentialConflict();

  /// Check lines in sketch about horizontal or vertical conflict
  bool checkHorizontalOrVerticalConflict();

private:
  std::set<ObjectPtr> myConstraints;
  std::set<ObjectPtr> myConstraintsToRemove;
};
#endif
