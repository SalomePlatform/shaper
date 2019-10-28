// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef Model_Tools_H_
#define Model_Tools_H_

#include <Model.h>

#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

/// A collection of methods useful for different parts of data model.
namespace Model_Tools
{
  /// makes copy of label and all its sub-labels without copying the attributes;
  /// and feel the relocation table
  void copyLabels(TDF_Label theSource, TDF_Label theDestination,
                  Handle(TDF_RelocationTable) theRelocTable);

  /// makes copy of all attributes on the given label and all sub-labels
  void copyAttrs(TDF_Label theSource, TDF_Label theDestination,
                 Handle(TDF_RelocationTable) theRelocTable = Handle(TDF_RelocationTable)());
};

#endif
