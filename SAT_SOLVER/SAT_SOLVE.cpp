#include<iostream>
#include<vector>
#include<algorithm>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<map>
using namespace std;

#define CROSSOVER_RATE 1            
#define MUTATION_RATE 0.007
#define LIMIT 100
#define ELITISM 0.6

int POP_SIZE;
int MAX_GEN;
int n,ncopy,m;
int no_of_elites;
int crosslen;									//used only for fixed-length crossover
vector< vector<int> > clause;
vector<int> reqd_val;
vector<bool> already_done;
vector< pair< vector<int> , vector<int> > > solve_clause;
vector< pair<int,int> > elites;
int already_satisfied;
string outputname;
FILE *outputfile;
map<int,int> m1;								//coordinate compression

struct mychromo                        						//structure of a chromosome
{
	string soln;                       
	int fitness;
	
	mychromo()
	{
		soln = "";
		fitness = 0;
	}
	
	mychromo(string s, int fit)
	{
		soln = s;
		fitness = fit;
	}
};

float random_gen()
{
long long int x = (long long int)rand();
long long int y = (long long int)RAND_MAX+1;
return (float)x/y;
}

void pre()
{
	solve_clause.resize(n);
	for(int i = 0 ; i < m ; i++)
	{
		for(int j = 0 ; j < clause[i].size() ; j++)
		{
			if(clause[i][j] < 0)
			{
				solve_clause[ abs(clause[i][j]) - 1 ].first.push_back(i);  	   //first represents the negative literal
			}
			else
			{
				solve_clause[ abs(clause[i][j]) - 1 ].second.push_back(i);	   //second represents the positive literal
			}
		}
	}
}

void mapping()
{
	int var = 1;
	for(int i = 0 ; i < n ; i++)
	{
		if(!already_done[i])
		{
			m1[i + 1] = var;
			var++;
		}
	}
	
	for(int i = 0 ; i < m ; i++)
	{
		for(int j = 0 ; j < clause[i].size() ; j++)
		{
			if(clause[i][j] < 0)
			{
				clause[i][j] = -m1[ abs(clause[i][j]) ];
			}
			else
			{
				clause[i][j] = m1[ abs(clause[i][j]) ];
			}
		}
	}
	ncopy = n;
	n = m1.size();
}

string random_pop(int chromo_len)       			//random chromosome generator
{
	string x="";
	for(int i=0;i<chromo_len;i++)
	(random_gen() < 0.5f)? x+='0' : x+='1';
	return x;
}

int calc_fitness(string &curr_soln) 				//assigns fitness to a chromosome
{
	int init_fitness = 0;
	int j;
	for(int i = 0 ; i < m ; i++)
	{
		for(j = 0 ; j < clause[i].size() ; j++)
		{
			if( (curr_soln[abs(clause[i][j])-1]=='0' && clause[i][j]<0) || 
				(curr_soln[abs(clause[i][j])-1]=='1' && clause[i][j]>0) )
				break;	
		}
		if(j != clause[i].size())
		init_fitness++;
	}
	return init_fitness;
}

void single_point_crossover(string &x, string &y)        		 //crossover function
{
	if (random_gen() < CROSSOVER_RATE)
  	{
    	int pivot = (int) (random_gen() * n);
    	
    	for(int i = pivot ; i < n ; i++)
    	swap(x[i],y[i]);				  
  	}
}

void two_point_crossover(string &x, string &y)
{
	if(random_gen() < CROSSOVER_RATE)
	{
		int pivot1 = (int) (random_gen() * n);
		int pivot2 = (int) (random_gen() * n);
		
		if(pivot1 > pivot2)
		swap(pivot1,pivot2);
		
		for(int i = pivot1 ; i <= pivot2 ; i++)
		swap(x[i],y[i]);
	}
}

void uniform_crossover(string &x, string &y)
{
	if(random_gen() < CROSSOVER_RATE)
	{
		for(int i = 0 ; i < n ; i += 2)
		swap(x[i],y[i]);
	}
}

void greedy_crossover(string &x,string &y)                     
{
	if(random_gen() < CROSSOVER_RATE)
	{
		int pre_x[n];
		int pre_y[n];
		pre_x[0] = (x[0] == '0')? solve_clause[0].first.size() : solve_clause[0].second.size();
		pre_y[0] = (y[0] == '0')? solve_clause[0].first.size() : solve_clause[0].second.size();
		for(int i = 1 ; i < n ; i++)
		{
			pre_x[i] = (x[i] == '0')? solve_clause[i].first.size() : solve_clause[i].second.size();
			pre_y[i] = (y[i] == '0')? solve_clause[i].first.size() : solve_clause[i].second.size();
			pre_x[i] += pre_x[i-1];
			pre_y[i] += pre_y[i-1];
		}
		int pivot;
		int maxm = -1;
		int temp;
		for(int i = 0 ; i < n ; i++)
		{
			temp = max( pre_x[i] + pre_y[n-1] - pre_y[i] , pre_y[i] + pre_x[n-1] - pre_x[i] );
			if(temp > maxm)
			{
				maxm = temp;
				pivot = i + 1;
			}
		}
		if(pivot != n)
		{
			for(int i = pivot ; i < n ; i++)
    		swap(x[i],y[i]);
		}
	}
}

void fixed_len_crossover(string &x, string &y)
{
	int len = n*crosslen;
	len/=100;
	if(random_gen() < CROSSOVER_RATE)
	{
		int max_fitness = - 1;
		int curr_fitness;
		int i,j;
		int pivot;
		for(i = 0 ; i < n - len +1 ; i++)
		{
			for(j = 0 ; j < len ; j++)
			{
				swap(x[i+j],y[i+j]);
			}
		
			curr_fitness = max(calc_fitness(x),calc_fitness(y));
			
			if(curr_fitness > max_fitness)
			{
				max_fitness = curr_fitness;
				pivot = i;
			}
			
			for(j = 0 ; j < len ; j++)
			{
				swap(x[i+j],y[i+j]);
			}
		}
		for(j = 0 ; j < len ; j++)
		{
			swap(x[pivot+j],y[pivot+j]);
		}
	}
}

void multiple_bit_flip_random(string &x)							//mutation function 1
{
	for(int i = 0; i < n; i++)
	{
		if(random_gen() < MUTATION_RATE)
		{
			(x[i] == '0')? x[i] = '1' : x[i] = '0';
		}
	}
}

void single_bit_flip_random(string &x)								//mutation function 2
{
	int pivot = (int) (random_gen() * n);
	
	if(random_gen() < MUTATION_RATE)
	{
		(x[pivot] == '0')? x[pivot] = '1' : x[pivot] = '0';
	}
}

void multiple_bit_flip_greedy(string &x)							//mutation function 3
{
	int max_fitness = calc_fitness(x);
	int curr_fitness;
	for(int i = 0 ; i < n ; i++)
	{
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
		curr_fitness = calc_fitness(x);
		if(curr_fitness > max_fitness)
		{
			max_fitness = curr_fitness;
		}
		else
		{
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
		}
	}
}

void single_bit_flip_greedy(string &x)							        //mutation function 4
{
	int max_fitness = calc_fitness(x);
	for(int i = 0 ; i < n ; i++)
	{
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
		if(calc_fitness(x) > max_fitness)
		{
			break;
		}
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
	}
}

void single_bit_flip_greedymax(string &x)							//mutation function 5
{
	int max_fitness = calc_fitness(x);
	int curr_fitness;
	int index = -1 ;
	for(int i = 0 ; i < n ; i++)
	{
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
		curr_fitness = calc_fitness(x);
		if(curr_fitness > max_fitness)
		{
			index = i ;
			max_fitness = curr_fitness;
		}
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
	}
	if(index != -1)
	(x[index] == '0')? x[index] = '1' : x[index] = '0';
}

void flipGA(string &x)										//mutation function 6
{
	int max_fitness = calc_fitness(x);
	int curr_fitness;
	bool repeat = true;
	while(repeat)
	{
	repeat = false;	
	for(int i = 0 ; i < n ; i++)
	{
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
		curr_fitness = calc_fitness(x);
		if(curr_fitness > max_fitness)
		{
			max_fitness = curr_fitness;
			repeat = true;
		}
		else
		{
		(x[i] == '0')? x[i] = '1' : x[i] = '0';
		}
	}
	}
}

string Roulette(int total_fitness, vector< mychromo > &gen)  					//Roulette-Wheel selector
{
	int reqd_fitness = (int)(random_gen() * total_fitness);
	
	int curr_fitness = 0;
	
	for (int i=0; i<POP_SIZE; i++)
	{
		curr_fitness += gen[i].fitness;
	
		if (curr_fitness >= reqd_fitness)
		{
			return gen[i].soln;
		}
	}
	return "";
}

int Trivial()
{
	bool alltrue;
	bool allfalse;
	int CountAllTrue = 0;
	int CountAllFalse = 0;
	for(int i = 0 ; i < m ; i++)
	{
		alltrue = allfalse = true;
		for(int j = 0 ; j < clause[i].size() ; j++)
		{
			if(clause[i][j] < 0)
			{
				alltrue = false;
			}
			else
			{
				allfalse = false;
			}
		}
		if(alltrue)
		CountAllTrue++;
		else
		if(allfalse)
		CountAllFalse++;
	}
	if(CountAllTrue == 0)
	return 0;
	else
	if(CountAllFalse == 0)
	return 1;
	else
	return -1;
}

vector< pair<int,bool> > unit_literal(vector< pair<int,bool> > &final_sol)			//Unit Literal reduction
{
	bool unit_literal = true;
	bool marked[m];
	vector< vector< pair<int,int> > > v(n);
	vector< pair<int,int> >::iterator it;
	vector< vector<bool> > marker(m);
	memset(marked,false,sizeof(marked));
	for(int i = 0 ; i < m ; i++)
	{
		for(int j = 0 ; j < clause[i].size() ; j++)
		{
			v[abs(clause[i][j])-1].push_back(make_pair(i,j));
			marker[i].push_back(false);
		}
	}
	int curr_literal = 0;
	int no_of_unmarked;
	int satisfied_clauses;
	while(unit_literal)
	{
		satisfied_clauses = 0;
		unit_literal = false;
		for(int i = 0 ; i < m ; i++)
		{
			if(!marked[i])
			{
				no_of_unmarked = 0;
				for(int j = 0 ; j < clause[i].size() ; j++)
				{
					if(!marker[i][j])
					{
					no_of_unmarked++;
					curr_literal = clause[i][j];
					}
				}
				if(no_of_unmarked == 1)
				{
					unit_literal = true;
					for(it = v[abs(curr_literal)-1].begin() ; it != v[abs(curr_literal)-1].end() ; it++)
					{
						if(clause[it->first][it->second] == curr_literal)
						marked[it->first] = true;
						else
						marker[it->first][it->second] = true;
					}
					if(curr_literal > 0)
					final_sol.push_back(make_pair(curr_literal-1,true));
					else
					final_sol.push_back(make_pair(-curr_literal-1,false));
				}
			}
			else
			satisfied_clauses++;
		}
	}
	vector< vector<int> > rep_clause(m-satisfied_clauses);
	int k = 0;
	for(int i = 0 ; i < m ; i++)
	{
		if(!marked[i])
		{
			for(int j = 0 ; j < clause[i].size(); j++)
			{
				if(!marker[i][j])
				{
					rep_clause[k].push_back(clause[i][j]);
				}
			}
			k++;
		}
	}
	already_satisfied += satisfied_clauses;
	clause.clear();
	m = rep_clause.size();
	clause.resize(m);
	k = 0;
	for(vector< vector<int> >::iterator itt = rep_clause.begin() ; itt != rep_clause.end() ; itt++)
	{
		clause[k++] = *itt;
	}
	return final_sol;
}

vector< pair<int,bool> > pure_literal(vector< pair<int,bool> > &final_sol)   		//Pure literal reduction
{
	bool pure_literal = true;
	int True[n];
	int False[n];
	bool marked[m];
	vector< vector<int> > v(n);
	memset(marked,false,sizeof(marked));
	while(pure_literal)
	{
	pure_literal = false;
	memset(True,0,sizeof(True));
	memset(False,0,sizeof(False));
	v.clear();
	for(int i = 0 ; i < m ; i++)
	{
		if(!marked[i])
		{
			for(int j = 0 ; j < clause[i].size() ; j++)
			{
				if(clause[i][j] < 0)
				False[abs(clause[i][j])-1] = 1;
				else
				True[abs(clause[i][j])-1] = 1;
				v[abs(clause[i][j])-1].push_back(i);
			}
		}
	}
	for(int i = 0 ; i < n ; i++)
	{
		if((True[i] ^ False[i]) == 1)
		{
			pure_literal = true;
			for(vector<int>::iterator it = v[i].begin() ; it != v[i].end() ; it++)
			{
				marked[*it] = true;
			}
			if(True[i])
			{
				final_sol.push_back(make_pair(i,true));
			}
			else
			{
				final_sol.push_back(make_pair(i,false));
			}
		}	
	}
	}
	vector< vector<int> > rep_clause;
	for(int i = 0 ; i < m ; i++)
	{
		if(!marked[i])
		{
			rep_clause.push_back(clause[i]);
		}
	}
	already_satisfied += ( m - rep_clause.size() );
	clause.clear();
	m = rep_clause.size();
	clause.resize(m);
	int k = 0;
	for(vector< vector<int> >::iterator itt = rep_clause.begin() ; itt != rep_clause.end() ; itt++)
	{
		clause[k++] = *itt;
	}
	return final_sol;
}


bool compare(const mychromo &first, const mychromo &second)
{
	if(first.fitness >= second.fitness)
	return true;
	else
	return false;
}

void GeneticAlgo()
{
	POP_SIZE = 500;
	if(POP_SIZE % 2 == 1)
	POP_SIZE++;
	MAX_GEN = 1e6/POP_SIZE;
	no_of_elites = ELITISM * POP_SIZE;
	if(no_of_elites % 2 == 1)
	no_of_elites++;
	vector<mychromo> old_gen(POP_SIZE);			  		  //container for the current population
	vector<mychromo> new_gen;			   			  //container for the next generation
	int curr_max = 0;
	int allmax = 0;
	int curr_gen = 0;
	int last_gen ;
	int tot_gen  = 0;
	int tot_max;
//	pre();									   //uncomment if using greedy_crossover
	while(1)
	{
	for(int i = 0 ; i < POP_SIZE ; i++)
	old_gen[i].soln=random_pop(n);
	int curr_gen = 1;							   //indicates the current generation number
	last_gen = 0;
	int total_fitness;
	curr_max = 0;					   	
	while(1)
	{
		total_fitness = 0;
		for(int i = 0 ; i < POP_SIZE ; i++)
		{
			old_gen[i].fitness = calc_fitness(old_gen[i].soln);
			elites.push_back(make_pair(old_gen[i].fitness,i));
			total_fitness += old_gen[i].fitness;
			if(old_gen[i].fitness>curr_max)
			{			
			curr_max = old_gen[i].fitness;  //mymax stores the maximum no of clauses satisfied till now.  
			printf("Current Generation -> %d, maximum no of clauses satisfied-> %d\n",curr_gen,curr_max+already_satisfied);
			last_gen = curr_gen;
			}
		}
		
		for(int i = 0 ; i < POP_SIZE ; i++)
		{
			if(old_gen[i].fitness == m)     					//all clauses satisfied
			{
				printf("\nThe formula is satisfiable\n");
				fprintf(outputfile,"The formula is satisfiable!\n");
				fprintf(outputfile,"Solution found in %d generations! The solution is-> \n",curr_gen);
				for(int j = 0 ; j < ncopy ; j++)
				{
					fprintf(outputfile,"x%d = ",j+1);
					if(!already_done[j])
					(old_gen[i].soln[ m1[j+1] - 1 ] == '1')? fprintf(outputfile,"True\n") : fprintf(outputfile,"False\n");
					else
					(reqd_val[j] == 1)? fprintf(outputfile,"True\n") : fprintf(outputfile,"False\n");
				}
				fclose(outputfile);
				return ;
			}
		}
	
		new_gen.resize(POP_SIZE);
		int counter;
		sort(elites.rbegin(),elites.rend());
		for(counter = 0 ; counter < no_of_elites ; counter++)
		new_gen[counter] = old_gen[ elites[counter].second ];
		while(counter < POP_SIZE)
		{
			string firstchild  = Roulette(total_fitness, old_gen); 		//fittest individual selection
			string secondchild = Roulette(total_fitness, old_gen);
	
			single_point_crossover(firstchild,secondchild);       		//selects a pivot and exchanges the substring from [pivot to n-1]
			
//			two_point_crossover(firstchild,secondchild);		        //selects two pivots and exchanges the substring contained within the two pivots
			
//			uniform_crossover(firstchild,secondchild);		        //swaps every alternatig bit
			
//			greedy_crossover(firstchild,secondchild);			//exchanges substring that has maximum sum of satisfying clauses
			
//			fixed_len_crossover(firstchild,secondchild);		        //slides a window of length = crosslen and exchanges that substring within the window which results in maximum fitness
			
//			multiple_bit_flip_random(firstchild);				//Flips all the bits,
//			multiple_bit_flip_random(secondchild);				//dependent on MUTATION_RATE	
			
//			single_bit_flip_random(firstchild);				//Flips a single random bit,
//			single_bit_flip_random(secondchild);				//dependent on MUTATION_RATE
		
			multiple_bit_flip_greedy(firstchild);				//Flips all those bits by which fitness increases
			multiple_bit_flip_greedy(secondchild);				//independent of MUTATION_RATE 
			
//			single_bit_flip_greedy(firstchild);				//Flips a single bit by which fitness increases
//			single_bit_flip_greedy(secondchild);				//independent of MUTATION_RATE
		
//			single_bit_flip_greedymax(firstchild);				//Flips a single bit by which fitness increases the most
//			single_bit_flip_greedymax(secondchild);				//independent of MUTATION_RATE
			
//			flipGA(firstchild);						//Flips bits from left to right iteratively till fitness can no further be increased
//			flipGA(secondchild);						//independent of MUTATION_RATE
			
			new_gen[counter++] = mychromo(firstchild,0);
			new_gen[counter++] = mychromo(secondchild,0);
			
		}
		
		for(int i = 0; i < POP_SIZE; i++)
		old_gen[i] = new_gen[i];
		
		new_gen.clear();
		elites.clear();
		allmax = max(allmax,curr_max);
		curr_gen++;
		
		if(curr_gen - last_gen > LIMIT)						//maximum generations reached
		{
			tot_gen += (curr_gen - 1);
			if(tot_gen > MAX_GEN)
			{	
			printf("\nThe formula isn't satisfiable\n");
			fprintf(outputfile,"The formula is not satisfiable!\n");
			fprintf(outputfile,"Maximum no of clauses satisfiable is -> %d",allmax+already_satisfied);
			//fclose(outputfile);		
			return ;
			}
			else
			{
			printf("\nREBUILD...\n\n");
			break;
			}
		}
	}	
    }
}

int main()
{
	srand((int)time(NULL));                       		 			//seeding the random generator
	clock_t start_t,end_t;
	char mychar,s[200];
	int temp;
	double total_time;
	FILE *readfile;
	start_t = clock();	
	string filename;								//input file (should be in DIMACS format)
    	string outputname = filename + "_result.txt";
	outputfile = fopen(outputname.c_str(),"w");	
	printf("%s\n",filename.c_str());
	readfile = fopen(filename.c_str(),"r");       					
	if(readfile == NULL)
	{
	printf("\nFile does not exist!\n");
	return 0;
	}
	while(1)
	{
		fscanf(readfile,"%c",&mychar);
		if(mychar=='p')
		{
			break;
		}
		else
		fgets(s,200,readfile);
	}
	fscanf(readfile,"%s",&s);
	fscanf(readfile,"%c",&mychar);	
						
	fscanf(readfile,"%d",&n);       						 //no of variables
	fscanf(readfile,"%d",&m);							 //no of clauses
//	fprintf(outputfile,"The no of variables are -> %d\n",n);
//	fprintf(outputfile,"The no of clauses are -> %d\n\n",m);
	already_done.resize(n,false);
	reqd_val.resize(n,-1);
	clause.resize(m);
	
	for(int i = 0 ; i < m ; i++)
	{
		while(1)
		{
			fscanf(readfile,"%d",&temp);
			if(temp)
			clause[i].push_back(temp);
			else
			break;
		}
	}
	fclose(readfile);
	bool found = false;
	int result = Trivial();						//if no such clause appears such that all of it's constituent
	if(result == 0)							//literals are true(false) , then the formula can be satisfied
	{								//by assigning all the variables to false(true).
		found = true;
		fprintf(outputfile,"The formula is satisfiable!\n");
/*		for(int j = 0 ; j < n ; j++)
		{
		fprintf(outputfile,"x%d = False\n",j+1);
		}
*/	}
	else
	if(result == 1)
	{
		found = true;
		fprintf(outputfile,"The formula is satisfiable!\n");
/*		fprintf(outputfile,"Solution Found!\n");
		for(int j = 0 ; j < n ; j++)
		{
		fprintf(outputfile,"x%d = True\n",j+1);
		}
*/	}
	if(!found)
	{
	vector< pair<int,bool> > final_sol;
	already_satisfied = 0;
	unit_literal(final_sol);              							
	printf("After unit literal reduction the no of clauses is -> %d\n\n",m);	
	pure_literal(final_sol);													//Pure Literal reduction
	printf("After pure literal reduction the no of clauses is -> %d\n\n",m);
	for(vector< pair<int,bool> >::iterator it =  final_sol.begin() ; it != final_sol.end() ; it++)
	{
		reqd_val[it->first] = (it->second == true)? 1 : 0 ;
		already_done[it->first] = true;
	} 
	mapping();
	GeneticAlgo();
	}
	clause.clear();
	reqd_val.clear();
	already_done.clear();
	m1.clear();
//	solve_clause.clear();									//uncomment if using greedy_crossover
	end_t = clock();
	total_time = (double)(end_t - start_t)/CLOCKS_PER_SEC;
//	fprintf(outputfile,"\nTime taken -> %.6lf",total_time);
	fclose(outputfile);
	return 0;
}
