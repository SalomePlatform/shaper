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

#ifndef PartSet_MouseProcessor_H
#define PartSet_MouseProcessor_H

#include "PartSet.h"

#include <memory>

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif

class ModuleBase_IViewWindow;
class ModuleBase_ViewerPrs;
class QMouseEvent;

/**
 * This is an interface to allow processing of mouse events. Implementation of necessary methods
* should be done in a child.
*/
class PartSet_MouseProcessor
{
public:
  /// Processing the mouse move event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  PARTSET_EXPORT virtual void mouseMoved(ModuleBase_IViewWindow* theWindow,
                                         QMouseEvent* theEvent) {}
  /// Processing the mouse press event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  PARTSET_EXPORT virtual void mousePressed(ModuleBase_IViewWindow* theWindow,
                                           QMouseEvent* theEvent) {}
  /// Processing the mouse release event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  PARTSET_EXPORT virtual void mouseReleased(ModuleBase_IViewWindow* theWindow,
                                            QMouseEvent* theEvent) {}
  /// Processing the mouse double click event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  PARTSET_EXPORT virtual void mouseDoubleClick(ModuleBase_IViewWindow* theWindow,
                                               QMouseEvent* theEvent) {}

  /// Fill preselection used in mouseReleased
  virtual void setPreSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected,
                               ModuleBase_IViewWindow* theWnd,
                               QMouseEvent* theEvent) {}
};

#endif
