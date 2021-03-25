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

#ifndef Model_Tools_H_
#define Model_Tools_H_

#include <Model.h>

#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

#include <memory>
#include <set>

/// A collection of methods useful for different parts of data model.
class Model_Tools
{
public:
  /// makes copy of label and all its sub-labels without copying the attributes;
  /// and feel the relocation table
  static void copyLabels(TDF_Label theSource, TDF_Label theDestination,
                         Handle(TDF_RelocationTable) theRelocTable);

  /// makes copy of all attributes on the given label and all sub-labels
  static void copyAttrs(TDF_Label theSource, TDF_Label theDestination,
                        Handle(TDF_RelocationTable) theRelocTable = Handle(TDF_RelocationTable)());

  /// makes copy of all attributes on the given label and all sub-labels,
  /// but keep references to the Origin, coordinate axes and coordinate planes
  static void copyAttrsAndKeepRefsToCoordinates(TDF_Label theSource, TDF_Label theDestination,
      const std::set<TCollection_AsciiString>& theCoordinateLabels,
      Handle(TDF_RelocationTable) theRelocTable);

  /// collect labels of coordinate planes, axes, and origin
  static void labelsOfCoordinates(
      std::set<TCollection_AsciiString>& theCoordinateLabels,
      Handle(TDF_RelocationTable) theRelocTable);
};

#endif
