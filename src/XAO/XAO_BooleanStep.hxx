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

#ifndef __XAO_BOOLEANSTEP_HXX__
#define __XAO_BOOLEANSTEP_HXX__

#include <vector>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Step.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#pragma warning(disable:4251) // Warning dll-interface ...
#endif


namespace XAO
{
    /**
     * @class BooleanStep
     * Step with boolean values.
     */
    class XAO_EXPORT BooleanStep : public Step
    {
    public:
        /**
         * Constructor.
         * @param step the step number.
         * @param stamp the stamp of the step.
         * @param nbElements the number elements of the geometry.
         * @param nbComponents the number of components of the field.
         */
        BooleanStep(int step, int stamp, int nbElements, int nbComponents);

        virtual XAO::Type getType() { return XAO::BOOLEAN; }

        /**
         * Gets all the values in a vector by elements and by components.
         * @return a vector containing all the values.
         */
        std::vector<bool> getValues();

        /**
         * Gets all the values for an element.
         * @param element the index of the element to get.
         * @return a vector containing all the values for the given element.
         */
        std::vector<bool> getElement(int element);

        /**
         * Gets all the values for a component.
         * @param component the index of the component to get.
         * @return a vector containing all the values for the given component.
         */
        std::vector<bool> getComponent(int component);

        /**
         * Gets a value for an element and a component.
         * @param element the index of the element.
         * @param component the index of the component.
         * @return the value.
         */
        bool getValue(int element, int component);

        /**
         * Sets all the values from a list.
         * @param values the list of values to set.
         */
        void setValues(const std::vector<bool>& values);

        /**
         * Sets the values for an element.
         * @param element the index of the element to set.
         * @param elements the values to set.
         */
        void setElement(int element, const std::vector<bool>& elements);

        /**
         * Sets the values for a component.
         * @param component the index of the component to set.
         * @param components the values to set.
         */
        void setComponent(int component, const std::vector<bool>& components);

        /**
         * Sets the value for an element and a component.
         * @param element the index of the element.
         * @param component the index of the component.
         * @param value the value.
         */
        void setValue(int element, int component, bool value);

        virtual const std::string getStringValue(int element, int component);
        virtual void setStringValue(int element, int component, const std::string& value);

    private:
        std::vector< std::vector<bool> > m_values;
    };
}


#endif /* __XAO_BOOLEANSTEP_HXX__ */
