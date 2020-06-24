(*

category:        Test
synopsis:        A replaced reaction whose ID is used in a reaction.
componentTags:   Compartment, Reaction, Species, comp:ModelDefinition, comp:ReplacedElement, comp:Submodel
testTags:        Amount, comp:SubmodelOutput
testType:        TimeCourse
levels:          3.1, 3.2
generatedBy:     Analytic
packagesPresent: comp

 In this model, a replaced submodel reaction ID is used in another reaction.

The 'flattened' version of this hierarchical model contains:
* 1 species (S1)
* 1 compartment (C)

There are 2 reactions:

[{width:30em,margin: 1em auto}|  *Reaction*  |  *Rate*  |
| J1: -> S1 | $5$ |
| A__J2: S1 -> | $J1$ |]

The initial conditions are as follows:

[{width:35em,margin: 1em auto}|       | *Value* | *Constant* |
| Initial concentration of species S1 | $3$ | variable |
| Initial volume of compartment 'C' | $1$ | constant |]

Note: The test data for this model was generated from an analytical
solution of the system of equations.

*)
