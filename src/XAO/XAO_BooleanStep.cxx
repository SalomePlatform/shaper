// Copyright (C) 2013-2023  CEA/DEN, EDF R&D
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

#include "XAO_BooleanStep.hxx"
#include "XAO_XaoUtils.hxx"

using namespace XAO;

BooleanStep::BooleanStep(int step, int stamp, int nbElements, int nbComponents)
{
    m_nbElements = nbElements;
    m_nbComponents = nbComponents;
    m_step = step;
    m_stamp = stamp;

    m_values.reserve(m_nbElements);
    for (int i = 0; i < m_nbElements; ++i)
    {
        std::vector<bool> row;
        row.reserve(m_nbComponents);
        for (int j = 0; j < m_nbComponents; ++j)
            row.push_back(false);
        m_values.push_back(row);
    }
}

std::vector<bool> BooleanStep::getValues()
{
    std::vector<bool> result;
    result.reserve(m_nbElements * m_nbComponents);

    std::vector< std::vector<bool>  >::iterator it;
    for (it = m_values.begin(); it != m_values.end(); ++it)
    {
        std::vector<bool> eltValues = *it;
        result.insert(result.end(), eltValues.begin(), eltValues.end());
    }

    return result;
}

std::vector<bool> BooleanStep::getElement(int element)
{
    checkElementIndex(element);

    std::vector<bool> result(m_values[element]);
    return result;
}

std::vector<bool> BooleanStep::getComponent(int component)
{
    checkComponentIndex(component);

    std::vector<bool> result;
    result.reserve(m_nbElements);

    std::vector< std::vector<bool>  >::iterator it;
    for (it = m_values.begin(); it != m_values.end(); ++it)
    {
        std::vector<bool> eltValues = *it;
        result.push_back(eltValues[component]);
    }

    return result;
}

bool BooleanStep::getValue(int element, int component)
{
    checkElementIndex(element);
    checkComponentIndex(component);

    return m_values[element][component];
}

const std::string BooleanStep::getStringValue(int element, int component)
{
    return XaoUtils::booleanToString(getValue(element, component));
}

void BooleanStep::setValues(const std::vector<bool>& values)
{
    checkNbValues((int)values.size());

    for (int i = 0; i < m_nbElements; ++i)
    {
        for (int j = 0; j < m_nbComponents; ++j)
        {
            m_values[i][j] = values[i * m_nbComponents + j];
        }
    }
}

void BooleanStep::setElement(int element, const std::vector<bool>& elements)
{
    checkElementIndex(element);
    checkNbComponents(elements.size());

    for (int i = 0; i < m_nbComponents; ++i)
        m_values[element][i] = elements[i];
}

void BooleanStep::setComponent(int component, const std::vector<bool>& components)
{
    checkComponentIndex(component);
    checkNbElements(components.size());

    for (int i = 0; i < m_nbElements; ++i)
        m_values[i][component] = components[i];
}

void BooleanStep::setValue(int element, int component, bool value)
{
    checkElementIndex(element);
    checkComponentIndex(component);

    m_values[element][component] = value;
}

void BooleanStep::setStringValue(int element, int component, const std::string& value)
{
    setValue(element, component, XaoUtils::stringToBoolean(value));
}
