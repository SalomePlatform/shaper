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

#ifndef Selector_AlgoWithSubs_H_
#define Selector_AlgoWithSubs_H_

#include "Selector_Algo.h"

#include <list>

/**\class Selector_AlgoWithSubs
 * \ingroup DataModel
 * \brief Kind of selection algorithm: generic algorithm that contains sub-algorithms inside.
 * It is base for Container, Intersection and FilterByNeighbours algorithms.
 */
class Selector_AlgoWithSubs: public Selector_Algo
{
  std::list<Selector_Algo*> mySubSelList; ///< list of sub-algorithms
public:
  /// Initializes selector
  Selector_AlgoWithSubs();
  /// Destructor
  virtual ~Selector_AlgoWithSubs();
  /// Erases the sub-algorithms stored
  void clearSubAlgos();
  /// Returns the next label for a new sub-selector created
  TDF_Label newSubLabel();
  /// Appends a new algorithm to the list, erases list if it is null (failed)
  bool append(Selector_Algo* theAlgo, const bool theEraseIfNull = true);
  /// Returns the stored list of sub-algorithms.
  std::list<Selector_Algo*>& list();
};

#endif
