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

#ifndef Selector_NameGenerator_H_
#define Selector_NameGenerator_H_

#include "Selector.h"

#include <TDF_Label.hxx>

/**\class Selector_NameGenerator
 * \ingroup DataModel
 * \brief An interface for generation of the naming name basing on the current selection and
 * document information. This requires higher-level data access, so, the caller of "name" method
 * of Selector must implement this helper-class abstract methods.
 */
class Selector_NameGenerator
{
public:
  /// empty constructor, nothing to add
  Selector_NameGenerator() {};

  /// This method returns the context name by the label of the sub-selected shape
  virtual std::string contextName(const TDF_Label theSelectionLab) = 0;

  /// This method restores by the context and value name the context label and
  /// sub-label where the value is. Returns true if it is valid.
  virtual bool restoreContext(std::string theName,
    TDF_Label& theContext, TDF_Label& theValue) = 0;

  /// Returns true if the first result is older than the second one in the tree of features
  virtual bool isLater(const TDF_Label theResult1, const TDF_Label theResult2) const = 0;
};

#endif
