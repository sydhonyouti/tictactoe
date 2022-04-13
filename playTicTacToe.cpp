// playTicTacToe.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below


//---------Sydney Honyouti--------------//
//---------Due Date: 3/27/2022----------//

#include "TicTacToe.h"
#include <WinSock2.h>
#include <iostream>
#include <string>

void initializeBoard(char board[10])
{
	char initBoard[10] = { '0','1','2','3','4','5','6','7','8','9' };
	for (int i = 0; i < 10; i++)
		board[i] = initBoard[i];
}

void updateBoard(char board[10], int move, int player)
{
	if (player == X_PLAYER) {
		board[move] = 'X';
	}
	else if (player == O_PLAYER) {
		board[move] = 'O';
	}
	else
		std::cout << "Problem with updateBoard function!" << std::endl;
}

void displayBoard(char board[10])
{
	std::cout << std::endl;
	std::cout << board[1] << " | " << board[2] << " | " << board[3] << std::endl;
	std::cout << "__+___+__" << std::endl;
	std::cout << board[4] << " | " << board[5] << " | " << board[6] << std::endl;
	std::cout << "__+___+__" << std::endl;
	std::cout << board[7] << " | " << board[8] << " | " << board[9] << std::endl;
	std::cout << std::endl;
}

int check4Win(char board[10])
{
	int winner = noWinner;

	// Check for vertical winners
	int i = 1;
	while (winner == noWinner && i < 4) {
		if (board[i] == board[i + 3] && board[i] == board[i + 6]) {
			winner = (board[i] == 'X') ? xWinner : oWinner;
		}
		i++;
	}

	// Check for horizontal winners
	i = 1;
	while (winner == noWinner && i < 8) {
		if (board[i] == board[i + 1] && board[i] == board[i + 2]) {
			winner = (board[i] == 'X') ? xWinner : oWinner;
		}
		i += 3;
	}

	// Check for diagonal winners
	if (winner == noWinner) {
		if ((board[1] == board[5] && board[1] == board[9]) ||
			(board[3] == board[5] && board[3] == board[7])) {
			winner = (board[5] == 'X') ? xWinner : oWinner;
		}
	}

	// Check for tie
	i = 1;
	int numMoves = 0;
	while (i < 10) {
		if ((board[i] == 'X' || board[i] == 'O')) {
			numMoves++;
		}
		i++;
	}
	if (winner == noWinner && numMoves == 9)
		winner = TIE;


	return winner;
}

int getMove(char board[10], int player)
{
	int move;
	char move_str[80];

	std::cout << "Where do you want to place your ";
	char mark = (player == X_PLAYER) ? 'X' : 'O';
	std::cout << mark << "? " << std::endl;

	do {
		std::cout << "Your move? ";
		std::cin >> move_str;
		move = atoi(move_str);
		if (board[move] == 'X' || board[move] == 'O') move = 0;
	} while (move < 1 || move > 9);

	return move;
}

int playTicTacToe(SOCKET s, std::string serverName, std::string host, std::string port, int player)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, xWinner, oWinner, TIE, ABORT
	int winner = noWinner;
	char board[10];
	int opponent;
	int move;
	bool myMove;
	const int MAXBUF = 128;
	const int MAXNAME = 80;
	char charMove[MAXNAME] = "";
	int intMove;


	if (player == X_PLAYER) {
		std::cout << "Playing as X" << std::endl;
		opponent = O_PLAYER;
		myMove = true;
	}
	else {
		std::cout << "Playing as O" << std::endl;
		opponent = X_PLAYER;
		myMove = false;
	}

	initializeBoard(board);
	displayBoard(board);

	while (winner == noWinner) {
		if (myMove) {
			// Get my move & display board
			move = getMove(board, player);
			std::cout << "Board after your move:" << std::endl;
			updateBoard(board, move, player);
			displayBoard(board);

			// Send move to opponent
/*****
	Task 1:	move is an integer that was assigned a value (from 1 to 9) in the 
			previous code segment. Add code here to convert move to a null-terminated 
			C-string and send it to your opponent. 
*****/
			//use _itoa_s() function - itoa(int value, char * str, int base) like it is used in UDP_send;
			//Using c_str() turns a string to char array

			_itoa_s(move, charMove, 2, 10);
			UDP_send(s,charMove,2, const_cast<char*>(host.c_str()), const_cast<char*>(port.c_str()));

		}
		else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
			//Get opponent's move & display board
			int status = wait(s, 20, 0);
			if (status > 0) {
/*****
	Task 2:   (i) Insert code inside this IF statement that will accept a null-terminated
				  C-string from your opponent that represents their move. Convert that
				  string to an integer and then
			 (ii) call a function that will update the game board (see above) using your
				  opponent's move, and
			(iii) call a function that will display the updated board on your screen
*****/
				status = UDP_recv(s, charMove, 2, const_cast<char*>(host.c_str()), const_cast<char*>(port.c_str()));
				intMove = atoi(charMove);
				updateBoard(board, intMove, opponent);
				displayBoard(board);

			}
			else {
				winner = ABORT;
			}
		}
		myMove = !myMove;

		if (winner == ABORT) {
			std::cout << "No response from opponent.  Aborting the game..." << std::endl;
		}
		else {
			winner = check4Win(board);
		}

		if (winner == player)
			std::cout << "You WIN!" << std::endl;
		else if (winner == TIE)
			std::cout << "It's a tie." << std::endl;
		else if (winner == opponent)
			std::cout << "I'm sorry.  You lost" << std::endl;
	}

	return winner;
}