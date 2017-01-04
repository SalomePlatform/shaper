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

#ifndef __XAO_GEOMETRICELEMENT_HXX__
#define __XAO_GEOMETRICELEMENT_HXX__

#include <string>
#include <map>

#include "XAO.hxx"
#include "XAO_Exception.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#pragma warning(disable:4251) // Warning dll-interface ...
#endif


namespace XAO
{
    /**
     * \class GeometricElement
     * Generic class to manipulate a topologic element (vertex, edge, face or solid).
     */
    class XAO_EXPORT GeometricElement
    {
    public:
        /**
         * Default constructor.
         */
        GeometricElement();
        /**
         * Constructor with name and reference.
         * \param name the name of the element.
         * \param reference the reference of the element.
         */
        GeometricElement(const std::string& name, const std::string& reference);
        /**
         * Destructor.
         */
        virtual ~GeometricElement();

        /**
         * Gets the name of the element.
         * \return the name.
         */
        const std::string getName()
        {
            return m_name;
        }
        /**
         * Sets the name of the element
         * \param name the name to set.
         */
        void setName(const std::string& name)
        {
            m_name = name;
        }

        /**
         * Checks if the element has a name.
         * @return true if the element has a name, false otherwise.
         */
        const bool hasName();

        /**
         * Gets the reference of the element.
         * \return the reference.
         */
        const std::string getReference()
        {
            return m_reference;
        }
        /**
         * Sets the reference of the element.
         * \param reference the reference to set.
         */
        void setReference(const std::string& reference)
        {
            m_reference = reference;
        }

    private:
        /** The name of the element. */
        std::string m_name;
        /** The reference of the element. */
        std::string m_reference;
    };

    /**
     * \class GeometricElementList
     * Generic class to manipulate a list of topologic element.
     */
    class XAO_EXPORT GeometricElementList
    {
    public:
        /**
         * Default constructor.
         */
        GeometricElementList();

        /**
         * Constructor with size.
         * \param nb the size to set.
         */
        GeometricElementList(const int& nb);

        /**
         * Destructor.
         */
        virtual ~GeometricElementList() {}

        /**
         * Gets the size of the list.
         * \return the size of the list.
         */
        const int getSize() const { return m_count; }

        /**
         * Sets the size of the list.
         * \param nb the size to set.
         * \warning the list will be cleared.
         */
        void setSize(const int& nb);

        /**
         * Sets the name and the reference of an element.
         * \param index the index of the element to set.
         * \param name the name to set.
         * \param reference the reference to set.
         * \throw XAO_Exception if index is bigger than the size of the list.
         */
        void setElement(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception);
        /**
         * Gets the name of an element.
         * \param index the index of the element to set.
         * \return the name of the element with the given index.
         * \throw XAO_Exception if index is bigger than the size of the list.
         */
        const std::string getName(const int& index) throw (XAO_Exception);
        /**
         * Sets the name of an element.
         * \param index the index of the element.
         * \param name the name to set.
         * \throw XAO_Exception if index is bigger than the size of the list.
         */
        void setName(const int& index, const std::string& name) throw (XAO_Exception);

        /**
         * Checks if an element has a name.
         * @param index the index of the element.
         * @return true if the element has a name, false otherwise.
         */
        const bool hasName(const int& index) throw (XAO_Exception);

        /**
         * Gets the reference of an element.
         * \param index the index of the element.
         * \return the reference of the element.
         * \throw XAO_Exception if index is bigger than the size of the list.
         */
        const std::string getReference(const int& index) throw (XAO_Exception);
        /**
         * Sets the reference of an element.
         * \param index the index of the element to set.
         * \param reference the reference to set.
         * \throw XAO_Exception if index is bigger than the size of the list.
         */
        void setReference(const int& index, const std::string& reference) throw (XAO_Exception);

        /**
         * Gets the index of an element using its reference.
         * \param reference the searched reference.
         * \return the index of the element or -1 if no element found.
         */
        const int getIndexByReference(const std::string& reference) throw (XAO_Exception);

        /**
         * Iterator on the element of the list.
         */
        typedef std::map<int, GeometricElement>::iterator iterator;

        /**
         * Gets an iterator on the first element.
         * @return an iterator on the first element.
         */
        iterator begin() { return m_elements.begin(); }

        /**
         * Gets an iterator on the last element.
         * @return an iterator on the last element.
         */
        iterator end() { return m_elements.end(); }

    private:
        void checkElementIndex(const int& index) const throw (XAO_Exception);

    private:
        int m_count;
        std::map<int, GeometricElement> m_elements;
    };
}

#endif /* __XAO_GEOMETRICELEMENT_HXX__ */
