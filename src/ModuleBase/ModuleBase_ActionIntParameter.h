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

#ifndef ModuleBase_ActionIntParameter_H_
#define ModuleBase_ActionIntParameter_H_

#include "ModuleBase_ActionParameter.h"

class ModuleBase_ActionIntParameter : public ModuleBase_ActionParameter
{
public:
  ModuleBase_ActionIntParameter(int theVal):ModuleBase_ActionParameter(), myVal(theVal) {}

  int value() const { return myVal; }

private:
  int myVal;
};

typedef std::shared_ptr<ModuleBase_ActionIntParameter> ActionIntParamPtr;

#endif