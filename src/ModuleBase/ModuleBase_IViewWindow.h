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

#ifndef ModuleBase_IViewWindow_H
#define ModuleBase_IViewWindow_H

#include "ModuleBase.h"

#include <V3d_View.hxx>

class QWidget;

/** 
* \ingroup GUI
* Interface to ViewWindow object which contains 3d scene
*/
class MODULEBASE_EXPORT ModuleBase_IViewWindow
{
public:
  /// Default constructor
  ModuleBase_IViewWindow();
  /// Virtual destructor
  virtual ~ModuleBase_IViewWindow();

  /// Returns OCCT object which contains 3d view object
  virtual Handle(V3d_View) v3dView() const = 0;

  /// Returns the view window view port
  virtual QWidget* viewPort() const = 0;
};


#endif