# Sudoku
Fast and efficient Sudoku solver for 9x9, 16x16, and 25x25 grids made with C++, runs in terminal.

Input is of the form:  
7 9 0 0 0 0 0 0 3  
0 0 0 0 0 0 0 6 0  
8 0 1 0 0 4 0 0 2  
0 0 5 0 0 0 0 0 0  
3 0 0 1 0 0 0 0 0  
0 4 0 0 0 6 2 0 9  
2 0 0 0 3 0 0 0 6  
0 3 0 6 0 5 4 2 1  
0 0 0 0 0 0 0 0 0  

where 0's are empty spaces. Size will obviously be different for the different grid sizes.

To run:
  1. Open terminal in the folder containing the .cpp files.
  2. Run "g++ SudokuNxN.cpp -o SudokuNxN" where N is replaced with either 9, 16 or 25.
  3. Run "./SudokuNxN" to start the program.
