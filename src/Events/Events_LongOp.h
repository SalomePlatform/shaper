// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef EVENTS_LONGOP_H_
#define EVENTS_LONGOP_H_

#include <Events.h>
#include <Events_Message.h>

/**
 * Informs the application that the long operation is performed.
 * Causes waiting coursor in GUI.
 */
class EVENTS_EXPORT Events_LongOp : public Events_Message
{
public:
  /// Default destructor
  virtual ~Events_LongOp();
  /// Returns the identifier of this event
  static Events_ID eventID();
  /// Starts the long operation
  static void start(void* theSender = 0);
  /// Stops the long operation
  static void end(void* theSender = 0);
  /// Returns true if the long operation is performed
  static bool isPerformed();

protected:
  /// Default constructor. Use "start" and "end" for generation.
  Events_LongOp(void* theSender = 0);
};

#endif /* EVENTS_ERROR_H_ */
