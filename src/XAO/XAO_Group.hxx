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
// Author : Nathalie Gore (OpenCascade), Frederic Pons (OpenCascade)

#ifndef __XAO_GROUP_HXX__
#define __XAO_GROUP_HXX__

#include <string>
#include <set>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#ifdef WIN32
#pragma warning(disable:4251) // Warning dll-interface ...
#endif
namespace XAO
{
    /**
     * \class Group
     * Class to represent a Geometrical Group.
     */
    class XAO_EXPORT Group
    {
    public:
        /**
         * Constructor.
         * @param dim the dimension of the group.
         * @param nbElements the number of geometrical elements for the dimension in the geometry.
         * @param name the name of the group.
         */
        Group(const XAO::Dimension& dim, const int& nbElements, const std::string& name = std::string(""))
        throw (XAO_Exception);

        /**
         * Destructor.
         */
        virtual ~Group();

        /**
        * Gets the name of the group.
        * \return the name of the group.
        */
       const std::string getName()
       {
           return m_name;
       }
        /**
         * Sets the name of the group.
         * \param name the name to set.
         */
        void setName(const std::string& name)
        {
            m_name = name;
        }

        /**
         * Gets the dimension of the group.
         * \return the dimension of the group.
         */
        const XAO::Dimension getDimension()
        {
            return m_dimension;
        }

        /**
         * Gets the numbers of elements in the geometry of the same type than the group.
         * \return the number of elements in the associated geometry.
         */
        const int getNbElements()
        {
            return m_nbElements;
        }

        /**
         * Gets the number of elements in the group.
         * \return the number of elements.
         */
        const int count() const
        {
            return m_elements.size();
        }

        /**
         * Gets the reference of an element.
         * \param index the index of the element.
         * \return the reference of the element.
         * \note use begin() and end() if you need to iterate.
         */
        const int get(const int& index)
        {
            checkIndex(index);
            std::set<int>::iterator it = m_elements.begin();
            std::advance(it, index);
            return (*it);
        }

        /**
         * Adds an element to the group.
         * \param value the index of the element to add.
         */
        void add(const int& value);

        /**
         * Removes an element from the group.
         * \param value the index of the element to remove.
         */
        void remove(const int& value);

        /**
         * Gets an iterator on the first element in the group.
         * @return an iterator on the first element.
         */
        std::set<int>::iterator begin() { return m_elements.begin(); }

        /**
         * Gets an iterator on the last element in the group.
         * @return an iterator on the last element.
         */
        std::set<int>::iterator end() { return m_elements.end(); }

    private:
        /**
         * Ensures that the given element is valid.
         * @param element
         * @throw XAO_Exception if element is bigger than the number of elements.
         */
        void checkIndex(const int& element)
        throw (XAO_Exception);

    private:
        /** The name of the group. */
        std::string m_name;
        /** The number of elements in the associated geometry. */
        int m_nbElements;
        /** The dimension of the group. */
        XAO::Dimension m_dimension;
        /** The number of elements in the group. */
        int m_count;
        /** The elements of the group. */
        std::set<int> m_elements;
    };
}

#endif
