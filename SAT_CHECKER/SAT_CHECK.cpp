#include<iostream>
#include<vector>
#include<math.h>
using namespace std;

int n,m;
vector< vector<int> > clause;
string mysoln;

bool isValid(string &curr_soln)
{
	int j;
	for(int i = 0 ; i < m ; i++)
	{
		for(j = 0 ; j < clause[i].size() ; j++)
		{
			if( (curr_soln[abs(clause[i][j])-1]=='0' && clause[i][j]<0) || 
				(curr_soln[abs(clause[i][j])-1]=='1' && clause[i][j]>0) )
				break;	
		}
		if(j == clause[i].size())
		{
			return false;
		}
	}
	return true;
}

int main()
{
	int temp,i;
	char temp2,mychar;
	char s[200];
	FILE *taskfile,*solnfile;
	string mytaskfile;								//input dimacs file here
	string mysolnfile;								//input soln file here

	taskfile = fopen(mytaskfile.c_str(),"r");
	if(taskfile == NULL)
	{
		printf("Taskfile not found!\n");
		return 0;
	}
	solnfile = fopen(mysolnfile.c_str(),"r");
	if(solnfile == NULL)
	{
		printf("Solution file not found!\n");
		return 0;
	}
	while(1)
	{
		fscanf(taskfile,"%c",&mychar);
		if(mychar=='p')
		{
			break;
		}
		else
		fgets(s,200,taskfile);
	}
	fscanf(taskfile,"%s",&s);
	fscanf(taskfile,"%c",&mychar);	
						
	fscanf(taskfile,"%d",&n);       						   //no of variables
	fscanf(taskfile,"%d",&m);								   //no of clauses
	clause.resize(m);
	
	for(i = 0 ; i < m ; i++)
	{
		while(1)
		{
			fscanf(taskfile,"%d",&temp);
			if(temp)
			clause[i].push_back(temp);
			else
			break;
		}
	}
	fclose(taskfile);
	fgets(s,200,solnfile);
	if(s[15] == 'n')
	{
		printf("The formula isn't satisfiable anyway!\n");
		printf("Press any key to continue!\n\n");
		scanf("%c",&temp2);
		return 0;
	}
	fgets(s,200,solnfile);
	mysoln = "";
	for(i = 0 ; i < n ; i++)
	{
		while(1)
		{
			temp2 = fgetc(solnfile);
			if(temp2 == 'T')
			{
				mysoln += '1';
				fgets(s,200,solnfile);
				break;
			}
			else
			if(temp2 == 'F')
			{
				mysoln += '0';
				fgets(s,200,solnfile);
				break;
			}
		}
	}
	fclose(solnfile);
	
	if(isValid(mysoln))
	{
		printf("Yes! The solution satisfies all the clauses!\n\n");
	}
	else
	{
		printf("No! The solution fails to satisfy all the clauses!\n\n");
		return 0;
	}
	clause.clear();
	return 0;
}
