(*

category:        Test
synopsis:        Missing MathML in a delayed event assignment with assignment-time values.
componentTags:   CSymbolTime, EventWithDelay, Parameter
testTags:        EventUsesAssignmentTimeValues, NoMathML, NonConstantParameter
testType:        TimeCourse
levels:          3.2
generatedBy:     Analytic
packagesPresent: 

The model ensures that a delayed even with an assignment with assignment-time values can handle the fact that one assignment actually has no MathML, while the other one does.

The model contains:
* 2 parameters (P1, P2)

There is one event:

[{width:40em,margin: 1em auto}|  *Event*  |  *Trigger*  |  *Use values from:*  |  *Delay*  | *Event Assignments* |
| E0 | $time >= 4.5$ | Assignment time | $1$ | $P2 = time$ |
|  |  |  |  | $P1 = $ |]

The initial conditions are as follows:

[{width:35em,margin: 1em auto}|       | *Value* | *Constant* |
| Initial value of parameter P1 | $1$ | variable |
| Initial value of parameter P2 | $1$ | variable |]

Note: The test data for this model was generated from an analytical
solution of the system of equations.

*)
