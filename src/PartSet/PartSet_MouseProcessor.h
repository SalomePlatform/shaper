// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_MouseProcessor.hxx
// Created:     21 Jul 2016
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_MouseProcessor_H
#define PartSet_MouseProcessor_H

#include "PartSet.h"

class ModuleBase_IViewWindow;
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
};

#endif
