// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Nathalie Gore (OpenCascade), Frederic Pons (OpenCascade)

#include "XAO_XaoUtils.hxx"
#include "XAO_Group.hxx"

using namespace XAO;


Group::Group(const XAO::Dimension& dim, const int& nbElements, const std::string& name)
throw (XAO_Exception)
{
    if (dim == XAO::WHOLE)
        throw XAO_Exception("Dimension WHOLE is not valid for group.");

    m_name = name;
    m_dimension = dim;
    m_count = 0;
    m_nbElements = nbElements;
}

Group::~Group()
{
}

void Group::checkIndex(const int& element)
throw (XAO_Exception)
{
    if (element < m_elements.size() && element >= 0)
        return;

    throw XAO_Exception(MsgBuilder() << "Index of element is out of range [0, "
                                     << m_elements.size()-1 << "]: " << element);
}

void Group::add(const int& value)
{
    m_elements.insert(value);
}

void Group::remove(const int& value)
{
    m_elements.erase(value);
}
