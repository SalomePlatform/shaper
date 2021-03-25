// Copyright (C) 2013-2021  CEA/DEN, EDF R&D
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

#include "XAO_IntegerField.hxx"
#include "XAO_IntegerStep.hxx"
#include "XAO_XaoUtils.hxx"

using namespace XAO;

IntegerField::IntegerField(XAO::Dimension dimension, int nbElements, int nbComponents, const std::string& name)
    : Field(dimension, nbElements, nbComponents, name)
{
}

Step* IntegerField::addNewStep(int step)

{
    return addStep(step, 0);
}

IntegerStep* IntegerField::addStep(int step)

{
    return addStep(step, 0);
}

IntegerStep* IntegerField::addStep(int step, int stamp)

{
    if (hasStep(step))
        throw XAO_Exception(MsgBuilder() << "Step with number " << step << " already exists.");

    IntegerStep* bstep = new IntegerStep(step, stamp, m_nbElements, m_nbComponents);
    m_steps.push_back(bstep);
    return bstep;
}

IntegerStep* IntegerField::getStep(int index)

{
    checkStepIndex(index);
    return (IntegerStep*)m_steps[index];
}
