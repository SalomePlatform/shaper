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

#ifndef __XAOMAINTEST_HXX__
#define __XAOMAINTEST_HXX__

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestRunner.h>
#include <stdexcept>

#include <cstring>
#include <iostream>
#include <fstream>

struct Arguments
{
public:
    bool List;
    bool Err;
    std::string Test;
};

Arguments parseArguments(int argc, char* argv[])
{
    Arguments res;
    res.List = false;
    res.Err = false;
    res.Test = "";

    if (argc > 1)
    {
        int i = 0;
        while (++i < argc) // skip 0
        {
            if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0)
            {
                res.List = true;
            }
            else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--err") == 0)
            {
                res.Err = true;
            }
            else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0)
            {
                if (i+1 < argc)
                    res.Test = argv[++i];
            }
        }
    }

    return res;
}

void listTests(CPPUNIT_NS::Test* test)
{
    if (!test) return;
    std::cout << test->getName() << std::endl;
    for (int i = 0; i < test->getChildTestCount(); ++i)
    {
        listTests(test->getChildTestAt(i));
    }
}

// ============================================================================
/*!
 *  Main program source for Unit Tests with cppunit package does not depend
 *  on actual tests, so we use the same for all partial unit tests.
 */
// ============================================================================
int main(int argc, char* argv[])
{
    Arguments args = parseArguments(argc, argv);

    // --- Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // ---  Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // ---  Add a listener that print dots as test run.
#ifdef WIN32
    CPPUNIT_NS::TextTestProgressListener progress;
#else
    CPPUNIT_NS::BriefTestProgressListener progress;
#endif
    controller.addListener(&progress);

    // ---  Get the top level suite from the registry

    CPPUNIT_NS::Test *suite =
            CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

    // ---  Adds the test to the list of test to run

    // list tests and exit
    if (args.List)
    {
        listTests(suite);
        return 0;
    }

    if (!args.Test.empty())
    {
        // find test by name
        // an exception is raised if not found
        suite = suite->findTest(args.Test);
    }

    CPPUNIT_NS::TestRunner runner;
    runner.addTest(suite);
    runner.run(controller);

    // ---  Print test in a compiler compatible format.

    if (args.Err)
    {
        CPPUNIT_NS::CompilerOutputter outputter(&result, std::cerr);
        outputter.write();
    }
    else
    {
        std::ofstream testFile;
        testFile.open("UnitTestsResult", std::ios::out | std::ios::trunc);
        CPPUNIT_NS::CompilerOutputter outputter(&result, testFile);
        outputter.write();
        testFile.close();
    }

    // ---  Run the tests.

    bool wasSucessful = result.wasSuccessful();

    // ---  Return error code 1 if the one of test failed.

    return wasSucessful ? 0 : 1;
}

#endif
