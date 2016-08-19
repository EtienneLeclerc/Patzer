#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


typedef struct chessboard chessboard;
struct chessboard {
   char* board_array;
   chessboard** all_moves;
   unsigned int num_moves;
   char white_to_move;
   int is_in_check; //white_to_move is in check
};

void init_first_pos(char* array);
chessboard* is_legal(chessboard* board, char* move);
int square_to_digit(char file, char rank);
void print_board(chessboard* board);
void print_all_boards(chessboard* board);
void delete_board(chessboard* board);
int is_square_threatened(chessboard* board_to_check, int square_pos, int threatened_by);
int delete_if_in_check(chessboard* board_to_check);
int in_check(chessboard* board_to_check,int white_in_check);
chessboard* create_board_copy(chessboard* board_to_copy);
void addRookMoves(chessboard* board_to_check, int piece_position, char white_to_move);
void addBishopMoves(chessboard* board_to_check, int piece_position, char white_to_move);
void addQueenMoves(chessboard* board_to_check, int piece_position, char white_to_move);
void addKnightMoves(chessboard* board_to_check, int piece_position, char white_to_move);
void addPawnMoves(chessboard* board_to_check, int piece_position, char white_to_move);
void addKingMoves(chessboard* board_to_check, int piece_position, char white_to_move);
void fill_all_moves(chessboard* board);

//Initializes starting position into /array/.
void init_first_pos(char* array) {
	memcpy(array,"rnbqkbnrppppppppp",16);
	int i;
	for (i=16; i<48; i++) {
		array[i]='.';
	}
	memcpy(&array[48],"PPPPPPPPRNBQKBNR",16);
}

/* Takes a position and return the legal resultant chessboard if the chosen move is legal, 0 otherwise
Syntax: a7-b8=Q, a7-b8, or O-O or O-O-O (not yet implemented)*/
chessboard* is_legal(chessboard* board, char* move) {
	char temp[65];
	strcpy(temp,board->board_array);
	int start_square = square_to_digit(move[0],move[1]);
	int piece_moved = temp[start_square];
	if ((!islower(piece_moved)&&!board->white_to_move) || (!isupper(piece_moved)&&board->white_to_move)) {
		return 0;
	}
	int end_square = square_to_digit(move[3],move[4]);
	if (strlen(move)>5) {
		piece_moved = move[6];
	} else if (piece_moved  == 'R' || piece_moved  == 'r' || piece_moved == 'K' || piece_moved == 'k') {
		piece_moved++; //alter piece_moved to S or s if piece_moved is R/r, or L/l if piece_moved is K/k
	} else if ((piece_moved == 'P' && end_square==start_square-16) || (piece_moved == 'p' && end_square==start_square+16)) {
		piece_moved--; //alter piece_moved to O/o if piece_moved is pawn that moved up two
	} else if (piece_moved == 'P' || piece_moved == 'p') {
		piece_moved-=3; //alter piece_moved to M/m if piece_moved is pawn that moved up one from start square
	} else if ((piece_moved == 'M' && end_square==start_square-9 && temp[start_square-1]=='o') || (piece_moved == 'm' && end_square==start_square+7 && temp[start_square-1]=='O')) {
		temp[start_square-1]='.';
	} else if ((piece_moved == 'M' && end_square==start_square-7 && temp[start_square+1]=='o') || (piece_moved == 'm' && end_square==start_square+9 && temp[start_square+1]=='O')) {
		temp[start_square+1]='.';
	}
	
	if (strstr(temp,"O")!=NULL && !board->white_to_move) strstr(temp,"O")[0]='M';
	if (strstr(temp,"o")!=NULL && board->white_to_move) strstr(temp,"o")[0]='m';

	
	temp[start_square]='.';
	temp[end_square]=piece_moved;
	
//	printf("%s",temp);
	
	int i;
	for (i=0; i<board->num_moves; i++) {
		if (!strcmp(board->all_moves[i]->board_array,temp)) return board->all_moves[i];
	}
	return NULL;
}

int square_to_digit(char file, char rank) {
	return (64+file-'a'-8*(rank-'0'));
}

void print_board(chessboard* board) {
	char line_to_print[9];
	int i;
	for (i=0; i<8; i++) {
		memcpy(line_to_print, &(board->board_array)[i*8], 8);
		line_to_print[8]=0;
		printf("%s\n",line_to_print);
	}
	printf("\n");
}

void print_all_boards(chessboard* board) {
	int i;
	for (i=0; i<board->num_moves; i++) {
		print_board(board->all_moves[i]);
	}
}

void delete_board(chessboard* board) {
	free(board->all_moves);
	free(board->board_array);
	free(board);
}

//returns 1 if square threatened by color threatened_by
int is_square_threatened(chessboard* board_to_check, int square_pos, int threatened_by) {
	//check for rook/orthagonal queen captures
	int i;
		
	for (i=square_pos; i>=8 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i-=8) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'R' : 'r') || board_to_check->board_array[i] == (threatened_by ? 'S' : 's')) return 1;

	for (i=square_pos; i<56 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i+=8) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'R' : 'r') || board_to_check->board_array[i] == (threatened_by ? 'S' : 's')) return 2;
	
	for (i=square_pos; (i+1)/8==i/8 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i++) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'R' : 'r') || board_to_check->board_array[i] == (threatened_by ? 'S' : 's')) return 3;
	
	for (i=square_pos; (i-1)/8==i/8 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i--) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'R' : 'r') || board_to_check->board_array[i] == (threatened_by ? 'S' : 's')) return 4;
	
	//Checks for bishop and diagonal queen moves.
	for (i=square_pos; (i-1)/8==i/8 && i>=8 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i-=9) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'B' : 'b')) return 5;
	
	for (i=square_pos; (i+1)/8==i/8 && i>=8 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i-=7) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'B' : 'b')) return 6;

	for (i=square_pos; (i-1)/8==i/8 && i<56 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i+=7) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'B' : 'b')) return 7;
	
	for (i=square_pos; (i+1)/8==i/8 && i<56 && (board_to_check->board_array[i]=='.' || board_to_check->board_array[i]==(threatened_by ? 'k' : 'K') || board_to_check->board_array[i]==(threatened_by ? 'l' : 'L')); i+=9) {}
	if (board_to_check->board_array[i] == (threatened_by ? 'Q' : 'q') || board_to_check->board_array[i] == (threatened_by ? 'B' : 'b')) return 8;
	
	//Check for knights
	if ((square_pos-1)/8==square_pos/8 && square_pos>=16 && board_to_check->board_array[square_pos-17] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos+1)/8==square_pos/8 && square_pos>=16 && board_to_check->board_array[square_pos-15] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos-2)/8==square_pos/8 && square_pos>=8 && board_to_check->board_array[square_pos-10] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos+2)/8==square_pos/8 && square_pos>=8 && board_to_check->board_array[square_pos-6] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos-2)/8==square_pos/8 && square_pos<56 && board_to_check->board_array[square_pos+6] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos+2)/8==square_pos/8 && square_pos<56 && board_to_check->board_array[square_pos+10] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos-1)/8==square_pos/8 && square_pos<48 && board_to_check->board_array[square_pos+15] == (threatened_by ? 'N' : 'n')) return 1;
	if ((square_pos+1)/8==square_pos/8 && square_pos<48 && board_to_check->board_array[square_pos+17] == (threatened_by ? 'N' : 'n')) return 1;

	//check for kings
	if ((square_pos-1)/8==square_pos/8 && square_pos>=8 && board_to_check->board_array[square_pos-9] == (threatened_by ? 'K' : 'k')) return 1;
	if (square_pos>=8 && board_to_check->board_array[square_pos-8] == (threatened_by ? 'K' : 'k')) return 1;
	if ((square_pos+1)/8==square_pos/8 && square_pos>=8 && board_to_check->board_array[square_pos-7] == (threatened_by ? 'K' : 'k')) return 1;
	if ((square_pos-1)/8==square_pos/8 && board_to_check->board_array[square_pos-1] == (threatened_by ? 'K' : 'k')) return 1;
	if ((square_pos+1)/8==square_pos/8 && board_to_check->board_array[square_pos+1] == (threatened_by ? 'K' : 'k')) return 1;
	if ((square_pos-1)/8==square_pos/8 && square_pos<56 && board_to_check->board_array[square_pos+7] == (threatened_by ? 'K' : 'k')) return 1;
	if (square_pos<56 && board_to_check->board_array[square_pos+8] == (threatened_by ? 'K' : 'k')) return 1;
	if ((square_pos+1)/8==square_pos/8 && square_pos<56 && board_to_check->board_array[square_pos+9] == (threatened_by ? 'K' : 'k')) return 1;

	//check for pawn captures
	if (threatened_by && (square_pos-1)/8==square_pos/8 && square_pos<48 && (board_to_check->board_array[square_pos+7]=='P' || board_to_check->board_array[square_pos+7]=='M' || board_to_check->board_array[square_pos+7]=='O')) return 1;
	if (threatened_by && (square_pos+1)/8==square_pos/8 && square_pos<48 && (board_to_check->board_array[square_pos+9]=='P' || board_to_check->board_array[square_pos+9]=='M' || board_to_check->board_array[square_pos+9]=='O')) return 1;
	if (!threatened_by && (square_pos-1)/8==square_pos/8 && square_pos>=16 && (board_to_check->board_array[square_pos-9]=='p' || board_to_check->board_array[square_pos+7]=='m' || board_to_check->board_array[square_pos-9]=='o')) return 1;
	if (!threatened_by && (square_pos+1)/8==square_pos/8 && square_pos>=16 && (board_to_check->board_array[square_pos-7]=='p' || board_to_check->board_array[square_pos-7]=='m' || board_to_check->board_array[square_pos-7]=='o')) return 1;
	
	return 0;
}

int in_check(chessboard* board_to_check, int white_in_check) {
	int king_pos;
	for (king_pos=0; king_pos<65; king_pos++) {
		if (board_to_check->board_array[king_pos] == (white_in_check ? 'K' : 'k') || board_to_check->board_array[king_pos] == (white_in_check ? 'L' : 'l')) {
			break;
		}
	}
	return is_square_threatened(board_to_check,king_pos,!white_in_check);
}	

// Returns 1 if !board_to_check->white_to_move is not in check, returns 0 and frees/destroys board otherwise
int delete_if_in_check(chessboard* board_to_check) {
	int king_pos;
	for (king_pos=0; king_pos<65; king_pos++) {
		if (board_to_check->board_array[king_pos] == (board_to_check->white_to_move ? 'k' : 'K') || board_to_check->board_array[king_pos] == (board_to_check->white_to_move ? 'l' : 'L')) {
			break;
		}
	}
	if (is_square_threatened(board_to_check,king_pos,board_to_check->white_to_move)) {
//		printf("Deleted: %d\n", is_square_threatened(board_to_check,king_pos,board_to_check->white_to_move));
//		print_board(board_to_check);
		delete_board(board_to_check);
		return 0;
	}
	return 1;
}

/* Creates board copy, but with white_to_move reversed */
chessboard* create_board_copy(chessboard* board_to_copy) {
	char* temp_array = malloc(65*sizeof(char));
	strcpy(temp_array,board_to_copy->board_array);
	chessboard* temp_board = malloc(sizeof(chessboard));
	temp_board->num_moves = 0;
	temp_board->white_to_move = !board_to_copy->white_to_move;
	temp_board->board_array = temp_array;
	chessboard** temp_all_moves = malloc(sizeof(chessboard*)*120);
	temp_board->all_moves = temp_all_moves;
	temp_board->is_in_check = 0;
	
	if (strstr(temp_board->board_array,"O")!=NULL) strstr(temp_board->board_array,"O")[0]='M';

	return temp_board;
}

void addRookMoves(chessboard* board_to_check, int piece_position, char white_to_move) {	
	int i=piece_position;
	while (i>=8 && ((white_to_move && !isupper(board_to_check->board_array[i-8])) || (!white_to_move && !islower(board_to_check->board_array[i-8])))) {
		i-=8;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;			
			break;
		}				

	}

	i=piece_position;
	while (i<56 && ((white_to_move && !isupper(board_to_check->board_array[i+8])) || (!white_to_move && !islower(board_to_check->board_array[i+8])))) {
		i+=8;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;			
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while ((i/8)==((i+1)/8) && ((white_to_move && !isupper(board_to_check->board_array[i+1])) || (!white_to_move && !islower(board_to_check->board_array[i+1])))) {
		i++;		
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}
	}

	i=piece_position;
	while (i>0 && ((i-1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i-1])) || (!white_to_move && !islower(board_to_check->board_array[i-1])))) {
		i--;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'S' : 's';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}
}

void addBishopMoves(chessboard* board_to_check, int piece_position, char white_to_move) {	
	int i=piece_position;
	while (i>=9 && ((i-1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i-9])) || (!white_to_move && !islower(board_to_check->board_array[i-9])))) {
		i-=9;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while (i>=7 && ((i+1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i-7])) || (!white_to_move && !islower(board_to_check->board_array[i-7])))) {
		i-=7;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while (i<55 && ((i+1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i+9])) || (!white_to_move && !islower(board_to_check->board_array[i+9])))) {
		i+=9;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while (i<57 && i>0 && ((i-1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i+7])) || (!white_to_move && !islower(board_to_check->board_array[i+7])))) {
		i+=7;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'B' : 'b';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}
}

void addQueenMoves(chessboard* board_to_check, int piece_position, char white_to_move) {
	int i=piece_position;
	while (i>=8 && ((white_to_move && !isupper(board_to_check->board_array[i-8])) || (!white_to_move && !islower(board_to_check->board_array[i-8])))) {
		i-=8;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}

	}

	i=piece_position;
	while (i<56 && ((white_to_move && !isupper(board_to_check->board_array[i+8])) || (!white_to_move && !islower(board_to_check->board_array[i+8])))) {
		i+=8;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

			break;
		}				
	}

	i=piece_position;
	while ((i/8)==((i+1)/8) && ((white_to_move && !isupper(board_to_check->board_array[i+1])) || (!white_to_move && !islower(board_to_check->board_array[i+1])))) {
		i++;		
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

			break;
		}
	}

	i=piece_position;
	while (i<0 && ((i-1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i-1])) || (!white_to_move && !islower(board_to_check->board_array[i-1])))) {
		i--;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

			break;
		}				
	}

	i=piece_position;
	while (i>=9 && ((i-1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i-9])) || (!white_to_move && !islower(board_to_check->board_array[i-9])))) {
		i-=9;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while (i>=7 && ((i+1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i-7])) || (!white_to_move && !islower(board_to_check->board_array[i-7])))) {
		i-=7;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while (i<55 && ((i+1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i+9])) || (!white_to_move && !islower(board_to_check->board_array[i+9])))) {
		i+=9;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}

	i=piece_position;
	while (i<57 && i>0 && ((i-1)/8)==(i/8) && ((white_to_move && !isupper(board_to_check->board_array[i+7])) || (!white_to_move && !islower(board_to_check->board_array[i+7])))) {
		i+=7;
		if (board_to_check->board_array[i]=='.') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		} else {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[i] = white_to_move ? 'Q' : 'q';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			break;
		}				
	}
}

void addKnightMoves(chessboard* board_to_check, int piece_position, char white_to_move) {
	if (piece_position>=16 && (piece_position-1)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position-17])) && !(!white_to_move && islower(board_to_check->board_array[piece_position-17]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-17] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position>=16 && (piece_position+1)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position-15])) && !(!white_to_move && islower(board_to_check->board_array[piece_position-15]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-15] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position>=8 && (piece_position-2)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position-10])) && !(!white_to_move && islower(board_to_check->board_array[piece_position-10]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-10] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position>=8 && (piece_position+2)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position-6])) && !(!white_to_move && islower(board_to_check->board_array[piece_position-6]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-6] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position<56 && piece_position>1 && ((piece_position-2)/8)==(piece_position/8) && !(white_to_move && isupper(board_to_check->board_array[piece_position+6])) && !(!white_to_move && islower(board_to_check->board_array[piece_position+6]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+6] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position<56 && (piece_position+2)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position+10])) && !(!white_to_move && islower(board_to_check->board_array[piece_position+10]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+10] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position<48 && piece_position!=0 && (piece_position-1)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position+15])) && !(!white_to_move && islower(board_to_check->board_array[piece_position+15]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+15] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
	if (piece_position<48 && (piece_position+1)/8==piece_position/8 && !(white_to_move && isupper(board_to_check->board_array[piece_position+17])) && !(!white_to_move && islower(board_to_check->board_array[piece_position+17]))) {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+17] = white_to_move ? 'N' : 'n';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
	}
}

void addPawnMoves(chessboard* board_to_check, int piece_position, char white_to_move) {
	if (white_to_move) {
		if (board_to_check->board_array[piece_position-8]=='.') {
			if (piece_position<16) {
				char* possible_promotions = "QSBN";
				int i;
				for (i=0; i<4; i++) {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-8] = possible_promotions[i];
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			} else {
				board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-8] = 'M';
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
				if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				
				if (piece_position>=48 && board_to_check->board_array[piece_position-16]=='.') {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-16] = 'O';
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			}
		}
		
		if ((piece_position-1)/8==piece_position/8 && islower(board_to_check->board_array[piece_position-9])) {
			if (piece_position<16) {
				char* possible_promotions = "QSBN";
				int i;
				for (i=0; i<4; i++) {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-9] = possible_promotions[i];
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			} else {
				board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-9] = 'M';
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
				if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			}
		}
		if ((piece_position+1)/8==piece_position/8 && islower(board_to_check->board_array[piece_position-7])) {
			if (piece_position<16) {
				char* possible_promotions = "QSBN";
				int i;
				for (i=0; i<4; i++) {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-7] = possible_promotions[i];
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			} else {
				board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-7] = 'M';
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
				if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			}
		}
		//en passant
		if ((piece_position-1)/8==piece_position/8 && board_to_check->board_array[piece_position-1]=='o') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-9] = 'M';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-1] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		}
		if ((piece_position+1)/8==piece_position/8 && board_to_check->board_array[piece_position+1]=='o') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-7] = 'M';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+1] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;

		}
	}
	else {
		if (board_to_check->board_array[piece_position+8]=='.') {
			if (piece_position>=48) {
				char* possible_promotions = "qbsn";
				int i;
				for (i=0; i<4; i++) {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+8] = possible_promotions[i];
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			} else {
				board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+8] = 'm';
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
				if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				
				if (piece_position<16 && board_to_check->board_array[piece_position+16]=='.') {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+16] = 'o';
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			}
		}
		
		if ((piece_position+1)/8==piece_position/8 && isupper(board_to_check->board_array[piece_position+9])) {
			if (piece_position>=48) {
				char* possible_promotions = "qsbn";
				int i;
				for (i=0; i<4; i++) {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+9] = possible_promotions[i];
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			} else {
				board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+9] = 'm';
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
				if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			}
		}
		if ((piece_position-1)/8==piece_position/8 && isupper(board_to_check->board_array[piece_position+7])) {
			if (piece_position>=48) {
				char* possible_promotions = "qsbn";
				int i;
				for (i=0; i<4; i++) {
					board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+7] = possible_promotions[i];
					board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
					if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
				}
			} else {
				board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+7] = 'm';
				board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
				if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
			}
		}
		if ((piece_position-1)/8==piece_position/8 && board_to_check->board_array[piece_position-1]=='O') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+7] = 'm';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-1] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		}
		if ((piece_position+1)/8==piece_position/8 && board_to_check->board_array[piece_position+1]=='O') {
			board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+9] = 'm';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
			board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+1] = '.';
			if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;
		}
	}
}

void addKingMoves(chessboard* board_to_check, int piece_position, char white_to_move) {
	if (piece_position>=8 && (piece_position-1)/8==piece_position/8 && ((!isupper(board_to_check->board_array[piece_position-9]) && white_to_move) || (!islower(board_to_check->board_array[piece_position-9]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-9] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if (piece_position>=8 && ((!isupper(board_to_check->board_array[piece_position-8]) && white_to_move) || (!islower(board_to_check->board_array[piece_position-8]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-8] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if (piece_position>=8 && (piece_position+1)/8==piece_position/8 && ((!isupper(board_to_check->board_array[piece_position-7]) && white_to_move) || (!islower(board_to_check->board_array[piece_position-7]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-7] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if (piece_position>0 && (piece_position-1)/8==piece_position/8 && ((!isupper(board_to_check->board_array[piece_position-1]) && white_to_move) || (!islower(board_to_check->board_array[piece_position-1]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position-1] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if ((piece_position+1)/8==piece_position/8 && ((!isupper(board_to_check->board_array[piece_position+1]) && white_to_move) || (!islower(board_to_check->board_array[piece_position+1]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+1] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if (piece_position<56 && (piece_position-1)/8==piece_position/8 && ((!isupper(board_to_check->board_array[piece_position+7]) && white_to_move) || (!islower(board_to_check->board_array[piece_position+7]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+7] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if (piece_position<56 && ((!isupper(board_to_check->board_array[piece_position+8]) && white_to_move) || (!islower(board_to_check->board_array[piece_position+8]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+8] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
	if (piece_position<56 && (piece_position+1)/8==piece_position/8 && ((!isupper(board_to_check->board_array[piece_position+9]) && white_to_move) || (!islower(board_to_check->board_array[piece_position+9]) && !white_to_move))) {
		board_to_check->all_moves[board_to_check->num_moves] = create_board_copy(board_to_check);
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position+9] = white_to_move ? 'L' : 'l';
		board_to_check->all_moves[board_to_check->num_moves]->board_array[piece_position] = '.';
		if(delete_if_in_check(board_to_check->all_moves[board_to_check->num_moves])) board_to_check->num_moves++;	
	}
}

void fill_all_moves(chessboard* board) {
	if (board->white_to_move) {
		int i;
		for (i=0; i<64; i++) {
			if (!isupper(board->board_array[i])) continue;
			else if (board->board_array[i] == 'R' || board->board_array[i] == 'S') addRookMoves(board,i,1);
			else if (board->board_array[i] == 'N') addKnightMoves(board,i,1);
			else if (board->board_array[i] == 'B') addBishopMoves(board,i,1);
			else if (board->board_array[i] == 'Q') addQueenMoves(board,i,1);
			else if (board->board_array[i] == 'K' || board->board_array[i] == 'L') addKingMoves(board,i,1);
			else addPawnMoves(board,i,1);
		}
		
		for (i=0; i<board->num_moves; i++) {
			if (strstr(board->all_moves[i]->board_array,"o")!=NULL) strstr(board->all_moves[i]->board_array,"o")[0]='m';
		}
	} else {
		int i;
		for (i=0; i<64; i++) {
			if (!islower(board->board_array[i])) continue;
			else if (board->board_array[i] == 'r' || board->board_array[i] == 's') addRookMoves(board,i,0);
			else if (board->board_array[i] == 'n') addKnightMoves(board,i,0);
			else if (board->board_array[i] == 'b') addBishopMoves(board,i,0);
			else if (board->board_array[i] == 'q') addQueenMoves(board,i,0);
			else if (board->board_array[i] == 'k' || board->board_array[i] == 'l') addKingMoves(board,i,0);
			else addPawnMoves(board,i,0);
		}
		for (i=0; i<board->num_moves; i++) {
			if (strstr(board->all_moves[i]->board_array,"O")!=NULL) strstr(board->all_moves[i]->board_array,"O")[0]='M';
		}
	}
}

void init_test_pos(char* array) {
	int i;
	for (i=0; i<65; i++) {
		array[i]='.';
	}
	array[0]='L';
	array[2]='r';
	array[11]='r';
}

int main() {
	/*
	initialize. DONE!
	LOOP: {
		Read in user input
		Is it legal?
		If so, update current position
		Find best move
		Update current position
	*/
	
	// initial board setup
	chessboard initial_board;
	initial_board.white_to_move = 1;
	char initial_setup[65];
	init_first_pos(initial_setup);
	initial_board.board_array = initial_setup;
	initial_board.num_moves = 0;
	chessboard* all_moves[120];
	initial_board.all_moves = all_moves;
	print_board(&initial_board);
	fill_all_moves(&initial_board);

//	print_all_boards(&initial_board);
	
	chessboard* current_board = &initial_board;
	char move[7];
	
	
	while (current_board->num_moves) {
		fgets(move,7,stdin);
		if (move[strlen(move)-1]=='\n') move[strlen(move)-1]=0;
		
		if (is_legal(current_board,move)!=NULL) {
			current_board=is_legal(current_board,move);
//			printf("%d",current_board->white_to_move);
			fill_all_moves(current_board);
			print_board(current_board);
//			printf("\n%d\n\n",current_board->num_moves);
//			print_all_boards(current_board);
		}
	}
	
	if (current_board->num_moves==0 && in_check(current_board,current_board->white_to_move)) {
		printf("Checkmate, %s wins!",(current_board->white_to_move ? "black" : "white"));
	} else if (current_board->num_moves==0) {
		printf("Stalemate!");
	}
		
	return 0;
}
