# PortfolioGA

This project can accomplish tasks including retrieving data and implementing genetic algorithms as shown in the menu below.



Menu:
A. Retrieve and populate S&P500 constituents.\
B. Retrieve and populate market data for S&P500 stocks and calculate return.\
C. Retrieve and populate fundamental data for S&P500 stocks.\
D. Back test For Exhibition.\
E. Probation test For Exhibition.\
F. Tune Parameters.\
G. Back test for Stats.\
H. Probation test for Stats.\
X. Exit.


A is for retrieving the list of S&P500 constituents and saving in the database.\
B is for retrieving historical daily trading data for S&P500 constituents and saving data in sqlite3 database.\
C is for retrieving fundamental data for S&P500 constituents and saving data in sqlite3 database.\
D and E are for running the application of GAs once and showing a figure for exhibition.\
F is for tuning parameters in GA; thus changing the macro definition in portfolio.h and re-running F would generate max fitness value and also a plot.\
G and H are for running the application of multiple times to give statistical results.\


For EDFGH, there is sub-menu to make users choose the fitness function to apply.

Choose Fitness Function:
1. Fitness = Return/Risk
2. Fitness = V*P*Q + V*V*A
3. Fitness = Scaled(Return/Risk) + Scaled(V*P*Q + V*V*A)
0. Return Menu



