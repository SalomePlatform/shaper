// Copyright (C) 2013-2024  CEA, EDF
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

#include "XAO_StringStep.hxx"

using namespace XAO;

StringStep::StringStep(int step, int stamp, int nbElements, int nbComponents)
{
    m_nbElements = nbElements;
    m_nbComponents = nbComponents;
    m_step = step;
    m_stamp = stamp;

    m_values.reserve(m_nbElements);
    for (int i = 0; i < m_nbElements; ++i)
    {
        std::vector<std::string> row;
        row.reserve(m_nbComponents);
        for (int j = 0; j < m_nbComponents; ++j)
            row.push_back("");
        m_values.push_back(row);
    }
}

std::vector<std::string> StringStep::getValues()
{
    std::vector<std::string> result;
    result.reserve(m_nbElements * m_nbComponents);

    std::vector< std::vector<std::string>  >::iterator it;
    for (it = m_values.begin(); it != m_values.end(); ++it)
    {
        std::vector<std::string> eltValues = *it;
        result.insert(result.end(), eltValues.begin(), eltValues.end());
    }

    return result;
}

std::vector<std::string> StringStep::getElement(int element)

{
    checkElementIndex(element);

    std::vector<std::string> result(m_values[element]);
    return result;
}

std::vector<std::string> StringStep::getComponent(int component)

{
    checkComponentIndex(component);

    std::vector<std::string> result;
    result.reserve(m_nbElements);

    std::vector< std::vector<std::string>  >::iterator it;
    for (it = m_values.begin(); it != m_values.end(); ++it)
    {
        std::vector<std::string> eltValues = *it;
        result.push_back(eltValues[component]);
    }

    return result;
}

const std::string StringStep::getValue(int element, int component)

{
    checkElementIndex(element);
    checkComponentIndex(component);

    return m_values[element][component];
}

const std::string StringStep::getStringValue(int element, int component)

{
    return getValue(element, component);
}

void StringStep::setValues(const std::vector<std::string>& values)

{
    checkNbValues(values.size());

    for (int i = 0; i < m_nbElements; ++i)
    {
        for (int j = 0; j < m_nbComponents; ++j)
        {
            m_values[i][j] = values[i * m_nbComponents + j];
        }
    }
}

void StringStep::setElement(int element, const std::vector<std::string>& elements)

{
    checkElementIndex(element);
    checkNbComponents(elements.size());

    for (int i = 0; i < m_nbComponents; ++i)
        m_values[element][i] = elements[i];
}

void StringStep::setComponent(int component, const std::vector<std::string>& components)

{
    checkElementIndex(component);
    checkNbElements(components.size());

    for (int i = 0; i < m_nbElements; ++i)
        m_values[i][component] = components[i];
}

void StringStep::setValue(int element, int component, const std::string& value)

{
    checkElementIndex(element);
    checkComponentIndex(component);

    m_values[element][component] = value;
}

void StringStep::setStringValue(int element, int component, const std::string& value)

{
    setValue(element, component, value);
}
