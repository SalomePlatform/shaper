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
// Author : Frederic Pons (OpenCascade)

#include "XAO_Xao.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Step.hxx"
#include "XAO_BooleanStep.hxx"
#include "XAO_IntegerStep.hxx"
#include "XAO_DoubleStep.hxx"
#include "XAO_StringStep.hxx"

using namespace XAO;

void Step::checkElementIndex(const int& element)
throw (XAO_Exception)
{
    if (element < m_nbElements && element >= 0)
        return;

    throw XAO_Exception(MsgBuilder() << "Element index is out of range [0, "
                                     << m_nbElements-1 << "]: " << element);
}

void Step::checkComponentIndex(const int& component)
throw (XAO_Exception)
{
    if (component < m_nbComponents && component >= 0)
        return;

    throw XAO_Exception(MsgBuilder() << "Component index is out of range [0, "
                                     << m_nbComponents-1 << "]: " << component);
}

void Step::checkNbElements(const int& nbElements)
throw (XAO_Exception)
{
    if (nbElements == m_nbElements)
        return;

    throw XAO_Exception(MsgBuilder() << "Invalid number of elements: " << nbElements
                                        << ", expected " << m_nbElements);
}

void Step::checkNbComponents(const int& nbComponents)
throw (XAO_Exception)
{
    if (nbComponents == m_nbComponents)
        return;

    throw XAO_Exception(MsgBuilder() << "Invalid number of components: " << nbComponents
                                        << ", expected " << m_nbComponents);
}

void Step::checkNbValues(const int& nbValues)
throw (XAO_Exception)
{
    if (nbValues == m_nbElements * m_nbComponents)
        return;

    throw XAO_Exception(MsgBuilder() << "Invalid number of values:" << nbValues
                                        << ", expected " << m_nbElements * m_nbComponents);
}
