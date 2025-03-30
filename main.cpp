#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "clui.h"
#include "functions.h"
using namespace std;

int main(){
    while(true){
        clear_screen();
        string username = username_getter();
        total_correct_moves = 0;
        total_wrong_moves = 0;
        total_time = 0;
        won_games = 0;
        lost_games = 0;
        number_of_saved_games = 0;
        change_color_rgb(227, 206, 45);
        clear_screen();
        reset_color();
        char menu_input;
        do{
            menu_input = display_menu(username);
            if(menu_input == '1'){
                clear_screen();
                Difficulty difficulty_input = difficulty_getter();
                delay(1000);
                clear_screen();
                sudoku_reset(table);
                correct_moves = wrong_moves = 0;
                chances = 5;
                saved_game = false;
                origin_time = time(nullptr);
                paused_time = 0;
                saved_time = 0;
                cursor_to_pos(2,(get_window_cols()/2)-7);
                change_color_rgb(66, 245, 242);
                cout << "Chances: ";
                change_color_rgb(47, 207, 35);
                cout << chances;
                cursor_to_pos(2,10);
                change_color_rgb(66, 245, 242);
                cout << "Timer: 0:00";
                display_sudoku_table(table);
                sudoku_editor(1,1,table,chances,time_deadline(difficulty_input));
            }
            if(menu_input == '2'){
                clear_screen();
                show_saved_games();
                char button = getch();
                if(button == 'm'){
                    clear_screen();
                }
                else{
                    string name_of_game = choose_saved_game();
                    clear_screen();
                    cursor_to_pos(2,(get_window_cols()/2)-7);
                    change_color_rgb(66, 245, 242);
                    cout << "Chances: ";
                    change_color_rgb(47, 207, 35);
                    cout << chances;
                    cursor_to_pos(2,10);
                    change_color_rgb(66, 245, 242);
                    cout << "Timer: 0:00";
                    time_t time_deadline = load_game(name_of_game);
                    paused_time = 0;
                    origin_time = time(nullptr);
                    display_sudoku_table(table);
                    sudoku_editor(1,1,table,chances,time_deadline);
                }
            }
            if(menu_input == '3'){
                clear_screen();
                number_of_players = leaderboard_reader();
                if(number_of_players == 0){
                    cursor_to_pos(get_window_rows()/2,(get_window_cols()/2)-2);
                    change_color_rgb(66, 245, 242);
                    cout << "There are currently no players!";
                    delay(2000);
                    clear_screen();
                }
                else{
                    cursor_to_pos(2,2);
                    change_color_rgb(66, 245, 242);
                    cout << "Press m to go back to main menu";
                    leaderboard_sorter(number_of_players);
                    Player* leaderboard = leaderboard_setter();
                    display_leaderboard(leaderboard);
                    remove_list(leaderboard);
                }
            }
        }while(menu_input != '4');
        store_username(username);
        store_player_data(username,total_correct_moves,total_wrong_moves,won_games,lost_games,total_time);
        clear_screen();
        cin.ignore();
    }
    return 0;
}