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

#ifndef __XAO_BREPGEOMETRY_TEST_HXX__
#define __XAO_BREPGEOMETRY_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class BrepGeometryTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(BrepGeometryTest);
        CPPUNIT_TEST(testGetIDs);
        CPPUNIT_TEST(testGetReferences);
        CPPUNIT_TEST(testGetNames);
        CPPUNIT_TEST(testGetEdgeVertices);
        CPPUNIT_TEST(testGetFaceEdges);
        CPPUNIT_TEST(testSolidFaces);
        CPPUNIT_TEST(testGetVertex);
        CPPUNIT_TEST(testGetEdgeLength);
        CPPUNIT_TEST(testGetFaceArea);
        CPPUNIT_TEST(testGetSolidVolume);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testGetIDs();
        void testGetReferences();
        void testGetNames();
        void testGetEdgeVertices();
        void testGetFaceEdges();
        void testSolidFaces();

        void testGetVertex();
        void testGetEdgeLength();
        void testGetFaceArea();
        void testGetSolidVolume();

        void testParse();
    };
}

#endif // __XAO_BREPGEOMETRY_TEST_HXX__
