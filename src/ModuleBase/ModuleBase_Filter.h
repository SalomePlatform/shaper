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

#ifndef ModuleBase_Filter_H
#define ModuleBase_Filter_H

#include "ModuleBase.h"

#include <SelectMgr_Filter.hxx>

#include <list>

/**
 * \ingroup GUI
 * This object is assigned by the name
 * in the XML file to the specific attribute or to the whole feature.
 * If isOK method of the filter returns "false", it is signalized in user interface
 * that the processed entity should not be selected.
 * Filterss must be registered in the filters factory to be
 * correctly identified by the XML string-ID.
 */
class ModuleBase_Filter
{
public:
  /// Constructor
  MODULEBASE_EXPORT ModuleBase_Filter() {}

  /**
   * Returns an OCC selection filter. It can be appended into the context of a viewer
   * It creates a filter if it has not be created yet.
   * \return the selection filter
   */
  MODULEBASE_EXPORT Handle(SelectMgr_Filter) getFilter();

  /**
   * Sets the arguments to the filter.
   * \param theArguments a list of arguments
   */
  MODULEBASE_EXPORT virtual void setArguments(const std::list<std::string>& theArguments) = 0;
protected:
  /**
   * It creates an internal instance of the OCC filter
   */
  virtual void createFilter() = 0;

protected:
  /// current instance of the OCC selection filter
  Handle(SelectMgr_Filter) myFilter;
};

#endif //ModuleBase_Filter
