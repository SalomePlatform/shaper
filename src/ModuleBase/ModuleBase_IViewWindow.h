// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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