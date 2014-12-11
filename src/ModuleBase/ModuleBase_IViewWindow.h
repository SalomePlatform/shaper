// Copyright (C) 2014-20xx CEA/DEN, EDF R&D



#ifndef ModuleBase_IViewWindow_H
#define ModuleBase_IViewWindow_H

#include <V3d_View.hxx>


/** 
* Interface to ViewWindow object which contains 3d scene
*/
class ModuleBase_IViewWindow
{
public:
  /// Returns OCCT object which contains 3d view object
  virtual Handle(V3d_View) v3dView() const = 0;
};


#endif