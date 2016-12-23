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

#include "TestUtils.hxx"
#include "ImportExportTest.hxx"
#include "../XAO_XaoUtils.hxx"
#include "../XAO_Geometry.hxx"
#include "../XAO_Group.hxx"
#include "../XAO_Field.hxx"
#include "../XAO_IntegerField.hxx"
#include "../XAO_IntegerStep.hxx"

using namespace XAO;


void ImportExportTest::setUp()
{
}

void ImportExportTest::tearDown()
{
}

void ImportExportTest::cleanUp()
{
}

void ImportExportTest::testExportNoGeometry()
{
    Xao xao("me", "1.0");

    bool res = xao.exportXAO("empty.xao");
    CPPUNIT_ASSERT(res);
}

void ImportExportTest::testExportGeometry()
{
    Xao xao("me", "1.0");
    Geometry* geom = Geometry::createGeometry(XAO::BREP);
    geom->setName("mygeom");
    CPPUNIT_ASSERT_EQUAL(false, geom->isReadOnly());

    // add elements
    geom->setCountVertices(4);
    geom->setVertex(0, "v1", "1");
    geom->setVertex(1, "", "2");
    geom->setVertex(2, "v3", "3");
    geom->setVertex(3, "", "4");

    geom->setCountEdges(3);
    geom->setEdge(0, "e1", "5");
    geom->setEdge(1, "", "6");
    geom->setEdge(2, "e3", "7");

    geom->setCountFaces(2);
    geom->setFace(0, "f1", "8");
    geom->setFace(1, "", "9");

    geom->setCountSolids(1);
    geom->setSolid(0, "s1", "10");

    xao.setGeometry(geom);
    CPPUNIT_ASSERT_EQUAL(true, geom->isReadOnly());

    // groups
    Group* group = xao.addGroup(XAO::SOLID);
    group->setName("boite1");
    group->add(0);

    group = xao.addGroup(XAO::FACE);
    group->setName("faces");
    group->add(0);
    group->add(1);

    // fields
    IntegerField* field = (IntegerField*)xao.addField(XAO::INTEGER, XAO::FACE, 2, "color");
    for (int stepIndex = 0; stepIndex < 10; ++stepIndex)
    {
        IntegerStep* istep = field->addStep(stepIndex, 100*stepIndex);
        for (int eltIndex = 0; eltIndex < istep->countElements(); ++eltIndex)
        {
            for (int compIndex = 0; compIndex < istep->countComponents(); ++compIndex)
            {
                istep->setValue(eltIndex, compIndex, istep->getStamp() + eltIndex*10 + compIndex);
            }
        }
    }

    bool res = xao.exportXAO("mygeom.xao");
    CPPUNIT_ASSERT(res);

    std::string xml = xao.getXML();
    //CPPUNIT_ASSERT_EQUAL(strlen(xml) == 1007);
}

void ImportExportTest::testGeometryError()
{
    Geometry* geom = Geometry::createGeometry(XAO::BREP);
    geom->setName("mygeom");
    geom->setCountVertices(2);
    CPPUNIT_ASSERT_THROW(geom->setVertex(3, "v4", "4"), XAO_Exception);
    delete geom;
}

void ImportExportTest::testImportXao()
{
    Xao xao;
    xao.importXAO(TestUtils::getTestFilePath("test.xao"));
    checkImport(xao);
}

void ImportExportTest::testImportXaoFromText()
{
    char* txt = TestUtils::readTextFile(TestUtils::getTestFilePath("test.xao"));

    Xao xao;
    xao.setXML(txt);
    checkImport(xao);
}

void ImportExportTest::checkImport(Xao& xao)
{
    CPPUNIT_ASSERT_EQUAL(std::string("me"), xao.getAuthor());
    CPPUNIT_ASSERT_EQUAL(std::string("1.0"), xao.getVersion());

    Geometry* geom = xao.getGeometry();
    CPPUNIT_ASSERT(geom != NULL);
    CPPUNIT_ASSERT_EQUAL(std::string("mygeom"), geom->getName());

    CPPUNIT_ASSERT_EQUAL(8, geom->countVertices());
    CPPUNIT_ASSERT_EQUAL(std::string("v1"), geom->getVertexName(0));
    CPPUNIT_ASSERT_EQUAL(std::string("6"), geom->getVertexReference(0));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getVertexName(1));
    CPPUNIT_ASSERT_EQUAL(std::string("7"), geom->getVertexReference(1));
    CPPUNIT_ASSERT_EQUAL(std::string("v3"), geom->getVertexName(2));
    CPPUNIT_ASSERT_EQUAL(std::string("9"), geom->getVertexReference(2));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getVertexName(3));
    CPPUNIT_ASSERT_EQUAL(std::string("11"), geom->getVertexReference(3));
    CPPUNIT_ASSERT_EQUAL(std::string("v5"), geom->getVertexName(4));
    CPPUNIT_ASSERT_EQUAL(std::string("16"), geom->getVertexReference(4));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getVertexName(5));
    CPPUNIT_ASSERT_EQUAL(std::string("17"), geom->getVertexReference(5));
    CPPUNIT_ASSERT_EQUAL(std::string("v7"), geom->getVertexName(6));
    CPPUNIT_ASSERT_EQUAL(std::string("19"), geom->getVertexReference(6));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getVertexName(7));
    CPPUNIT_ASSERT_EQUAL(std::string("21"), geom->getVertexReference(7));

    CPPUNIT_ASSERT_EQUAL(12, geom->countEdges());
    CPPUNIT_ASSERT_EQUAL(std::string("e1"), geom->getEdgeName(0));
    CPPUNIT_ASSERT_EQUAL(std::string("5"), geom->getEdgeReference(0));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getEdgeName(1));
    CPPUNIT_ASSERT_EQUAL(std::string("8"), geom->getEdgeReference(1));
    CPPUNIT_ASSERT_EQUAL(std::string("e3"), geom->getEdgeName(2));
    CPPUNIT_ASSERT_EQUAL(std::string("10"), geom->getEdgeReference(2));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getEdgeName(3));
    CPPUNIT_ASSERT_EQUAL(std::string("12"), geom->getEdgeReference(3));
    CPPUNIT_ASSERT_EQUAL(std::string("e5"), geom->getEdgeName(4));
    CPPUNIT_ASSERT_EQUAL(std::string("15"), geom->getEdgeReference(4));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getEdgeName(5));
    CPPUNIT_ASSERT_EQUAL(std::string("18"), geom->getEdgeReference(5));
    CPPUNIT_ASSERT_EQUAL(std::string("e7"), geom->getEdgeName(6));
    CPPUNIT_ASSERT_EQUAL(std::string("20"), geom->getEdgeReference(6));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getEdgeName(7));
    CPPUNIT_ASSERT_EQUAL(std::string("22"), geom->getEdgeReference(7));
    CPPUNIT_ASSERT_EQUAL(std::string("e9"), geom->getEdgeName(8));
    CPPUNIT_ASSERT_EQUAL(std::string("25"), geom->getEdgeReference(8));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getEdgeName(9));
    CPPUNIT_ASSERT_EQUAL(std::string("26"), geom->getEdgeReference(9));
    CPPUNIT_ASSERT_EQUAL(std::string("e11"), geom->getEdgeName(10));
    CPPUNIT_ASSERT_EQUAL(std::string("29"), geom->getEdgeReference(10));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getEdgeName(11));
    CPPUNIT_ASSERT_EQUAL(std::string("30"), geom->getEdgeReference(11));

    CPPUNIT_ASSERT_EQUAL(6, geom->countFaces());
    CPPUNIT_ASSERT_EQUAL(std::string("f1"), geom->getFaceName(0));
    CPPUNIT_ASSERT_EQUAL(std::string("3"), geom->getFaceReference(0));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getFaceName(1));
    CPPUNIT_ASSERT_EQUAL(std::string("13"), geom->getFaceReference(1));
    CPPUNIT_ASSERT_EQUAL(std::string("f3"), geom->getFaceName(2));
    CPPUNIT_ASSERT_EQUAL(std::string("23"), geom->getFaceReference(2));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getFaceName(3));
    CPPUNIT_ASSERT_EQUAL(std::string("27"), geom->getFaceReference(3));
    CPPUNIT_ASSERT_EQUAL(std::string("f5"), geom->getFaceName(4));
    CPPUNIT_ASSERT_EQUAL(std::string("31"), geom->getFaceReference(4));
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->getFaceName(5));
    CPPUNIT_ASSERT_EQUAL(std::string("33"), geom->getFaceReference(5));

    CPPUNIT_ASSERT_EQUAL(1, geom->countSolids());
    CPPUNIT_ASSERT_EQUAL(std::string("s1"), geom->getSolidName(0));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), geom->getSolidReference(0));

    CPPUNIT_ASSERT_EQUAL(2, xao.countGroups());
    Group* group = xao.getGroup(0);
    CPPUNIT_ASSERT_EQUAL(1, group->count());
    CPPUNIT_ASSERT_EQUAL(std::string("boite_1"), group->getName());
    CPPUNIT_ASSERT_EQUAL(XAO::SOLID, group->getDimension());
    CPPUNIT_ASSERT_EQUAL(0, group->get(0));
    group = xao.getGroup(1);
    CPPUNIT_ASSERT_EQUAL(2, group->count());
    CPPUNIT_ASSERT_EQUAL(std::string(""), group->getName());
    CPPUNIT_ASSERT_EQUAL(XAO::FACE, group->getDimension());
    CPPUNIT_ASSERT_EQUAL(0, group->get(0));
    CPPUNIT_ASSERT_EQUAL(1, group->get(1));

    CPPUNIT_ASSERT_EQUAL(1, xao.countFields());
    Field* field= xao.getField(0);
    CPPUNIT_ASSERT_EQUAL(std::string("color"), field->getName());
    CPPUNIT_ASSERT_EQUAL(XAO::INTEGER, field->getType());
    CPPUNIT_ASSERT_EQUAL(XAO::SOLID, field->getDimension());

}
