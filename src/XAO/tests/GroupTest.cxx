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

#include <vector>

#include "TestUtils.hxx"
#include "GroupTest.hxx"
#include "../XAO_XaoUtils.hxx"
#include "../XAO_Xao.hxx"
#include "../XAO_Group.hxx"

using namespace XAO;

void GroupTest::setUp()
{
}

void GroupTest::tearDown()
{
}

void GroupTest::cleanUp()
{
}

void GroupTest::testGroup()
{
    Group* group = new Group(XAO::FACE, 20);

    CPPUNIT_ASSERT_EQUAL(XAO::FACE, group->getDimension());
    CPPUNIT_ASSERT_EQUAL(20, group->getNbElements());

    CPPUNIT_ASSERT_EQUAL(std::string(""), group->getName());
    group->setName("the Group");
    CPPUNIT_ASSERT_EQUAL(std::string("the Group"), group->getName());

    CPPUNIT_ASSERT_EQUAL(0, group->count());
    group->add(10);
    CPPUNIT_ASSERT_EQUAL(1, group->count());
    group->add(12);
    CPPUNIT_ASSERT_EQUAL(2, group->count());
    group->add(12);
    CPPUNIT_ASSERT_EQUAL(2, group->count());

    CPPUNIT_ASSERT_EQUAL(10, group->get(0));
    CPPUNIT_ASSERT_EQUAL(12, group->get(1));
    CPPUNIT_ASSERT_THROW(group->get(2), XAO_Exception);

    group->remove(15);
    CPPUNIT_ASSERT_EQUAL(2, group->count());
    group->remove(10);
    CPPUNIT_ASSERT_EQUAL(1, group->count());
    CPPUNIT_ASSERT_EQUAL(12, group->get(0));

    delete group;
}

void GroupTest::testGroupErrors()
{
    CPPUNIT_ASSERT_THROW(new Group(XAO::WHOLE, 20), XAO_Exception);
}
