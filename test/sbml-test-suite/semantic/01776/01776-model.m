(*

category:        Test
synopsis:        Species conversion factor set with avogadro.
componentTags:   CSymbolAvogadro, Compartment, InitialAssignment, Parameter, Reaction, Species
testTags:        Amount, ConversionFactors, InitialValueReassigned
testType:        TimeCourse
levels:          3.1, 3.2
generatedBy:     Analytic
packagesPresent: 

REVOLUTION!!!!
 
In this model, a species conversion factor is set via an initial assignment with the avogadro csymbol.

The model contains:
* 2 species (S1, S2)
* 1 parameter (s1_cf)
* 1 compartment (C)

There is one reaction:

[{width:30em,margin: 1em auto}|  *Reaction*  |  *Rate*  |
| J0: S1 -> S2 | $0.01$ |]

The conversion factor for the species 'S1' is 's1_cf'

The initial conditions are as follows:

[{width:35em,margin: 1em auto}|       | *Value* | *Constant* |
| Initial amount of species S1 | $2$ | variable |
| Initial amount of species S2 | $3$ | variable |
| Initial value of parameter s1_cf | $avogadro / 1.5e24$ | constant |
| Initial volume of compartment 'C' | $1$ | constant |]

The species' initial quantities are given in terms of substance units to
make it easier to use the model in a discrete stochastic simulator, but
their symbols represent their values in concentration units where they
appear in expressions.

Note: The test data for this model was generated from an analytical
solution of the system of equations.

*)
