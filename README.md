# GeneticSAT
An Evolutionary approach towards solving the Boolean Satisfiability Problem.

This project was created during my summer internship at IIT Guwahati
(2017).

Team Members -
1. Arunava Bhattacharjee
2. Prabal Pratap Singh Chauhan

(Under the guidance of Dr. Pinaki Mitra, Associate Professor, Department of CSE, IIT Guwahati)

It uses genetic algorithms to find solutions to SAT instances. The
clause length is not restricted to any single value, so the problem
statement need not necessary  be in k-SAT form.

Parameters used in our code -

MAX_GEN : Maximum no of generations allowed for our program to run.

ELITISM_RATE : The fraction of the total no of individuals in our
current generation that would be transferred to the next generation.

CROSSOVER_RATE : Determines the probability that two selected
individuals would perform crossover.

MUTATION_RATE : Determines the probability that the concerned individual
would undergo mutation.

POP_SIZE : The total no of individuals in any generation.

LIMIT : Specifies a threshold, if no improvement occurs for a given span
of generations (specified by LIMIT), then a new fresh random population
is generated.
