#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;

int const N = 9;
int block_size = sqrt(N);

// BITMASKS
void set_initial_masks(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);
void remove_possibility(int &cell, int num);
bool is_value_possible(int cell_mask, int num);
bool update_possibilities(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values, int row, int col); //called when specific change made
int pos_bit(int bitmask); //gets single value
int count_ones(int n); //counts 1's in binary masks

// MAIN SOLVE FUNCTIONS
bool Solve(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);
bool find_next(vector<vector<int>> &sudoku,vector<vector<int>> &possible_values, int &row, int &col);

// SECONDARY SOLVE FUNCTIONS
// fills singles
bool singles(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);
bool by_row(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);
bool by_col(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);
bool by_block(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);
void fill(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values);

//PRINTING TO TERMINAL
void output(vector<vector<int>> &sudoku);



int main () {
    vector<vector<int>> sudoku(N, vector<int> (N)); //initialise sudoku
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++){
            cin >> sudoku[i][j];
        }

    using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::milliseconds;

        auto t1 = high_resolution_clock::now();

    vector<vector<int>> possible_values(N,vector<int> (N)); //fill bitmasks
    set_initial_masks(sudoku,possible_values);
    fill(sudoku,possible_values); //solves any that can be

    if (Solve(sudoku,possible_values)){
        output(sudoku);
    }
    else
        cout << "No Solution" << endl;

    auto t2 = high_resolution_clock::now();
        auto ms = duration_cast<milliseconds>(t2 - t1);
        //cout<<ms.count()<<"ms"<<endl;

    return 0;
}

//main solve finction
bool Solve(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    int row, col;
    if (!find_next(sudoku,possible_values,row,col)) {
        return true; //solved! :)
    }

    vector<vector<int>> restore_possible = possible_values; //backups
    vector<vector<int>> restore_sudoku = sudoku;

    for (int i = 1; i < N+1; i++) { //trying values
        if (is_value_possible(possible_values[row][col],i)){
            sudoku[row][col] = i;
            possible_values[row][col] = 0;
            if (update_possibilities(sudoku,possible_values,row,col)){
                fill(sudoku,possible_values);
                if (Solve(sudoku,possible_values)) {
                    return true; //solved! :)
                }
            }

            possible_values = restore_possible; //restores backup
            sudoku = restore_sudoku;

        }
    }
    return false; // nothing worked so backtrack
}

//finds next 0 with least possibilities in sudoku and updates row col integers
bool find_next(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values, int &row, int &col) {
    int min = N + 1;
    bool found = false;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (sudoku[i][j] == 0) {
                int count = count_ones(possible_values[i][j]);
                if (count < min) {
                    row = i;
                    col = j;
                    min = count;
                    found = true;
                }
            }
        }
    }

    return found;
}

//fills any single values in the grid
bool singles(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    bool changed = false;
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (count_ones(possible_values[i][j]) == 1){
                int bitmask = possible_values[i][j];
                sudoku[i][j] = pos_bit(bitmask);
                possible_values[i][j] = 0;
                update_possibilities(sudoku,possible_values,i,j);
                changed = true;

            }
        }
    }
    return changed;
}

//fills values that are singular in each row
bool by_row(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    bool changed = false;
    for (int row = 0; row < N; row++){
        for (int num = 1; num < N+1; num++)
        {
            int count = 0;
            int enter_row, enter_col;
            for(int col = 0; col < N; col++){
                if (is_value_possible(possible_values[row][col],num)){
                    count ++;
                    enter_row = row;
                    enter_col = col;
                }
            }
            if (count == 1){
                sudoku[enter_row][enter_col] = num;
                possible_values[enter_row][enter_col] = 0;
                update_possibilities(sudoku,possible_values,enter_row,enter_col);
                changed = true;

            }
        }
    }
    return changed;
}

//fills any values that are singular in each col
bool by_col(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    bool changed = false;
    for (int col = 0; col < N; col++){
        for (int num = 1; num < N+1; num++)
        {
            int count = 0;
            int enter_row, enter_col;
            for(int row = 0; row < N; row++){
                if (is_value_possible(possible_values[row][col],num)){
                    count ++;
                    enter_row = row;
                    enter_col = col;
                }
            }
            if (count == 1){
                sudoku[enter_row][enter_col] = num;
                possible_values[enter_row][enter_col] = 0;
                update_possibilities(sudoku,possible_values,enter_row,enter_col);
                changed = true;
            }
        }
    }
    return changed;
}

//fills any values that are singular in each block
bool by_block(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    bool changed = false;
    for (int start_row = 0; start_row < N; start_row += block_size){
        for (int start_col = 0; start_col < N; start_col += block_size)
        {
            for (int num = 1; num < N+1; num++)
            {
                int count = 0;
                int enter_row, enter_col;
                for(int row = start_row; row < start_row+block_size; row++){
                    for (int col = start_col; col < start_col+block_size; col++){
                        if (is_value_possible(possible_values[row][col],num)){
                            count ++;
                            enter_row = row;
                            enter_col = col;
                        }
                    }
                }
                if (count == 1){
                    sudoku[enter_row][enter_col] = num;
                    possible_values[enter_row][enter_col] = 0;
                    update_possibilities(sudoku,possible_values,enter_row,enter_col);
                    changed = true;
                }
            }
        }
    }
    return changed;
}

//fills any values that can be determined without backtracking
void fill(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    bool changed = true;
    while (changed) {
        bool single = singles(sudoku,possible_values);
        bool block = by_block(sudoku,possible_values);
        bool row = by_row(sudoku,possible_values);
        bool col = by_col(sudoku,possible_values);
        changed = single||block||row||col;
    }
}

//gets position of single bit in mask
// (gets only possibility for a cell)
int pos_bit(int bitmask){
    int bit = 1, pos = 1;
    while(!(bit&bitmask)){
        bit = bit << 1;
        pos++;
    }
    return pos;
}

// sets initial bit masks for each cell
// clues are set to 0, others are initialised with possible values
void set_initial_masks(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (sudoku[i][j] == 0){
                possible_values[i][j] = 0b111111111; //any number is possible
            }
            else {
                possible_values[i][j] = 0;
            }
        }
    }

    //done after initialising so all values are set
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (possible_values[i][j] == 0)
                update_possibilities(sudoku,possible_values,i,j); // updates initial possibililites from initial values
}

//removes possibility from bit mask
void remove_possibility(int &cell, int num){
    cell = cell & ~(1 << (num-1));
}

//called when a change occurs
//updates possible values for all affected cells
bool update_possibilities(vector<vector<int>> &sudoku, vector<vector<int>> &possible_values, int row, int col){

    //UPDATE ROW
    for (int i = 0; i < N; i++){
        remove_possibility(possible_values[row][i],sudoku[row][col]);
        if (possible_values[row][i] == 0 && sudoku[row][i] == 0)
            return false;
    }

    //UPDATE COL
    for (int i = 0; i < N; i++) {
        remove_possibility(possible_values[i][col],sudoku[row][col]);
        if (possible_values[i][col] == 0 && sudoku[i][col] == 0)
            return false;
    }

    //UPDATE BLOCK
    int start_row = block_size;
    start_row = row - row%start_row; //starts at beginning of block
    int start_col = block_size;
    start_col = col - col%start_col;
    for (int i = start_row; i < start_row + block_size; i++) {
        for (int j = start_col; j < start_col + block_size; j++) {
            remove_possibility(possible_values[i][j],sudoku[row][col]);
            if (possible_values[row][col] == 0 && sudoku[row][col] == 0)
                return false;
        }
    }

    return true;
}

//count possible numbers for a given cell
//using bitmasks
int count_ones(int n){
    return __builtin_popcount(n);
}


//checks if value can work there using bitmask
bool is_value_possible(int cell_mask, int num){
    return cell_mask & (1 << (num-1));
}


void output(vector<vector<int>> &sudoku) {
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < N; j ++){
            if (j == N-1)
                cout << sudoku[i][j] << endl;
            else
                cout << sudoku[i][j] << " ";
        }
    }
}
