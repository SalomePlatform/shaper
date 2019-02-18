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

#include "ModelAPI_Expression.h"

ModelAPI_Expression::ModelAPI_Expression()
{

}

ModelAPI_Expression::~ModelAPI_Expression()
{

}

bool ModelAPI_Expression::isInitialized()
{
  return myIsInitialized;
}

void ModelAPI_Expression::setInitialized()
{
  myIsInitialized = true;
}

ModelAPI_ExpressionDouble::ModelAPI_ExpressionDouble()
{

}

ModelAPI_ExpressionInteger::ModelAPI_ExpressionInteger()
{


}
bool ModelAPI_Expression::isVariable(const std::string& theString)
{
  if (theString.empty())
    return false;
  try {
    std::string::const_iterator it = theString.begin();
    if (!(isalpha(*it) || (*it) == '_') || it == theString.end())
      return false;
    it++;
    for ( ; it != theString.end(); ++it ) {
      if(!(isalnum(*it) || (*it) == '_')) {
        return false;
      }
    }
  } catch(...) {
    return false;
  }
  return true;
}

