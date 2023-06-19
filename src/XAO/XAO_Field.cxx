// Copyright (C) 2013-2023  CEA, EDF
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

#include <string>
#include <iostream>

#include "XAO_Xao.hxx"
#include "XAO_Field.hxx"
#include "XAO_BooleanField.hxx"
#include "XAO_IntegerField.hxx"
#include "XAO_DoubleField.hxx"
#include "XAO_StringField.hxx"
#include "XAO_XaoUtils.hxx"

using namespace XAO;

// -------------------------------------------------------

Field::Field(XAO::Dimension dimension,
             int nbElements, int nbComponents, const std::string& name)
  : m_name(name), m_dimension(dimension),
    m_nbComponents(nbComponents), m_components(nbComponents, ""),
    m_nbElements(nbElements)
{
}

Field::~Field()
{
  for (unsigned int i = 0; i < m_steps.size(); ++i)
    delete m_steps[i];
}

Field* Field::createField(XAO::Type type, XAO::Dimension dimension,
        int nbElements, int nbComponents, const std::string& name)
{
    if (type == XAO::BOOLEAN)
        return new BooleanField(dimension, nbElements, nbComponents, name);
    if (type == XAO::INTEGER)
        return new IntegerField(dimension, nbElements, nbComponents, name);
    if (type == XAO::DOUBLE)
        return new DoubleField(dimension, nbElements, nbComponents, name);
    if (type == XAO::STRING)
        return new StringField(dimension, nbElements, nbComponents, name);

    throw XAO_Exception(MsgBuilder() << "Bad Type: " << type);
}

const std::string Field::getComponentName(int index)
{
    checkComponent(index);
    return m_components[index];
}

void Field::setComponentName(int index, const std::string& name)
{
    checkComponent(index);
    m_components[index] = name;
}

void Field::setComponentsNames(const std::vector<std::string>& names)
{
    for (unsigned int  i = 0; i < names.size(); ++i)
    {
      if ((int)i < m_nbComponents)
            m_components[i] = names[i];
    }
}

bool Field::removeStep(Step* step)
{
    std::vector<Step*>::iterator it = m_steps.begin();
    for (; it != m_steps.end(); ++it)
    {
        Step* current = *it;
        if (step == current)
        {
            m_steps.erase(it);
            return true;
        }
    }

    return false;
}

bool Field::hasStep(int step)
{
    std::vector<Step*>::iterator it = m_steps.begin();
    for (; it != m_steps.end(); ++it)
    {
        Step* current = *it;
        if (current->getStep() == step)
            return true;
    }

    return false;
}

void Field::checkComponent(int component)
{
    if (component < m_nbComponents && component >= 0)
        return;

    throw XAO_Exception(MsgBuilder() << "Step index is out of range [0, "
                                     << m_nbComponents-1 << "]: " << component);
}

void Field::checkStepIndex(int step)
{
  if (step < (int)m_steps.size() && step >= 0)
        return;

    throw XAO_Exception(MsgBuilder() << "Step index is out of range [0, "
                                     << m_steps.size()-1 << "]: " << step);
}
