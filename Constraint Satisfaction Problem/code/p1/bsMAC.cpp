#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include <set>
#include <list>
#include <cstdlib>
#include <ctime>

#define N 9
#define N_sqrt 3
using namespace std;

//Assignment 2d Matrix of int stores 1 to 9 if valid else store 0 if unassigned
class arc;

bool solve(int (&assignment)[N][N], long long int& no_backtrack);
bool backtrack(int (&assignment)[N][N], list<int> (&domain)[N][N], long long int& no_backtrack);
bool findunassigned(int (&assignment)[N][N], list<int> (&domain)[N][N], int& row, int &col);
bool checkconsistent (int var_i, int var_j, int value, int (&assignment)[N][N]);
void print(int (&assignment)[N][N], char* outputFileName =  NULL);
bool make_arc_consistent(int var_i, int var_j, int value, int (&assignment)[N][N], list<int> (&domain)[N][N], list<int> (&new_domain)[N][N]);
bool revise(arc& reviseArc, list<int> (&new_domain)[N][N]);

int main(int argc, char* argv[]){

	if (argc< 2){
		cout<<"Please specify input file and output file name"<<endl;
		return 1;
	}
	if (argc< 3){
		cout<<"Please specify output file name"<<endl;
		return 1;
	}

	ifstream input_file;
	input_file.open(argv[1],ios::in);

	if (input_file.fail()) {
		cout<<"Invalid Input file"<<endl;
		return 1;
	}

	ofstream test_output_file;
	test_output_file.open(argv[2], ios::out | ios::trunc);

	if (test_output_file.fail()) {
		cout<<"Error opening Output file"<<endl;
		return 1;
	}
	else {
		test_output_file.close();
	}

	string line;
	int problem[N][N];
	int parse;

	double max_time;
	double min_time;
	double total_time=0;
	int count=0;

	long long int max_no_backtrack;
	long long int min_no_backtrack;
	long long int total_no_backtrack=0;

	ofstream output_file;
	bool sol;

	while(input_file>>line){
		if (line.size() == N*N){
			parse = 0;
			char in_char;
			for (int i=0; i<N; i++){
				for (int j=0; j<N; j++){
					in_char = line[parse++];
					if (in_char >= '0' && in_char <= '9'){
						problem[i][j] = in_char - '0';
					}
					else{
						problem[i][j] = 0;
					}		
				}
			}

			//Print problem to cout
			cout<<"Solving"<<endl;
			print(problem);

			long long int no_backtrack =0;

			clock_t start = clock();
			sol = solve(problem, no_backtrack);			
			clock_t end = clock();

			if (sol){
				//Print solution to file
				print(problem, argv[2]);				

				//Print solution to cout
				cout<<"Solution"<<endl;
				print(problem);
			}
			else{
				//Print unsat to output
				output_file.open(argv[2], ios::out | ios::app);
				output_file<<"UNSAT"<<endl;
				output_file.close();
				
				//Print unsat to cout
				cout<<"UNSAT"<<endl;
			}
			double time = double (end-start)/ CLOCKS_PER_SEC;

			if (count == 0 || time > max_time){
				max_time = time;
			}
			total_time += time;
			if (count == 0 || time < min_time){
				min_time =  time;
			}
			cout<<"Running time: "<<time<<endl;

			if (count == 0 || no_backtrack > max_no_backtrack){
				max_no_backtrack = no_backtrack;
			}
			total_no_backtrack += no_backtrack;
			if (count == 0 || no_backtrack < min_no_backtrack){
				min_no_backtrack =  no_backtrack;
			}
			cout<<"No of backtrack: "<<no_backtrack<<endl;
			cout<<endl;

			count++;
		}
		
	}
	cout<<"-------"<<endl;
	cout<<"Maximum Running time: "<<max_time<<endl;
	cout<<"Average Running time: "<<(total_time/count)<<endl;
	cout<<"Mininum Running time: "<<min_time<<endl;
	cout<<"Total   Running time: "<<total_time<<endl;	

	cout<<"-------"<<endl;
	cout<<"Maximum No of backtrack: "<<max_no_backtrack<<endl;
	cout<<"Average No of backtrack: "<<(total_no_backtrack/count)<<endl;
	cout<<"Mininum No of backtrack: "<<min_no_backtrack<<endl;
	cout<<"Total   No of backtrack: "<<total_no_backtrack<<endl;
	return 0;
}

void print(int (&assignment)[N][N], char* outputFileName){
	if (outputFileName){
		ofstream output;
		output.open(outputFileName, ios::out | ios::app);
		for (int i=0; i<N; i++){
			for (int j=0; j<N; j++){
				if (assignment[i][j] != 0){
					output<<assignment[i][j];
				}
				else{
					output<<".";
				}
			}
		}
		output<<endl;
		output.close();
	}
	else{
		for (int i=0; i<N; i++){
			for (int j=0; j<N; j++){
				if (assignment[i][j] != 0){
					cout<<assignment[i][j];
				}
				else{
					cout<<".";
				}
			}
		}
		cout<<endl;
	}
	
}

bool solve(int (&problem)[N][N], long long int& no_backtrack){

	list<int> domain[N][N];

	for (int i=0; i<N; i++){
		for (int j=0; j<N; j++){
			for (int k=1; k<=N; k++){
				domain[i][j].push_back(k); 
			}
		}
	}
	return backtrack(problem, domain, no_backtrack);
}

bool backtrack(int (&assignment)[N][N], list<int> (&domain)[N][N], long long int& no_backtrack){
	//Assignment is complete
	int row, col;
	if (!findunassigned(assignment, domain, row, col)){
		return true;
	}

	//Order of Value- Least constraining value

	vector<pair<int, int> > leastconstraintorder;
	vector<int> conflicts(N+1, 0); // Array storing conflicts for a number


	//Column unassigned neighbours
	for (int i=0; i<N; i++){
		if (i != row && assignment[i][col] == 0){
			for (list<int>::iterator iter=domain[i][col].begin(); iter!=domain[i][col].end(); ++iter){
				if (checkconsistent(i, col, *iter, assignment)){
					conflicts[*iter]++;
				}
			}
		}
	}

	//Row unassigned neighbours
	for (int j=0; j<N; j++){
		if (j != col && assignment[row][j] == 0){
			for (list<int>::iterator iter=domain[row][j].begin(); iter!=domain[row][j].end(); ++iter){
				if (checkconsistent(row, j, *iter, assignment)){
					conflicts[*iter]++;
				}
			}
		}
	}

	//Box unassigned neighbours
	int start_i = row - row%N_sqrt;
	int end_i = start_i + N_sqrt;

	int start_j = col - col%N_sqrt;
	int end_j = start_j + N_sqrt;


	for (int i= start_i; i < end_i; i++){
		for (int j= start_j; j < end_j; j++){
			if (i != row && j != col && assignment[i][j] == 0){
				for (list<int>::iterator iter=domain[i][j].begin(); iter!=domain[i][j].end(); ++iter){
					if (checkconsistent(i, j, *iter, assignment)){
						conflicts[*iter]++;
					}
				}
			}
		}
	}

	for (list<int>::iterator it = domain[row][col].begin(); it != domain[row][col].end(); ++it){
		leastconstraintorder.push_back(make_pair(conflicts[*it], *it));
	}

	sort(leastconstraintorder.begin(), leastconstraintorder.end());
	list<int> new_domain[N][N];
	for(int i=0; i<leastconstraintorder.size(); i++){

		int value = leastconstraintorder[i].second;
		if (checkconsistent(row, col, value, assignment)){
			
			// Add to assignment
			assignment[row][col] = value;			

			if (make_arc_consistent(row, col, value, assignment, domain, new_domain)){
				if (backtrack(assignment, new_domain, no_backtrack)){
					return true;
				}
				no_backtrack++;	
				
			}

			// Remove from assignment
			assignment[row][col] = 0;
		}
	}
	return false;
}

bool findunassigned(int (&assignment)[N][N], list<int> (&domain)[N][N], int& row, int &col){
	bool found = false;
	int min_legal_values = N+1;
	int legal_values;
	for (int i = 0; i < N; i++){
		for (int j =0; j< N; j++){
			if (assignment[i][j] == 0){
				found = true;
				legal_values = 0;
				for (list<int>::iterator value_it = domain[i][j].begin(); value_it != domain[i][j].end();++value_it){
					if (checkconsistent(i, j, *value_it, assignment)){
						legal_values++;
					}
				}
				if (legal_values < min_legal_values){
					row = i;
					col = j;
					min_legal_values = legal_values;
				}
				if (legal_values == 0){
					break;
				}
			}
		}
	}
	return found;
}


bool checkconsistent (int var_i, int var_j, int value, int (&assignment)[N][N]){
	//Column contraint
	for (int i=0; i<N; i++){
		if (value == assignment[i][var_j]){
			return false;
		}
	}

	//Row contraint
	for (int j=0; j<N; j++){
		if (value == assignment[var_i][j]){
			return false;
		}
	}

	//Box contraint
	int start_i = var_i - var_i%N_sqrt;
	int start_j = var_j - var_j%N_sqrt;


	for (int m= 0; m < N_sqrt; m++){
		for (int n= 0; n < N_sqrt; n++){
			if (value == assignment[start_i + m][start_j + n]){
				return false;
			}
		}
	}
	return true;
}

class arc{
public:
	int x1;
	int y1;
	int	x2;
	int y2;

	arc(int v_x1, int v_y1, int v_x2, int v_y2) : x1(v_x1), y1(v_y1), x2(v_x2), y2(v_y2) {}
};

bool make_arc_consistent(int var_i, int var_j, int value, int (&assignment)[N][N], list<int> (&domain)[N][N], list<int> (&new_domain)[N][N]){
	for (int m=0; m<N; m++){
		for (int n=0; n<N; n++){
			new_domain[m][n] = domain[m][n];
		}
	}

	new_domain[var_i][var_j].clear();
	new_domain[var_i][var_j].push_back(value);

	list<arc> arcQueue;
	
	//Column unassigned neighbours
	for (int i=0; i<N; i++){
		if (i != var_i && assignment[i][var_j] == 0){
			arcQueue.push_back(arc(i,var_j,var_i, var_j));
		}
	}

	//Row unassigned neighbours
	for (int j=0; j<N; j++){
		if (j != var_j && assignment[var_i][j] == 0){
			arcQueue.push_back(arc(var_i,j,var_i, var_j));
		}
	}

	//Box unassigned neighbours
	int start_i = var_i - var_i%N_sqrt;
	int end_i = start_i + N_sqrt;

	int start_j = var_j - var_j%N_sqrt;
	int end_j = start_j + N_sqrt;


	for (int i= start_i; i < end_i; i++){
		for (int j= start_j; j < end_j; j++){
			if (i != var_i && j != var_j && assignment[i][j] == 0){
				arcQueue.push_back(arc(i,j,var_i, var_j));
			}
		}
	}

	while (!arcQueue.empty()){
		arc reviseArc = arcQueue.front();

		arcQueue.pop_front();

		if (revise(reviseArc, new_domain)){
			
			if (new_domain[reviseArc.x1][reviseArc.y1].empty()){
				return false;
			}
			else{
				//Column unassigned neighbours
				for (int i=0; i<N; i++){
					if (i != reviseArc.x1 && assignment[i][reviseArc.y1] == 0 && !(i == reviseArc.x2 && reviseArc.y1 == reviseArc.y2)){
						arcQueue.push_back(arc(i,reviseArc.y1,reviseArc.x1, reviseArc.y1));
					}
				}

				//Row unassigned neighbours
				for (int j=0; j<N; j++){
					if (j != reviseArc.y1 && assignment[reviseArc.x1][j] == 0 && !(reviseArc.x1 == reviseArc.x2 && j == reviseArc.y2)){
						arcQueue.push_back(arc(reviseArc.x1,j,reviseArc.x1, reviseArc.y1));
					}
				}

				//Box unassigned neighbours
				int start_i = reviseArc.x1 - reviseArc.x1%N_sqrt;
				int end_i = start_i + N_sqrt;

				int start_j = reviseArc.y1 - reviseArc.y1%N_sqrt;
				int end_j = start_j + N_sqrt;


				for (int i= start_i; i < end_i; i++){
					for (int j= start_j; j < end_j; j++){
						if (i != reviseArc.x1 && j != reviseArc.y1 && assignment[i][j] == 0  && !(i == reviseArc.x2 && j == reviseArc.y2)){
							arcQueue.push_back(arc(i,j,reviseArc.x1, reviseArc.y1));
						}
					}
				}
			}
		}
	}
	return true;
}

bool revise(arc& reviseArc, list<int> (&new_domain)[N][N]){
	bool pruned = false;
	bool found;
	for (list<int>::iterator iter1=new_domain[reviseArc.x1][reviseArc.y1].begin(); iter1!=new_domain[reviseArc.x1][reviseArc.y1].end();){
		found = false;
		for (list<int>::iterator iter2=new_domain[reviseArc.x2][reviseArc.y2].begin(); iter2!=new_domain[reviseArc.x2][reviseArc.y2].end(); ++iter2){
			if(*iter1 != *iter2){
				found = true;
				break;
			}
		}
		if (!found){
			iter1 = new_domain[reviseArc.x1][reviseArc.y1].erase(iter1);
			pruned = true;
		}
		else{
			++iter1;
		}
	}
	return pruned;
}