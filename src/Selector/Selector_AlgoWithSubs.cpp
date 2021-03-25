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

#include <Selector_AlgoWithSubs.h>

Selector_AlgoWithSubs::Selector_AlgoWithSubs() : Selector_Algo()
{}

void Selector_AlgoWithSubs::clearSubAlgos()
{
  std::list<Selector_Algo*>::iterator anAlgo = mySubSelList.begin();
  for(; anAlgo != mySubSelList.end(); anAlgo++) {
    delete *anAlgo;
  }
  mySubSelList.clear();
}

Selector_AlgoWithSubs::~Selector_AlgoWithSubs()
{
  clearSubAlgos();
}

TDF_Label Selector_AlgoWithSubs::newSubLabel()
{
  return label().FindChild(int(mySubSelList.size() + 1));
}

bool Selector_AlgoWithSubs::append(Selector_Algo* theAlgo, const bool theEraseIfNull)
{
  if (theAlgo) {
    mySubSelList.push_back(theAlgo);
    return true;
  }
  if (theEraseIfNull)
    clearSubAlgos();
  return false;
}

std::list<Selector_Algo*>& Selector_AlgoWithSubs::list()
{
  return mySubSelList;
}
