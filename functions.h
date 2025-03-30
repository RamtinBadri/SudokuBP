#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "clui.h"
using namespace std;

struct SudokuTable{
    int rows[9][9];
    int cols[9][9];
    int subgrid[9][9];
    int const_cells_coordinates[30][2] = {
        {1,3},{1,5},{1,9},{2,1},{2,3},{2,4},{2,8},
        {3,2},{3,6},{3,7},{3,9},{4,5},{4,7},{4,8},
        {5,4},{5,5},{5,8},{6,3},{6,5},{6,6},{6,9},
        {7,1},{7,5},{7,9},{8,4},{8,8},{8,9},{9,2},
        {9,4},{9,7}
    };
}table;

struct PlayerData {
    string username;
    int total_correct_moves;
    int total_wrong_moves;
    int won_games;
    int lost_games;
    int total_score;
    time_t total_time;
};

struct GameData {
    string game_name;
    SudokuTable table;
    int correct_moves;
    int wrong_moves;
    int chances;
    time_t time_deadline;
    time_t time_record;
};

struct Player {
    PlayerData player_data;
    Player* next;
};

enum Difficulty{
    Easy, Medium, Hard
};

//Global variables
GameData games[100];
PlayerData players[100];
int last_rows[9][9];
int chances = 5;
int correct_moves;
int wrong_moves;
int won_games;
int lost_games;
int total_correct_moves;
int total_wrong_moves;
int number_of_players;
int number_of_saved_games;
bool saved_game;
time_t saved_time;
time_t origin_time;
time_t paused_time;
time_t time_record;
time_t total_time;

void display_timer(time_t time_difference) {
    int minutes = time_difference / 60;
    int seconds = time_difference % 60;
    cout << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
}

void box_maker(int row, int col, int width, int height){
    cursor_to_pos(row, col);
    for(int i=0; i<width; i++){
        cout<< "-";
    }
    for(int j=1; j<height-1; j++){
        cursor_to_pos(row+j, col);
        cout << "|";
    }
    for(int j=1; j<height-1; j++){
        cursor_to_pos(row+j, col+width-1);
        cout << "|";
    }
    cursor_to_pos(row+height-1, col);
    for(int i=0; i<width; i++){
        cout<< "-";
    }
}

void swap_players(PlayerData& player1, PlayerData& player2){    //This function is useful for sorting the leaderboard.
    PlayerData temp = player1;
    player1 = player2;
    player2 = temp;
}

void store_player_data(string username, int total_correct_moves, int total_wrong_moves, int won_games, int lost_games, time_t total_time){
    ofstream outFile("leaderboard.txt", ios::app);
    int total_score = total_correct_moves - total_wrong_moves;
    outFile << username << endl << total_correct_moves << endl << total_wrong_moves << endl << won_games << endl << lost_games << endl << total_score << endl << total_time << endl;
    outFile.close();
}

int leaderboard_reader(){     //This function returns the number of players read from the file after it's done reading the data.
    ifstream inFile("leaderboard.txt");
    string line;
    int i=0;
    while(getline(inFile,line)){
        if(line.empty()){
            continue;
        }
        players[i].username = line;
        inFile >> players[i].total_correct_moves >> players[i].total_wrong_moves >> players[i].won_games >> players[i].lost_games >> players[i].total_score >> players[i].total_time;
        i++;
    }
    inFile.close();
    return i;
}

void leaderboard_sorter(int number_of_players){   //This function uses the bubble sort method.
    for(int i=0; i<number_of_players-1; i++){
        for(int j=0; j<number_of_players-1-i; j++){
            if(players[j+1].total_score > players[j].total_score){
                swap_players(players[j+1],players[j]);
            }
            else if(players[j+1].total_score == players[j].total_score){
                if(players[j+1].total_time < players[j].total_time){
                    swap_players(players[j+1],players[j]);
                }
            }
        }
    }
}

//init, add and remove functions for setting and displaying the linked list of leaderboard
void init(Player* first, PlayerData player_data){
    first->player_data = player_data;
    first->next = NULL;
}
void add(Player* player, PlayerData player_data){
    while(player->next != NULL){
        player = player->next;
    }
    Player* newPlayer = new Player;
    newPlayer->player_data = player_data;
    newPlayer->next = NULL;
    player->next = newPlayer;
}
void remove_list(Player* player){
    if(player == NULL){
        return;
    }
    remove_list(player->next);
    delete player;
}

Player* leaderboard_setter(){
    Player* leaderboard = new Player;
    init(leaderboard, players[0]);
    int i=1;
    while(i<number_of_players){
        add(leaderboard, players[i]);
        i++;
    }
    return leaderboard;
}

void display_leaderboard(Player* player){
    int i=1;
    cursor_to_pos(2,(get_window_cols()/2)-3);
    change_color_rgb(219, 242, 7);
    cout << "Leaderboard";
    while(player != NULL){
        change_color_rgb(66, 245, 242);
        box_maker(3*i,1,150,4);
        switch(i){
            case 1: change_color_rgb(237, 220, 26); break;
            case 2: change_color_rgb(173, 173, 172); break;
            case 3: change_color_rgb(204, 148, 84); break;
            default: reset_color();
        }
        cursor_to_pos(3*i+1,2);
        cout << i << ". " << player->player_data.username;
        cursor_to_pos(3*i+2,2);
        cout << "Total correct moves: " << player->player_data.total_correct_moves;
        cursor_to_pos(3*i+2,27);
        cout << "Total wrong moves: " << player->player_data.total_wrong_moves;
        cursor_to_pos(3*i+2,52);
        cout << "Number of games won: " << player->player_data.won_games;
        cursor_to_pos(3*i+2,77);
        cout << "Number of games lost: " << player->player_data.lost_games;
        cursor_to_pos(3*i+2,107);
        cout << "Total score: " << player->player_data.total_score;
        cursor_to_pos(3*i+2,127);
        cout << "Total time: ";
        display_timer(player->player_data.total_time);
        player = player->next;
        i++;
    }
    char button;
    do{
        cursor_to_pos(2,33);
        button = getch();
    }while(button != 'm');
    clear_screen();
}

bool is_name_taken(string line){
    for(int i=0; i<number_of_saved_games; i++){
        if(games[i].game_name == line){
            return true;
        }
    }
    return false;
}

void save_game(string game_name, SudokuTable table, int correct_moves, int wrong_moves, int chances, time_t time_record, time_t time_deadline){
    games[number_of_saved_games].game_name = game_name;
    games[number_of_saved_games].table = table;
    games[number_of_saved_games].correct_moves = correct_moves;
    games[number_of_saved_games].wrong_moves = wrong_moves;
    games[number_of_saved_games].chances = chances;
    games[number_of_saved_games].time_record = time_record;
    games[number_of_saved_games].time_deadline = time_deadline;
    number_of_saved_games++;
}

void show_saved_games(){
    if(number_of_saved_games == 0){
        cursor_to_pos(get_window_rows()/2,(get_window_cols()/2)-2);
        change_color_rgb(66, 245, 242);
        cout << "There are currently no saved games!";
        delay(2000);
        clear_screen();
        return;
    }
    else{
        cursor_to_pos(2,2);
        change_color_rgb(219, 242, 7);
        cout << "Saved games:";
        for(int i=0; i<number_of_saved_games; i++){
            cursor_to_pos(3+i,2);
            change_color_rgb(66, 245, 242);
            cout << "Name: " << games[i].game_name;
        }
        cursor_to_pos(4+number_of_saved_games,2);
        change_color_rgb(219, 242, 7);
        cout << "Press m to go back to menu";
        cursor_to_pos(5+number_of_saved_games,2);
        change_color_rgb(219, 242, 7);
        cout << "Press any button to load a game";
    }
}

string choose_saved_game(){
    string line;
    cursor_to_pos(4+number_of_saved_games,2);
    cout << "                                             ";
    cursor_to_pos(5+number_of_saved_games,2);
    change_color_rgb(219, 242, 7);
    cout << "Please enter the saved game's name: ";
    reset_color();
    if (cin.peek() == '\n'){
        cin.ignore();
    }
    getline(cin,line);
    while(!is_name_taken(line)){
        clear_screen();
        show_saved_games();
        cursor_to_pos(4+number_of_saved_games,2);
        cout << "                                             ";
        cursor_to_pos(5+number_of_saved_games,2);
        change_color_rgb(219, 242, 7);
        cout << "Please enter the saved game's name: ";
        change_color_rgb(240, 4, 0);
        cursor_to_pos(6+number_of_saved_games,2);
        cout << "Game name not found! Please try again.";
        delay(2000);
        cursor_to_pos(6+number_of_saved_games,2);
        cout << "                                              ";
        cursor_to_pos(5+number_of_saved_games,38);
        reset_color();
        if (cin.peek() == '\n'){
            cin.ignore();
        }
        getline(cin,line);
    }
    return line;
}

time_t load_game(string line){
    int found_index = 0;
    for(int i=0; i<number_of_saved_games; i++){
        if(games[i].game_name == line){
            found_index = i;
            break;
        }
    }
    table = games[found_index].table;
    correct_moves = games[found_index].correct_moves;
    wrong_moves = games[found_index].wrong_moves;
    chances = games[found_index].chances;
    saved_time = games[found_index].time_record;
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            last_rows[i][j] = games[found_index].table.rows[i][j];
        }
    }
    return games[found_index].time_deadline;
}

void store_username(string username) {
    ofstream outFile("usernames.txt", ios::app);
    outFile << username << endl << endl;
    outFile.close();
}

bool is_username_taken(string username) {
    ifstream inFile("usernames.txt");
    string line;
    while (getline(inFile, line)){
        if (line == username){
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}

int subgrid_finder(int x, int y){
    if(1<=x && x<=3){
        if(1<=y && y<=3){
            return 0;
        }
        if(4<=y && y<=6){
            return 3;
        }
        if(7<=y && y<=9){
            return 6;
        }
    }
    if(4<=x && x<=6){
        if(1<=y && y<=3){
            return 1;
        }
        if(4<=y && y<=6){
            return 4;
        }
        if(7<=y && y<=9){
            return 7;
        }
    }
    if(7<=x && x<=9){
        if(1<=y && y<=3){
            return 2;
        }
        if(4<=y && y<=6){
            return 5;
        }
        if(7<=y && y<=9){
            return 8;
        }
    }
    return 0;
}

int subgrid_index_finder(int subgrid, int x, int y){
    switch(subgrid){
        case 0: return 3*(y-1)+x-1;
        case 1: return 3*(y-1)+x-4;
        case 2: return 3*(y-1)+x-7;
        case 3: return 3*(y-4)+x-1;
        case 4: return 3*(y-4)+x-4;
        case 5: return 3*(y-4)+x-7;
        case 6: return 3*(y-7)+x-1;
        case 7: return 3*(y-7)+x-4;
        case 8: return 3*(y-7)+x-7;
    }
    return 0;
}

bool sudoku_checker(SudokuTable table, int x, int y){
    int subgrid = subgrid_finder(x,y);
    int subgrid_index = subgrid_index_finder(subgrid,x,y);
    int rows_repeats = 0, cols_repeats = 0, subgrid_repeats = 0;
    for(int i=0; i<9; i++){
        if(table.rows[y-1][x-1] == table.rows[y-1][i] && table.rows[y-1][x-1] != 0){
            rows_repeats++;
        }
    }
    for(int i=0; i<9; i++){
        if(table.cols[x-1][y-1] == table.cols[x-1][i] && table.cols[x-1][y-1] != 0){
            cols_repeats++;
        }
    }
    for(int i=0; i<9; i++){
        if(table.subgrid[subgrid][subgrid_index] == table.subgrid[subgrid][i] && table.subgrid[subgrid][subgrid_index] != 0){
            subgrid_repeats++;
        }
    }
    if((rows_repeats != 1 || cols_repeats != 1 || subgrid_repeats != 1) && table.rows[y-1][x-1] != 0){
        return false;
    }
    else{
        return true;
    }
}

void cell_navigator(int x, int y){
    int rows_step = (get_window_rows()-9)/9;
    int cols_step = (get_window_cols()-9)/9; 
    cursor_to_pos(4+rows_step*(y-1)+(rows_step+1)/2,5+cols_step*(x-1)+(cols_step+1)/2);
}

bool isNotConst(const SudokuTable& table, int i, int j) {
    for(int k = 0; k < 30; k++) {
        if(i == table.const_cells_coordinates[k][0] && j == table.const_cells_coordinates[k][1]) {
            return false;
        }
    }
    return true;
}

void sudoku_reset(SudokuTable& table){
    for(int i=1; i<10; i++){
        for(int j=1; j<10; j++){
            table.rows[i-1][j-1] = table.cols[j-1][i-1] = table.subgrid[subgrid_finder(j,i)][subgrid_index_finder(subgrid_finder(j,i),j,i)] = last_rows[i-1][j-1] = 0;
        }
    }
    table.rows[0][2] = table.cols[2][0] = table.subgrid[0][2] = last_rows[0][2] = 3;
    table.rows[0][4] = table.cols[4][0] = table.subgrid[1][1] = last_rows[0][4] = 5;
    table.rows[0][8] = table.cols[8][0] = table.subgrid[2][2] = last_rows[0][8] = 9;
    table.rows[1][0] = table.cols[0][1] = table.subgrid[0][3] = last_rows[1][0] = 4;
    table.rows[1][2] = table.cols[2][1] = table.subgrid[0][5] = last_rows[1][2] = 6;
    table.rows[1][3] = table.cols[3][1] = table.subgrid[1][3] = last_rows[1][3] = 7;
    table.rows[1][7] = table.cols[7][1] = table.subgrid[2][4] = last_rows[1][7] = 2;
    table.rows[2][1] = table.cols[1][2] = table.subgrid[0][7] = last_rows[2][1] = 8;
    table.rows[2][5] = table.cols[5][2] = table.subgrid[1][8] = last_rows[2][5] = 3;
    table.rows[2][6] = table.cols[6][2] = table.subgrid[2][6] = last_rows[2][6] = 4;
    table.rows[2][8] = table.cols[8][2] = table.subgrid[2][8] = last_rows[2][8] = 6;
    table.rows[3][4] = table.cols[4][3] = table.subgrid[4][1] = last_rows[3][4] = 7;
    table.rows[3][6] = table.cols[6][3] = table.subgrid[5][0] = last_rows[3][6] = 3;
    table.rows[3][7] = table.cols[7][3] = table.subgrid[5][1] = last_rows[3][7] = 1;
    table.rows[4][3] = table.cols[3][4] = table.subgrid[4][3] = last_rows[4][3] = 5;
    table.rows[4][4] = table.cols[4][4] = table.subgrid[4][4] = last_rows[4][4] = 6;
    table.rows[4][7] = table.cols[7][4] = table.subgrid[5][4] = last_rows[4][7] = 9;
    table.rows[5][2] = table.cols[2][5] = table.subgrid[3][8] = last_rows[5][2] = 7;
    table.rows[5][4] = table.cols[4][5] = table.subgrid[4][7] = last_rows[5][4] = 1;
    table.rows[5][5] = table.cols[5][5] = table.subgrid[4][8] = last_rows[5][5] = 2;
    table.rows[5][8] = table.cols[8][5] = table.subgrid[5][8] = last_rows[5][8] = 4;
    table.rows[6][0] = table.cols[0][6] = table.subgrid[6][0] = last_rows[6][0] = 9;
    table.rows[6][4] = table.cols[4][6] = table.subgrid[7][1] = last_rows[6][4] = 3;
    table.rows[6][8] = table.cols[8][6] = table.subgrid[8][2] = last_rows[6][8] = 5;
    table.rows[7][3] = table.cols[3][7] = table.subgrid[7][3] = last_rows[7][3] = 6;
    table.rows[7][7] = table.cols[7][7] = table.subgrid[8][4] = last_rows[7][7] = 7;
    table.rows[7][8] = table.cols[8][7] = table.subgrid[8][5] = last_rows[7][8] = 8;
    table.rows[8][1] = table.cols[1][8] = table.subgrid[6][7] = last_rows[8][1] = 6;
    table.rows[8][3] = table.cols[3][8] = table.subgrid[7][6] = last_rows[8][3] = 8;
    table.rows[8][6] = table.cols[6][8] = table.subgrid[8][6] = last_rows[8][6] = 2;
}

void display_sudoku_table(SudokuTable table){
    int rows = get_window_rows();
    int cols = get_window_cols();
    int rows_step = (rows-9)/9;
    int cols_step = (cols-9)/9;
    cursor_to_pos(rows,cols);
    change_color_rgb(186, 180, 179);
    for(int j=4; j<=4+9*rows_step; j+=rows_step){
        for(int i=5; i<=5+9*cols_step; i++){
            cursor_to_pos(j,i);
            cout << "-";
        }
    }
    for(int i=5; i<=5+9*cols_step; i+=cols_step){
        for(int j=4; j<=4+8*rows_step; j+=rows_step){
            for(int k=1; k<rows_step; k++){
                cursor_to_pos(j+k,i);
                cout << "|";
            }
        }
    }
    for(int i=1; i<=9; i++){
        for(int j=1; j<=9; j++){
            cell_navigator(i,j);
            cout << table.cols[i-1][j-1];
        }
    }
}

void sudoku_updater(SudokuTable& table, int x, int y){
    if(isNotConst(table,y,x)){
        if(!sudoku_checker(table,x,y)){
            cursor_to_pos(2,(get_window_cols()/2)+10);
            change_color_rgb(255, 106, 0);
            cout << "WRONG MOVE!";
            wrong_moves++;
            chances--;
            table.rows[y-1][x-1] = last_rows[y-1][x-1];
            table.cols[x-1][y-1] = last_rows[y-1][x-1];
            table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)] = last_rows[y-1][x-1];
            reset_color();
            cell_navigator(x,y);
            cout << table.rows[y-1][x-1];
            cursor_to_pos(2,(get_window_cols()/2)+2);
            switch(chances){
                case 4: change_color_rgb(47, 207, 35); cout << chances; break;
                case 3: change_color_rgb(47, 207, 35); cout << chances; break;
                case 2: change_color_rgb(227, 206, 18); cout << chances; break;
                case 1: change_color_rgb(242, 21, 10); cout << chances; break;
                case 0: change_color_rgb(46, 44, 44); cout << chances; break;
            }
            delay(2000);
            cursor_to_pos(2,(get_window_cols()/2)+10);
            cout << "              ";
            cell_navigator(x,y);
            reset_color();
        }
        else{
            if(table.rows[y-1][x-1] != 0){
                correct_moves++;
            }
            last_rows[y-1][x-1] = table.rows[y-1][x-1];
        }
    }
    else{
        cursor_to_pos(2,(get_window_cols()/2)+10);
        change_color_rgb(255, 106, 0);
        cout << "You can't change an original cell.";
        table.rows[y-1][x-1] = last_rows[y-1][x-1];
        table.cols[x-1][y-1] = last_rows[y-1][x-1];
        table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)] = last_rows[y-1][x-1];
        reset_color();
        cell_navigator(x,y);
        cout << table.rows[y-1][x-1];
        delay(2000);
        cursor_to_pos(2,(get_window_cols()/2)+10);
        cout << "                                     ";
        cell_navigator(x,y);
        reset_color();
    }
}

bool win_check(SudokuTable table){
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(table.rows[i][j] == 0){
                return false;
            }
        }
    }
    return true;
}

time_t time_deadline(Difficulty difficulty){
    switch(difficulty){
        case Easy: return 300;
        case Medium: return 180;
        case Hard: return 120;
    }
    return 0;
}

time_t timer(){
    return (time(nullptr) - origin_time - paused_time + saved_time);
}

void pause_screen(){
    char button;
    time_t pause_start = time(nullptr);
    clear_screen();
    cursor_to_pos((get_window_rows()/2)-1,(get_window_cols()/2)-12);
    change_color_rgb(255, 247, 0);
    cout << "Game paused. Press Esc to continue...";
    do{
        button = getch();
    }while(button != 27);
    paused_time += time(nullptr) - pause_start;
    clear_screen();
    cursor_to_pos(2,10);
    change_color_rgb(66, 245, 242);
    cout << "Timer: ";
    cursor_to_pos(2,(get_window_cols()/2)-7);
    change_color_rgb(66, 245, 242);
    cout << "Chances: ";
    switch(chances){
        case 5: change_color_rgb(47, 207, 35); cout << chances; break;
        case 4: change_color_rgb(47, 207, 35); cout << chances; break;
        case 3: change_color_rgb(47, 207, 35); cout << chances; break;
        case 2: change_color_rgb(227, 206, 18); cout << chances; break;
        case 1: change_color_rgb(242, 21, 10); cout << chances; break;
        case 0: change_color_rgb(46, 44, 44); cout << chances; break;
    }
    display_sudoku_table(table);
}

void game_name_getter(time_t time_deadline){
    string line;
    time_t pause_start = time(nullptr);
    clear_screen();
    cursor_to_pos((get_window_rows()/2)-1,(get_window_cols()/2)-12);
    change_color_rgb(255, 247, 0);
    cout << "Enter a name for your saved game: ";
    reset_color();
    if (cin.peek() == '\n'){
        cin.ignore();
    }
    getline(cin,line);
    while(is_name_taken(line)){
        clear_screen();
        cursor_to_pos((get_window_rows()/2)-1,(get_window_cols()/2)-12);
        change_color_rgb(245, 9, 5);
        cout << "Game name is taken! ";
        delay(2000);
        cursor_to_pos((get_window_rows()/2)-1,(get_window_cols()/2)-12);
        cout << "                     ";
        cursor_to_pos((get_window_rows()/2)-1,(get_window_cols()/2)-12);
        change_color_rgb(255, 247, 0);
        cout << "Enter a name for your saved game: ";
        reset_color();
        if (cin.peek() == '\n'){
            cin.ignore();
        }
        getline(cin,line);
    }
    clear_screen();
    cursor_to_pos((get_window_rows()/2)-1,(get_window_cols()/2)-12);
    change_color_rgb(255, 247, 0);
    cout << "Game saved successfully!";
    delay(2000);
    paused_time += time(nullptr) - pause_start;
    save_game(line,table,correct_moves,wrong_moves,chances,timer(),time_deadline);
    saved_game = true;
    clear_screen();
    cursor_to_pos(2,10);
    change_color_rgb(66, 245, 242);
    cout << "Timer: ";
    cursor_to_pos(2,(get_window_cols()/2)-7);
    change_color_rgb(66, 245, 242);
    cout << "Chances: ";
    switch(chances){
        case 5: change_color_rgb(47, 207, 35); cout << chances; break;
        case 4: change_color_rgb(47, 207, 35); cout << chances; break;
        case 3: change_color_rgb(47, 207, 35); cout << chances; break;
        case 2: change_color_rgb(227, 206, 18); cout << chances; break;
        case 1: change_color_rgb(242, 21, 10); cout << chances; break;
        case 0: change_color_rgb(46, 44, 44); cout << chances; break;
    }
    display_sudoku_table(table);
}

void show_results(bool win){
    change_color_rgb(66, 245, 242);
    box_maker(2,(get_window_cols()/2)-15,50,10);
    cursor_to_pos(3,(get_window_cols()/2)+7);
    change_color_rgb(255, 247, 0);
    cout << "Results";
    cursor_to_pos(4,(get_window_cols()/2)-5);
    cout << "Status: ";
    if(win){
        change_color_rgb(43, 255, 0);
        cout << "Won";
    }
    else{
        change_color_rgb(255, 33, 33);
        cout << "Lost";
    }
    change_color_rgb(255, 247, 0);
    cursor_to_pos(5,(get_window_cols()/2)-5);
    cout << "Correct moves: " << correct_moves;
    cursor_to_pos(6,(get_window_cols()/2)-5);
    cout << "Wrong moves: " << wrong_moves;
    cursor_to_pos(7,(get_window_cols()/2)-5);
    cout << "Time Record: "; display_timer(time_record);
    cursor_to_pos(8,(get_window_cols()/2)-5);
    cout << "Score: " << correct_moves - wrong_moves;
    cursor_to_pos(9,(get_window_cols()/2)-5);
    cout << "Press m to go back to main menu";
    char button;
    do{
        button = getch();
    }while(button != 'm');
    clear_screen();
}

void sudoku_editor(int x, int y, SudokuTable& table, int& chances, time_t time_deadline){
    int rows_step = (get_window_rows()-9)/9;
    int cols_step = (get_window_cols()-9)/9;
    cell_navigator(1,1);
    char button;
    do{
        cursor_to_pos(2,17);
        change_color_rgb(66, 245, 242);
        display_timer(timer());
        reset_color();
        cell_navigator(x,y);
        button = getch();
        switch(button){
            case 72: if(y==1){break;}else{y--;} cell_navigator(x,y); break;
            case 80: if(y==9){break;}else{y++;} cell_navigator(x,y); break;
            case 77: if(x==9){break;}else{x++;} cell_navigator(x,y); break;
            case 75: if(x==1){break;}else{x--;} cell_navigator(x,y); break;
            case 27: pause_screen(); break;
            case 115: game_name_getter(time_deadline); break;
            case 109: if(saved_game){clear_screen(); return;}else{cursor_to_pos(2,(get_window_cols()/2)+10); change_color_rgb(255, 106, 0); cout << "You can't exit an unsaved game.";
            delay(2000); cursor_to_pos(2,(get_window_cols()/2)+10); cout << "                                       "; cell_navigator(x,y); reset_color();} break;
            case (int)('0'): cell_navigator(x,y); cout << "0"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=0; sudoku_updater(table,x,y); break;
            case (int)('1'): cell_navigator(x,y); cout << "1"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=1; sudoku_updater(table,x,y); break;
            case (int)('2'): cell_navigator(x,y); cout << "2"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=2; sudoku_updater(table,x,y); break;
            case (int)('3'): cell_navigator(x,y); cout << "3"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=3; sudoku_updater(table,x,y); break;
            case (int)('4'): cell_navigator(x,y); cout << "4"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=4; sudoku_updater(table,x,y); break;
            case (int)('5'): cell_navigator(x,y); cout << "5"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=5; sudoku_updater(table,x,y); break;
            case (int)('6'): cell_navigator(x,y); cout << "6"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=6; sudoku_updater(table,x,y); break;
            case (int)('7'): cell_navigator(x,y); cout << "7"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=7; sudoku_updater(table,x,y); break;
            case (int)('8'): cell_navigator(x,y); cout << "8"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=8; sudoku_updater(table,x,y); break;
            case (int)('9'): cell_navigator(x,y); cout << "9"; table.rows[y-1][x-1]=table.cols[x-1][y-1]=table.subgrid[subgrid_finder(x,y)][subgrid_index_finder(subgrid_finder(x,y),x,y)]=9; sudoku_updater(table,x,y); break;
        }
    }while(button != 'q' && chances != 0 && (!win_check(table)) && timer()<time_deadline);
    if(timer()>=time_deadline){
        time_record = time_deadline;
    }
    else{
        time_record = timer();
    }
    total_time += time_record;
    total_correct_moves += correct_moves;
    total_wrong_moves += wrong_moves;
    if(win_check(table)){
        won_games++;
        clear_screen();
        cursor_to_pos((get_window_rows()/2),(get_window_cols()/2)-5);
        change_color_rgb(43, 255, 0);
        cout << "You won.";
        delay(2000);
    }
    else{
        clear_screen();
        lost_games++;
        cursor_to_pos((get_window_rows()/2),(get_window_cols()/2)-5);
        change_color_rgb(255, 33, 33);
        cout << "You lost.";
        delay(2000);
    }
    clear_screen();
    show_results(win_check(table));
}

string username_getter(){
    int cols = get_window_cols();
    int rows = get_window_rows();
    string username;
    change_color_rgb(66, 245, 242);
    box_maker((rows/2)-2,(cols/2)-52,104,6);
    cursor_to_pos(rows/2,(cols/2)-15);
    cout << "Please enter your username:" << endl;
    cursor_to_pos((rows/2)+1,(cols/2)-50);
    getline(cin, username);
    while(is_username_taken(username)){
        cursor_to_pos((rows/2)+1,(cols/2)-50);
        change_color_rgb(240, 4, 0);
        cout << "Username is already taken!";
        delay(2000);
        cursor_to_pos((rows/2)+1,(cols/2)-50);
        cout << "                                 ";
        cursor_to_pos((rows/2)+1,(cols/2)-50);
        change_color_rgb(66, 245, 242);
        getline(cin, username);
    }
    reset_color();
    return username;
}

char display_menu(string username){
    char input;
    int cols = get_window_cols();
    int rows = get_window_rows();
    cursor_to_pos(3,(cols/2)-1);
    change_color_rgb(227, 206, 45);
    cout << "IN THE NAME OF GOD";
    cursor_to_pos(4,(cols/2)-5);
    change_color_rgb(48, 242, 48);
    cout << "Welcome to my Sudoku game!";
    cursor_to_pos(6,(cols/2)-5);
    change_color_rgb(232, 184, 28);
    cout << "You logged in as " << username << endl;
    reset_color();
    cout << "1: ";
    change_color_rgb(230, 11, 44);
    cout << "Start a New Game" << endl;
    reset_color();
    cout << "2: ";
    change_color_rgb(230, 11, 44);
    cout << "Play a Saved Game" << endl;
    reset_color();
    cout << "3: ";
    change_color_rgb(230, 11, 44);
    cout << "Leaderboard" << endl;
    reset_color();
    cout << "4: ";
    change_color_rgb(230, 11, 44);
    cout << "Exit Game\n\n";
    reset_color();
    cursor_to_pos(12, 1);
    cout << "Please enter a number from 1 to 4: ";
    cin >> input;
    while(input < '1' || input > '4'){
        cursor_to_pos(12, 1);
        change_color_rgb(245, 9, 5);
        cout << "Invalid Input! ";
        reset_color();
        cout << "Please enter a number from 1 to 4: ";
        cin >> input;
    }
    return input;
}

Difficulty difficulty_getter(){
    char input;
    cursor_to_pos(2,(get_window_cols()/2)-10);
    change_color_rgb(237, 225, 147);
    cout << "Choose your difficulty";
    cursor_to_pos(3,(get_window_cols()/2)-10);
    change_color_rgb(20, 222, 13);
    cout << "1. Easy (5 mins)";
    cursor_to_pos(4,(get_window_cols()/2)-10);
    change_color_rgb(255, 255, 3);
    cout << "2. Medium (3 mins)";
    cursor_to_pos(5,(get_window_cols()/2)-10);
    change_color_rgb(222, 26, 22);
    cout << "3. Hard (2 mins)";
    reset_color();
    cursor_to_pos(7,(get_window_cols()/2)-10);
    cout << "Please enter a number from 1 to 3: ";
    cin >> input;
    while(input < '1' || input > '3'){
        cursor_to_pos(7,(get_window_cols()/2)-10);
        change_color_rgb(245, 9, 5);
        cout << "Invalid Input! ";
        reset_color();
        cout << "Please enter a number from 1 to 3: ";
        cin >> input;
    }
    cursor_to_pos(8,(get_window_cols()/2)-10);
    switch(input){
        case '1': change_color_rgb(20, 222, 13); cout << "Easy difficulty selected"; return Easy;
        case '2': change_color_rgb(255, 255, 3); cout << "Medium difficulty selected"; return Medium;
        case '3': change_color_rgb(222, 26, 22); cout << "Hard difficulty selected"; return Hard;
    }
    return Easy;
}