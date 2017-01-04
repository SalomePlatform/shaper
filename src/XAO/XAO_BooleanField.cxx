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

#include "XAO_BooleanField.hxx"
#include "XAO_BooleanStep.hxx"
#include "XAO_XaoUtils.hxx"

using namespace XAO;

BooleanField::BooleanField(const XAO::Dimension& dimension,
                           const int& nbElements, const int& nbComponents, const std::string& name)
    : Field(dimension, nbElements, nbComponents, name)
{
}

Step* BooleanField::addNewStep(const int& step)
throw (XAO_Exception)
{
    return addStep(step, 0);
}

BooleanStep* BooleanField::addStep(const int& step)
throw (XAO_Exception)
{
    return addStep(step, 0);
}

BooleanStep* BooleanField::addStep(const int& step, const int& stamp)
throw (XAO_Exception)
{
    if (hasStep(step))
        throw XAO_Exception(MsgBuilder() << "Step with number " << step << " already exists.");

    BooleanStep* bstep = new BooleanStep(step, stamp, m_nbElements, m_nbComponents);
    m_steps.push_back(bstep);
    return bstep;
}

BooleanStep* BooleanField::getStep(const int& index)
throw (XAO_Exception)
{
    checkStepIndex(index);
    return (BooleanStep*)m_steps[index];
}
