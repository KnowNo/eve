/******************************************************************************\
* This source file is part of the 'eve' framework.                             *
* (A linear, elegant, modular engine for rapid game development)               *
*                                                                              *
* The MIT License (MIT)                                                        *
*                                                                              *
* Copyright (c) 2013                                                           *
*                                                                              *
* Permission is hereby granted, free of charge, to any person obtaining a copy *
* of this software and associated documentation files (the "Software"), to deal*
* in the Software without restriction, including without limitation the rights *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    *
* copies of the Software, and to permit persons to whom the Software is        *
* furnished to do so, subject to the following conditions:                     *
*                                                                              *
* The above copyright notice and this permission notice shall be included in   *
* all copies or substantial portions of the Software.                          *
*                                                                              *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,*
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    *
* THE SOFTWARE.                                                                *
\******************************************************************************/

#include <UnitTest++.h>
#include <TestReporterStdout.h>
#include <cstring>

using namespace UnitTest;

int main(int argc, char** argv)
{
  if( argc > 1 )
  {
      //if first arg is "suite", we search for suite names instead of test names
    const bool bSuite = strcmp( "suite", argv[ 1 ] ) == 0;

      //walk list of all tests, add those with a name that
      //matches one of the arguments  to a new TestList
    const TestList& allTests( Test::GetTestList() );
    TestList selectedTests;
    Test* p = allTests.GetHead();
    while( p )
    {
      for( int i = 1 ; i < argc ; ++i )
        if( strcmp( bSuite ? p->m_details.suiteName
                           : p->m_details.testName, argv[ i ] ) == 0 )
          selectedTests.Add( p );
      Test* q = p;
      p = p->next;
      q->next = NULL;
    }

      //run selected test(s) only
    TestReporterStdout reporter;
    TestRunner runner( reporter );
    
    return runner.RunTestsIf( selectedTests, 0, True(), 0 );
  }
  else
  {
    return RunAllTests();
  }
}