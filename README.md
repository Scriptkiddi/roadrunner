# libRoadRunner
[![GitHub version](https://badge.fury.io/gh/sys-bio%2Froadrunner.svg)](http://badge.fury.io/gh/sys-bio%2Froadrunner)
[![Build Status](https://dev.azure.com/TheRoadrunnerProject/roadrunner/_apis/build/status/sys-bio.roadrunner?branchName=develop)](https://dev.azure.com/TheRoadrunnerProject/roadrunner/_build/latest?definitionId=8&branchName=develop)

<table style="width:100%">
  <tr>
    <td><img alt="Read the Docs" src="https://img.shields.io/readthedocs/roadrunner"></td>
    <td><a href="https://badge.fury.io/gh/sys-bio%2Froadrunner"><img src="https://badge.fury.io/gh/sys-bio%2Froadrunner.svg" alt="GitHub version" height="18"></a></td>
  </tr>
</table> 

 <table style="width:100%">
  <tr>
    <td><img alt="Licence", src="https://img.shields.io/badge/License-Apache%202.0-yellowgreen"</td>
    <td><img alt="PyPI - Downloads", src="https://img.shields.io/pypi/dm/roadrunner"></td>
    <td><img alt="Funding", src="https://img.shields.io/badge/Funding-NIH%20(GM123032)-blue"></td>
    <td><a href="https://badge.fury.io/py/tellurium"><img src="https://badge.fury.io/py/roadrunner.svg" alt="PyPI version" height="18"></a> </td>
   </tr>
</table> 

# Summary

libroadrunner is a C/C++ library that supports simulation of SBML based models. It uses LLVM to generate extremely high performace code and is the fastest SBML based simulator currently avaialable. Its main purpose is for use as a resuable library that can be hosted by other applications, particularly on large compute clusters for doing parameter optimization where performance is critical. 

We provide C/C++, Python and Julia bindings.

Sometimes the link to the C API docs goes bad in the readthedocs. If this happens, here is a permanent link that should remain whatever happens:

# Installation
Python front end (stable):

`pip install libroadrunner`

Binaries:

Head over to the [Releases](https://github.com/sys-bio/roadrunner/releases) page to download binaries. 

Experimental front end: 

`pip install libroadrunner-experimental`

# Documentation

[Python API Documentation](http://sys-bio.github.io/roadrunner/)

[C API Documention](https://sys-bio.github.io/roadrunner/OriginalDoxygenStyleDocs/html/index.html)

# Copyright

Copyright 2013-2021

E. T. Somogyi <sup>1</sup>, J. K. Medley <sup>3</sup>, M. T. Karlsson <sup>2</sup>, M. Swat <sup>1</sup>, M. Galdzicki <sup>3</sup>, K. Choi <sup>3</sup>, W. Copeland <sup>3</sup>, L. Smith <sup>3</sup>, C. Welsh <sup>3</sup> and H. M. Sauro <sup>3</sup>

1. Biocomplexity Institute, Indiana University, Simon Hall MSB1, Bloomington, IN 47405
2. Dune Scientific, 10522 Lake City Way NE, #302 Seattle WA
3. Department of Bioengineering, University of Washington, Seattle, WA, 98195

The current (2021) developers are Lucian Smith and Ciaran Welsh.

## Introduction

libRoadRunner is a high-performance and portable simulation engine for systems and synthetic biology.

## Contributing

**IMPORTANT!** Contributors **must** follow the [contribution guidelines](https://github.com/sys-bio/roadrunner/wiki). Contibuters are responsible for complying with the guidelines, including (but not limited to) making commits to the correct branch. Maintainers are not responsible for changes made to the wrong branch. Contributors take full responsibility for ensuring that their changes get merged into the develop branch.

## libRoadRunner supports the following features:

* Time Dependent Simulation (with optional conservation law reduction) using CVODE
* Supports SBML Level 2 to 3 but currently excludes algebraic rules and delay differential equations
* Uses latest libSBML distribution
* Defaults to LLVM code generation on the backend, resulting is very fast simulation times
* Optional generation of model C code and linking at run-time
* Add plugins, distribution comes with Levenberg-Marquardt optimizer plugin
* Compute steady state
* Metabolic Control Analysis
* Frequency Domain Analysis
* Access to:
  * Eigenvalues and Eigenvectors
  * Jacobian, full and reduced
  * Structural Matrices of the stoichiometry matrix


## Availability

RoadRunner is licensed for free as an open source programmatic library for use in other 
applications and as a standalone command line driven application. Its C++ API, C API, and 
Python APIs have comprehensive documentation. On Windows, OS X, and Linux binary files can be 

downloaded from http://sourceforge.net/projects/libroadrunner/files and can be installed 
ready for use.

## Docker images
Currently we have a manylinux2014 [build](https://hub.docker.com/repository/docker/ciaranwelsh/roadrunner-manylinux2014-base) docker image. The 
base provides the environment you need to be able to build roadrunner 
yourself on manylinux2014 (centos 8).  

There are two docker tags associated with roadrunner depending on which 
version of llvm you want to build with. The options are llvm-13.x for newer 
roadrunner versions (> v2.2.0) and llvm-6.x for older.

To get the base image: 

`docker pull ciaranwelsh/roadrunner-manylinux2014-base:llvm-13.x`

and the build image:

`docker pull ciaranwelsh/roadrunner-manylinux2014-build:llvm-13.x`

Docker build scripts can be found under the `docker` directory from the roadrunner
root directory. 

We can also build roadrunner in alternative docker environments (ubuntu etc.) on request. 

## Acknowledgements

This work is funded by NIGMS grant: GM081070

Licence

Licensed under the Apache License, Version 2.0 (the License); you may not use this 
file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed 
under the License is distributed on an ÎAS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS 
OF ANY KIND, either express or implied. See the License for the specific language 
governing permissions and limitations under the License.

In plain english this means:

You CAN freely download and use this software, in whole or in part, for personal, 
company internal, or commercial purposes;

You CAN use the software in packages or distributions that you create.

You SHOULD include a copy of the license in any redistribution you may make;

You are NOT required include the source of software, or of any modifications you may 
have made to it, in any redistribution you may assemble that includes it.

YOU CANNOT: redistribute any piece of this software without proper attribution;

 
  

libRoadRunner logo

  The libroadrunner logo is an adaptation of the image originally posted to Flickr by 
  El Brujo+ at http://flickr.com/photos/11039104@N08/2954808342. It was reviewed on 
  9 August 2009 by the FlickreviewR robot and was confirmed to be licensed under the 
  terms of the cc-by-sa-2.0.
