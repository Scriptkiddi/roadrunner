"""
The LibRoadRunner SBML Simulation Engine, (c) 2009-2014 Andy Somogyi and Herbert Sauro

LibRoadRunner is an SBML JIT compiler and simulation engine with a variety of analysis
functions. LibRoadRunner is a self contained library which is designed to be integrated
into existing simulation platforms or may be used a stand alone simulation and analysis
package.
"""

import roadrunner
from roadrunner import *

__version__ = getVersionStr()

def runTests(testDir=None):
  import testing
  return testing.tester.runTester(testDir)