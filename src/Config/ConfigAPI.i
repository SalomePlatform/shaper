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

/* Config.i */

%module ConfigAPI
%{
  #include "Config_swig.h"
%}

// to avoid error on this
#define CONFIG_EXPORT

%include "typemaps.i"
%include "std_list.i"
%include "std_string.i"
%include "std_vector.i"

%include "Config_ModuleReader.h"
%include "Config_PropManager.h"
%include "Config_Prop.h"
%include "Config_ValidatorReader.h"
%include "Config_XMLReader.h"

// std::list -> []
%template(ListOfString) std::list< std::string >;
%template(listOfProp) std::list< Config_Prop* >;
// std::vector -> []
%template(VectorOfInteger) std::vector<int>;
