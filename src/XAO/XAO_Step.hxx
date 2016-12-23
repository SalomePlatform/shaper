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

#ifndef __XAO_STEP_HXX__
#define __XAO_STEP_HXX__

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif


namespace XAO
{
    /**
     * @class Step
     * Base class for steps.
     */
    class XAO_EXPORT Step
    {
    protected:
        /** Default constructor. */
        Step() {}

    public:
        /**
         * Destructor.
         */
        virtual ~Step() {}

        /**
         * Gets the type of the step.
         * @return
         */
        virtual const XAO::Type getType() = 0;

        /**
         * Gets the step index.
         * @return the index of the step.
         */
        const int getStep() { return m_step; }

        /**
         * Sets the number of the step.
         * @param step the index to set.
         */
        void setStep(const int& step) { m_step = step; }

        /**
         * Gets the stamp of the index.
         * @return the stamp of the index.
         */
        const int getStamp() { return m_stamp; }

        /**
         * Sets the stamp of the index.
         * @param stamp the stamp to set.
         */
        void setStamp(const int& stamp) { m_stamp = stamp; }

        /**
         * Gets the number of components of the step.
         * @return the number of components.
         */
        const int countComponents() { return m_nbComponents; }

        /**
         * Gets the number of elements for the step.
         * @return the number of elements.
         */
        const int countElements() { return m_nbElements; }

        /**
         * Gets the number of values for the step.
         * @return the number of values.
         */
        const int countValues() { return m_nbElements * m_nbComponents; }

        /**
         * Gets a value as a string.
         * @param element the index of the element.
         * @param component the index of the component.
         * @return the value as a string.
         */
        virtual const std::string getStringValue(const int& element, const int& component) = 0;

        /**
         * Sets a value as a string
         * @param element the index of the element.
         * @param component the index of the component.
         * @param value the string value.
         * @throw XAO_Exception if the value is not valid.
         */
        virtual void setStringValue(const int& element, const int& component, const std::string& value) = 0;

    protected:
        /**
         * Checks that given element index is in the range of element indexes.
         * @param element the index to check.
         */
        void checkElementIndex(const int& element) throw (XAO_Exception);
        /**
         * Checks that given component index is in the range of component indexes.
         * @param component the index to check.
         */
        void checkComponentIndex(const int& component)throw (XAO_Exception);

        /**
         * Checks that the given number of elements is correct.
         * @param nbElements the number of elements to check.
         */
        void checkNbElements(const int& nbElements)throw (XAO_Exception);

        /**
         * Checks that the given number of components is correct.
         * @param nbComponents the number of components to check.
         */
        void checkNbComponents(const int& nbComponents)throw (XAO_Exception);

        /**
         * checks that the given number of values is correct.
         * @param nbValues the number of values to check.
         */
        void checkNbValues(const int& nbValues)throw (XAO_Exception);

    protected:
        /** the index of the step. */
        int m_step;
        /** The stamp of the step. */
        int m_stamp;
        /** The number of components. */
        int m_nbComponents;
        /** The number of elements. */
        int m_nbElements;
    };
}


#endif /* __XAO_STEP_HXX__ */
