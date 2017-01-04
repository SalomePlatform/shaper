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

#ifndef __XAO_FIELD_TEST_HXX__
#define __XAO_FIELD_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

#include "../XAO_Xao.hxx"
#include "../XAO_Field.hxx"
#include "../XAO_Step.hxx"

namespace XAO
{
    class FieldTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(FieldTest);
        CPPUNIT_TEST(testBooleanField);
        CPPUNIT_TEST(testDoubleField);
        CPPUNIT_TEST(testIntegerField);
        CPPUNIT_TEST(testStringField);
        CPPUNIT_TEST(testBooleanStep);
        CPPUNIT_TEST(testIntegerStep);
        CPPUNIT_TEST(testDoubleStep);
        CPPUNIT_TEST(testStringStep);
        CPPUNIT_TEST(testBooleanStepValues);
        CPPUNIT_TEST(testIntegerStepValues);
        CPPUNIT_TEST(testDoubleStepValues);
        CPPUNIT_TEST(testStringStepValues);
        CPPUNIT_TEST(testSetComponents);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        Field* testField(XAO::Type type);
        void testBooleanField();
        void testIntegerField();
        void testDoubleField();
        void testStringField();

        void testStep(XAO::Type type, Step* step);
        void testBooleanStep();
        void testIntegerStep();
        void testDoubleStep();
        void testStringStep();

        void testBooleanStepValues();
        void testIntegerStepValues();
        void testDoubleStepValues();
        void testStringStepValues();
        void testSetComponents();
    };
}

#endif // __XAO_FIELD_TEST_HXX__
