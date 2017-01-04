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

#ifndef __XAO_DOUBLESTEP_HXX__
#define __XAO_DOUBLESTEP_HXX__

#include <vector>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Step.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif


namespace XAO
{
    /**
     * @class DoubleStep
     * Step with double values.
     */
    class XAO_EXPORT DoubleStep : public Step
    {
    public:
        /**
         * Constructor.
         * @param step the step number.
         * @param stamp the stamp of the step.
         * @param nbElements the number elements of the geometry.
         * @param nbComponents the number of components of the field.
         */
        DoubleStep(const int& step, const int& stamp, const int& nbElements, const int& nbComponents);

        virtual const XAO::Type getType() { return XAO::DOUBLE; }

        /**
         * Gets all the values of the step as a list.
         * @return a vector containing all the values of the step.
         */
        std::vector<double> getValues();

        /**
         * Gets all the values for a given element.
         * @param element the index of the element.
         * @return a vector containing all the values for the given element.
         */
        std::vector<double> getElement(const int& element) throw (XAO_Exception);

        /**
         * Gets all the values for a given component.
         * @param component the index of the component.
         * @return a vector containing all the values for the given component.
         */
        std::vector<double> getComponent(const int& component) throw (XAO_Exception);

        /**
         * Gets the value for an element and a component.
         * @param element the index of the element.
         * @param component the index of the component.
         * @return the value for the given element and component.
         */
        const double getValue(const int& element, const int& component) throw (XAO_Exception);

        /**
         * Sets all the values from a list.
         * @param values the list of values to set.
         */
        void setValues(const std::vector<double>& values) throw (XAO_Exception);

        /**
         * Sets the values for an element.
         * @param element the index of the element to set.
         * @param elements the values to set.
         */
        void setElement(const int& element, const std::vector<double>& elements) throw (XAO_Exception);

        /**
         * Sets the values for a component.
         * @param component the index of the component to set.
         * @param components the values to set.
         */
        void setComponent(const int& component, const std::vector<double>& components) throw (XAO_Exception);

        /**
         * Sets the value for an element and a component.
         * @param element the index of the element.
         * @param component the index of the component.
         * @param value the value.
         */
        void setValue(const int& element, const int& component, const double& value) throw (XAO_Exception);

        virtual const std::string getStringValue(const int& element, const int& component) throw (XAO_Exception);
        virtual void setStringValue(const int& element, const int& component, const std::string& value) throw (XAO_Exception);

    private:
        std::vector< std::vector<double> > m_values;
    };
}


#endif /* __XAO_DOUBLESTEP_HXX__ */
