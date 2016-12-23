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

#ifndef __XAO_DOUBLEFIELD_HXX__
#define __XAO_DOUBLEFIELD_HXX__

#include <string>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Field.hxx"
#include "XAO_DoubleStep.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif


namespace XAO
{
    /**
     * @class DoubleField
     * Represents a field with double values.
     */
    class XAO_EXPORT DoubleField : public Field
    {
    public:
        /**
         * Constructor.
         * @param dimension the dimension of the field.
         * @param nbElements the number of elements.
         * @param nbComponents the number of components.
         * @param name the name of the field.
         */
        DoubleField(const XAO::Dimension& dimension, const int& nbElements, const int& nbComponents, const std::string& name);

        virtual const XAO::Type getType() { return XAO::DOUBLE; }

        virtual Step* addNewStep(const int& step) throw (XAO_Exception);

        /**
         * Adds a new step.
         * @param step the number of the step.
         * @return the newly created step.
         */
        DoubleStep* addStep(const int& step) throw (XAO_Exception);

        /**
         * Adds a new step.
         * @param step the number of the step.
         * @param stamp the stamp of the step.
         * @return the newly created step.
         */
        DoubleStep* addStep(const int& step, const int& stamp) throw (XAO_Exception);

        /**
         * Gets the step of given index.
         * @param index the index.
         * @return the step for the given index.
         */
        DoubleStep* getStep(const int& index) throw (XAO_Exception);
    };
}

#endif /* __XAO_DOUBLEFIELD_HXX__ */
