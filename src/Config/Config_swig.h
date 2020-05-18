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

#ifndef SRC_CONFIG_SWIG_H_
#define SRC_CONFIG_SWIG_H_

  #include "Config_ModuleReader.h"
  #include "Config_Prop.h"
  #include "Config_PropManager.h"
  #include "Config_ValidatorReader.h"
  #include "Config_XMLReader.h"

  #ifdef _MSC_VER
  # pragma warning(disable: 4127) // conditional expression is constant
  # pragma warning(disable: 4456) // declaration of variable hides previous local declaration
  # pragma warning(disable: 4459) // declaration of variable hides global declaration
  # pragma warning(disable: 4701) // potentially uninitialized local variable
  # pragma warning(disable: 4703) // potentially uninitialized local pointer variable
  #endif

#endif /* SRC_CONFIG_SWIG_H_ */
