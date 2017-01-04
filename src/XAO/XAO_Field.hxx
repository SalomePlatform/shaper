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

#ifndef __XAO_FIELD_HXX__
#define __XAO_FIELD_HXX__

#include <string>
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
    typedef std::vector<Step*>::iterator stepIterator;

    /**
     * @class Field
     * A geometrical Field.
     */
    class XAO_EXPORT Field
    {
    protected:
        /**
         * Constructor.
         * @param dimension the dimension ot the field.
         * @param nbElements the number of elements.
         * @param nbComponents the number of components.
         * @param name the name of the field.
         */
        Field(const XAO::Dimension& dimension,
              const int& nbElements, const int& nbComponents, const std::string& name);

    public:
        /**
        /**
         * Creates a Field of the given type.
         * @param type the type of the field to create.
         * @param dimension the dimension.
         * @param nbElements the number of geometrical elements.
         * @param nbComponents the number of components.
         * @name the name of the field.
         * @return the created field.
         */
        static Field* createField(const XAO::Type& type, const XAO::Dimension& dimension,
                                  const int& nbElements, const int& nbComponents,
                                  const std::string& name = std::string(""))
        throw (XAO_Exception);

        /**
         * Destructor.
         */
        virtual ~Field();

        /**
         * Gets the Type of the field.
         * @return the Type of the field.
         */
        virtual const XAO::Type getType() = 0;

        /**
         * Gets the name of the Field.
         * @return the name of the Field.
         */
        const std::string getName() const
        {
            return m_name;
        }

        /**
         * Sets the name of the Field.
         * @param name the name to set.
         */
        void setName(const std::string& name)
        {
            m_name = name;
        }

        /**
         * Gets the Dimension of the Field.
         * @return the Dimension of the Field.
         */
        const XAO::Dimension getDimension() const
        {
            return m_dimension;
        }

        /**
         * Gets the number of elements of each step.
         * @return the number of elements of each step.
         */
        const int countElements() const
        {
            return m_nbElements;
        }

        /**
         * Gets the number of components.
         * @return the number of components.
         */
        const int countComponents() const
        {
            return m_nbComponents;
        }

        /**
         * Gets the number of values for each step.
         * @return the number of values for each step.
         */
        const int countValues() const
        {
            return m_nbElements * m_nbComponents;
        }

        /**
         * Gets the number of the steps.
         * @return the number of steps.
         */
        const int countSteps() const { return m_steps.size(); }

        /**
         * Gets the name of a component.
         * @param index the index of the component to get.
         * @return the name of the component for the given index.
         */
        const std::string getComponentName(const int& index) throw (XAO_Exception);

        /**
         * Sets the name of a component.
         * @param componentIndex the index of the component to set.
         * @param name the name to set.
         */
        void setComponentName(const int& componentIndex, const std::string& name) throw (XAO_Exception);

        /**
         * Sets the name of the components.
         * @param names the names to set.
         */
        void setComponentsNames(const std::vector<std::string>& names) throw (XAO_Exception);

        /**
         * Adds a new step of the same type than the field.
         * @param number the numer of the step.
         * @return the new create step.
         */
        virtual Step* addNewStep(const int& number) throw (XAO_Exception) = 0;

        /**
         * Remove a step.
         * @param step the step to remove.
         * @return true if the step has been removed, false otherwise.
         */
        bool removeStep(Step* step);

        /**
         * Verifies if the field has a step with the given step number.
         * @param step the step number.
         * @return true if the field has a step for the given number.
         */
        bool hasStep(const int& step);

        /**
         * Returns the first step.
         * @return an iterator on the first step.
         */
        stepIterator begin() { return m_steps.begin(); }

        /**
         * Returns the last step.
         * @return an iterator on the last step.
         */
        stepIterator end() { return m_steps.end(); }

    protected:
        void checkComponent(const int& component) throw (XAO_Exception);
        void checkStepIndex(const int& step) throw (XAO_Exception);

    protected:
        /** The name of the Field. */
        std::string m_name;
        /** The dimension of the Field. */
        XAO::Dimension m_dimension;

        /** The number of components. */
        int m_nbComponents;
        /** The components of the field. */
        std::vector<std::string> m_components;
        /** The number of elements. */
        int m_nbElements;

        /** The list of steps. */
        std::vector<Step*> m_steps;
    };
}

#endif
