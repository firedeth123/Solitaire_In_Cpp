/*
Author:     Connor Hays
Date:       7/3/2020
Program:    Solitaire.cpp
Hours:      50 hours
*/

#pragma GCC diagnostic ignored "-Woverflow"
#pragma GCC diagnostic error "-Wmultichar"

#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

//!CREATING ASSETS!//
enum cardsuit {spade = 'S', heart = 'H', club = 'C', diamond = 'D', none = ' '};
enum color_value {black = 1, red = 2, blue = 3, green = 4};
enum stacktype {pile_1 = '1', pile_2 = '2', pile_3 = '3', pile_4 = '4', pile_5 = '5', pile_6 = '6', pile_7 = '7', spade_foundation = 'S', heart_foundation = 'H', club_foundation = 'C', diamond_foundation = 'D', draw_stock = 'r', talon_pile = 't'};
enum stackindex {pile_1_index = 0, pile_2_index = 1, pile_3_index = 2, pile_4_index = 3, pile_5_index = 4, pile_6_index = 5, pile_7_index = 6, talon_pile_index = 7, draw_stock_index = 8, spade_foundation_index = 9, heart_foundation_index = 10, club_foundation_index = 11, diamond_foundation_index = 12};
stacktype ALL_STACKTYPES[13] = {pile_1, pile_2, pile_3, pile_4, pile_5, pile_6, pile_7, talon_pile, draw_stock, spade_foundation, heart_foundation, club_foundation, diamond_foundation}; //I create this array to handle the construction of all the stacks automatically.
cardsuit ALL_SUITS[4] = {spade, heart, club, diamond}; //Array holding all suits, used to help automate card creation.
const string black_c = "\033[30;10;107m";
const string red_c = "\033[31;1;107m";
const string blue_c = "\033[94;1;107m";
const string green_c = "\033[32;1;107m";
const string white_c = "\033[0m";

struct color_char //Structure made to allow me to print different characters with different colors.
{
    char hold_char;
    string color_escape_code;
};
struct move_composition
{
    int card_value;
    stackindex from_index;
    stackindex to_index;
    char do_a_flip;
    char move_type;
};

char upper_lower_edge = '\u00c3'+65;
char left_right_edge = '\u00c3'+48;
char top_left = '\u00c3'+87;
char top_right = '\u00bf';
char bot_left = '\u00bf'+1;
char bot_right = '\u00c3'+86;
char card_back_fill = '\u00b1';
char empty_fill = '\u0020';
char empty_field_dots = '\u0000'+248;
char two_lines = '\u0000'+185;
char mid_sec = '\u0000'+205;

char back_of_card[6][5] = {
    {top_left,upper_lower_edge,upper_lower_edge,upper_lower_edge,top_right},
    {left_right_edge,card_back_fill,card_back_fill,card_back_fill,left_right_edge},
    {left_right_edge,card_back_fill,card_back_fill,card_back_fill,left_right_edge},
    {left_right_edge,card_back_fill,card_back_fill,card_back_fill,left_right_edge},
    {left_right_edge,card_back_fill,card_back_fill,card_back_fill,left_right_edge},
    {bot_left,upper_lower_edge,upper_lower_edge,upper_lower_edge,bot_right}};
char empty_field_image[6][5] = {
    {empty_field_dots,empty_fill,empty_field_dots,empty_fill,empty_field_dots},
    {empty_field_dots,empty_fill,empty_fill,empty_fill,empty_field_dots},
    {empty_field_dots,empty_fill,empty_fill,empty_fill,empty_field_dots},
    {empty_field_dots,empty_fill,empty_fill,empty_fill,empty_field_dots},
    {empty_field_dots,empty_fill,empty_fill,empty_fill,empty_field_dots},
    {empty_field_dots,empty_fill,empty_field_dots,empty_fill,empty_field_dots}};
char spadesIm[3][3] = {
    {' ','^',' '},
    {'(',' ',')'},
    {' ',two_lines,' '}};
char heartsIm[3][3] = {
    {'/','V','\\'},
    {'\\',' ','/'},
    {' ','V',' '}};
char clubsIm[3][3] = {
    {' ','0',' '},
    {'0',mid_sec,'0'},
    {' ',two_lines,' '}};
char diamondsIm[3][3] = {
    {' ','^',' '},
    {'<',' ','>'},
    {' ','V',' '}};

void explainRulesFunction(void)
{
    cout << endl << "Solitaire is a one player game wherein the goal is to finish creating four \"Foundation\" piles (cards of the same suit that are stacked in ascending order)." << endl << "The game starts with seven piles which have the same number of cards as their pile number (pile 1 has 1 card, pile 2 has 2 cards, and so on) that only have" << endl << "the top card flipped over; no cards start off in the Foundation piles, and the remaining cards are put into the Stock which can be drawn from. Cards can be" << endl << "moved from the Stock to the Talon (a pile separate from the Stock containing the cards that have been drawn), from the Talon to the Tableau (the area with" << endl << "the seven piles), from the Talon to the Foundation, from the Tableau to the top of the Foundation, and from the top of the Foundation to the Tableau. Cards" << endl << "can only be moved within the Tableau if the card is being moved to a card of a different color and a higher order/value, and cards can only be moved to the" << endl << "Foundation of the same suit if the card being put into the Foundation is of a higher order/value than the card at the top of the Foundation (Aces are the" << endl << "first to be moved to the Foundation). You can also chose to move stacks of cards within the Tableau by targeting card within the middle of a pile (the target" << endl << "card and all cards below the target card will be moved)." << endl << endl << "For this program, cards will be drawn 1 at a time and all shuffles will be random - meaning some hands will be unwinnable, so be careful!";
};

void explainControlsFunction(void)
{
    cout << endl << "To move a card, enter [move card] [from stack number] [to stack number] in that order, separated by a space." << endl << ">>[move card]: enter the value displayed on the card (or the lowercase equivalent of the letter on" << endl << "\tthe card). Can be \"a\", 2-10, \"j\", \"q\", or \"k\"." << endl << ">>[from stack number]: can be 1-7 to target a specific stack in the Tableau (1 is the leftmost stack," << endl << "\t2 is the stack to the right of 1, and so on), or \"t\" to target the Talon." << endl << "\tYou can also enter \"sf\", \"hf\", \"cf\", or \"df\" to target that suit's foundation." << endl << ">>[to stack number]: same rules as the [from stack number] field." << endl << "Additionally, you can enter \"help\" to see the rules controls, \"quit\" to quit, or \"d\" to draw from the" << endl << "Stock, \"u\" to undo a move, or \"refresh\" to refresh the screen." << endl << "Make sure to enter your commands in lowercase letters." << endl;
}
//!END OF ASSETS!//

//!HANDLING CODE!//
//!==================================================================================!//
class Card
{
private:
	int value;
	cardsuit suit;
	int color;
	string color_sequence;
	int flip_dir; //Int that can either be 1 (face up) or 0 (face down)
	int movable; //can be either 1 (movable) or 0 (immovable).
    char card_image[6][5] = {
        {top_left,upper_lower_edge,upper_lower_edge,upper_lower_edge,top_right},
        {left_right_edge,'v',' ','s',left_right_edge},
        {left_right_edge,'p','p','p',left_right_edge},
        {left_right_edge,'p','p','p',left_right_edge},
        {left_right_edge,'p','p','p',left_right_edge},
        {bot_left,upper_lower_edge,upper_lower_edge,upper_lower_edge,bot_right}
        };
public:
    Card(int thisValue, cardsuit thisSuit, int flip);
    int getValue(void) {return value;};
    cardsuit getSuit(void) {return suit;};
    int getThisColor(void) {return color;};
    int getMovableColor(void);
    string getColorSequence(void) {return color_sequence;};
    int getMovable(void) {return movable;};
    char getImagePiece(int y, int x) {return card_image[y][x];};
    int getFlip(void) {return flip_dir;};
    void FlipCard(void);
    void immobilize(void) {movable = 0;};
    void makeMobile(void) {movable = 1;};
};

Card::Card(int thisValue, cardsuit thisSuit, int flip = 1)
{
    value = thisValue;
    suit = thisSuit;
    if (suit == 'H' || suit == 'D')
    {
        color_sequence = red_c;
        color = red;
    }
    else if (suit == 'C' || suit == 'S')
    {
        color_sequence = black_c;
        color = black;
    }
    else
    {
        color_sequence = green_c;
        color = green;
    }
    flip_dir = flip;
    movable = flip;
    switch(thisValue)
    {
    case 0: for(int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    card_image[i][j] = empty_field_image[i][j];
                }
            }
            movable = 0;
            break;
    case 1: card_image[1][1] = 'A';
            break;
    case 2: card_image[1][1] = '2';
            break;
    case 3: card_image[1][1] = '3';
            break;
    case 4: card_image[1][1] = '4';
            break;
    case 5: card_image[1][1] = '5';
            break;
    case 6: card_image[1][1] = '6';
            break;
    case 7: card_image[1][1] = '7';
            break;
    case 8: card_image[1][1] = '8';
            break;
    case 9: card_image[1][1] = '9';
            break;
    case 10: card_image[1][1] = '1';
            card_image[1][2] = '0';
            break;
    case 11: card_image[1][1] = 'J';
            break;
    case 12: card_image[1][1] = 'Q';
            break;
    case 13: card_image[1][1] = 'K';
            break;
    case 14: for(int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    card_image[i][j] = empty_field_image[i][j];
                }
            }
            movable = 0;
            break;
    }

    card_image[1][3] = suit;

    if (flip_dir == 1)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (suit == 'S')
                    card_image[i+2][j+1] = spadesIm[i][j];
                if (suit == 'H')
                    card_image[i+2][j+1] = heartsIm[i][j];
                if (suit == 'C')
                    card_image[i+2][j+1] = clubsIm[i][j];
                if (suit == 'D')
                    card_image[i+2][j+1] = diamondsIm[i][j];
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                card_image[i+1][j+1] = card_back_fill;
            }
        }
        color = blue;
        color_sequence = blue_c;
    }
}

int Card::getMovableColor(void)
{
    if (color == 1)
        return 2;
    else if (color == 2)
        return 1;
    else
        return 0;
}

void Card::FlipCard(void)
{
    if (flip_dir == 0)
    {
        flip_dir = 1;
    }
    else
    {
        flip_dir = 0;
    }

    if (suit == 'H' || suit == 'D')
    {
        color_sequence = red_c;
        color = red;
    }
    else if (suit == 'C' || suit == 'S')
    {
        color_sequence = black_c;
        color = black;
    }
    else
    {
        color_sequence = green_c;
        color = green;
    }
    movable = flip_dir;
    switch(value)
    {
    case 0: for(int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    card_image[i][j] = empty_field_image[i][j];
                }
            }
            movable = 0;
            break;
    case 1: card_image[1][1] = 'A';
            card_image[1][2] = ' ';
            break;
    case 2: card_image[1][1] = '2';
            card_image[1][2] = ' ';
            break;
    case 3: card_image[1][1] = '3';
            card_image[1][2] = ' ';
            break;
    case 4: card_image[1][1] = '4';
            card_image[1][2] = ' ';
            break;
    case 5: card_image[1][1] = '5';
            card_image[1][2] = ' ';
            break;
    case 6: card_image[1][1] = '6';
            card_image[1][2] = ' ';
            break;
    case 7: card_image[1][1] = '7';
            card_image[1][2] = ' ';
            break;
    case 8: card_image[1][1] = '8';
            card_image[1][2] = ' ';
            break;
    case 9: card_image[1][1] = '9';
            card_image[1][2] = ' ';
            break;
    case 10: card_image[1][1] = '1';
            card_image[1][2] = '0';
            break;
    case 11: card_image[1][1] = 'J';
            card_image[1][2] = ' ';
            break;
    case 12: card_image[1][1] = 'Q';
            card_image[1][2] = ' ';
            break;
    case 13: card_image[1][1] = 'K';
            card_image[1][2] = ' ';
            break;
    case 14: for(int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    card_image[i][j] = empty_field_image[i][j];
                }
            }
            movable = 0;
            break;
    }

    card_image[1][3] = suit;

    if (flip_dir == 1)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (suit == 'S')
                    card_image[i+2][j+1] = spadesIm[i][j];
                if (suit == 'H')
                    card_image[i+2][j+1] = heartsIm[i][j];
                if (suit == 'C')
                    card_image[i+2][j+1] = clubsIm[i][j];
                if (suit == 'D')
                    card_image[i+2][j+1] = diamondsIm[i][j];
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                card_image[i+1][j+1] = card_back_fill;
            }
        }
        color = blue;
        color_sequence = blue_c;
    }
}

Card emptySpaceBack(14, none);
Card emptySpaceFront(0, none);

vector<Card> flipDeck(vector<Card> deck)
{
    int number_of_cards = deck.size();
    for (int i = 0; i < number_of_cards; i++)
    {
        deck[i].FlipCard();
    }
    return deck;
}

//!==================================================================================!//
class FieldStack
{
private:
	stacktype stack_identifyer;
public:
    vector<Card> cardsInStack = {};
    FieldStack(stacktype stackId);
    stacktype getID(void) {return stack_identifyer;};
	void pushCard(Card cardToPush) {cardsInStack.push_back(cardToPush);};
	void flip_stack(void);
	void MoveCard(int searchCardValue, vector<Card> whereToMove);
	Card searchForCardInStack(int whatCardValue);
	vector<Card> getCardsAfter(int whatCardValue);
};

FieldStack::FieldStack(stacktype stackId)
{
    stack_identifyer = stackId;
    if (stackId == 't' || stackId == 'r')
    {
        //Do nothing.
    }
    else if (stackId == '1' || stackId == '2' || stackId == '3' || stackId == '4' || stackId == '5' || stackId == '6' || stackId == '7')
    {
        cardsInStack.push_back(emptySpaceBack);
    }
    else
    {
        cardsInStack.push_back(emptySpaceFront);
    }
}

void FieldStack::flip_stack(void)
{
    cardsInStack = flipDeck(cardsInStack);
}

Card FieldStack::searchForCardInStack(int whatCardValue)
{
    for (int i = 0; i < cardsInStack.size(); i++)
    {
        if (cardsInStack[i].getValue() == whatCardValue && cardsInStack[i].getMovable() == 1)
        {
            return cardsInStack[i];
        }
    }
    return cardsInStack.back();
}

vector<Card> FieldStack::getCardsAfter(int whatCardValue)
{
    vector<Card> tempPlayerHand = {};

    int foundDepth = 0;
    for (int i = 0; i < cardsInStack.size(); i++)
    {
        if (cardsInStack[i].getValue() == whatCardValue && cardsInStack[i].getMovable() == 1)
        {
            foundDepth = cardsInStack.size() - i;
        }
    }

    if (foundDepth == 0 && !(cardsInStack.back().getValue() == whatCardValue))
        goto end_of_function;

    for (int i = 0; i < foundDepth; i++)
    {
        tempPlayerHand.push_back(cardsInStack.back());
        cardsInStack.pop_back();
    }

    end_of_function:
    return tempPlayerHand;
}

//!==================================================================================!//
class Playspace
{
private:
    int width;
    int height;
    color_char playspace_image[55][55];
    char refresh_type;
public:
    Playspace(int objWidth, int objHeight);
    void display_space(void);
    void set_scroll_type(char chosen_type) {refresh_type = chosen_type;};
    void set_single_char(color_char set_this, int placeY, int placeX) {playspace_image[placeY][placeX] = set_this;};
    void place_card(Card heldCard, int placeY, int placeX);
    void place_stacks(vector<FieldStack> stacks_container);
    void hard_reset_space(void);
    void full_refresh_space(vector<FieldStack> stacks_container);
};

Playspace::Playspace(int objWidth, int objHeight) //Playspace object class constructor.
{
    width = objWidth;
    height = objHeight;
    refresh_type = 'c'; //Default refresh_type.
    color_char tempChar;
    tempChar.color_escape_code = black_c;
    tempChar.hold_char = ' ';
    for (int i = 0; i < objHeight; i++) //Initializes Playspace with spaces in all locations.
    {
        for (int j = 0; j < objWidth; j++)
        {
            playspace_image[i][j] = tempChar;
        }
    }

    tempChar.color_escape_code = green_c;
    tempChar.hold_char = '+';
    for (int i = 0; i < objWidth; i++) //Draws borders of the Playspace.
    {
        playspace_image[i][0] = tempChar;
        playspace_image[i][objWidth-1] = tempChar;
        playspace_image[0][i] = tempChar;
        playspace_image[objHeight-1][i] = tempChar;
    }
}

void Playspace::display_space(void)
{
    if (refresh_type == 's') //Checks to see if the refresh type is the one that scrolls. If it is, the system will spit out 25 newlines to the console, meaning that the Playspace will be displayed at the bottom of the console.
    {
        for (int i = 0; i < 25; i++)
        {
            cout << endl;
        }
    }
    else
        system("CLS"); //Used to clear the console.

    for (int i = 0; i < height; i++) //Iterates through each character in the Playspace array and outputs it to the console with its color.
    {
        for (int j = 0; j < width; j++)
        {
            cout << white_c << playspace_image[i][j].color_escape_code << playspace_image[i][j].hold_char; //white_c is outputted first because it makes the black text look darker.
        }
        cout << endl;
    }
    cout << white_c << black_c;
}

void Playspace::place_card(Card heldCard, int placeY, int placeX) //Places only a single card.
{
    color_char tempChar;
    tempChar.color_escape_code = heldCard.getColorSequence();
    if (!(placeX < 0 || placeX > 49 || placeY < 0 || placeY > 50)) //Prevents code from placing a card outside the bounds of the Playspace's image array. If this wasn't here the program would crash without warning if a card was slightly out of bounds.
    {
        for (int i = 0; i < 6; i++) //Replaces characters in the Playspace's array with the specified card's image at the location the function specifies.
        {
            for (int j = 0; j < 5; j++)
            {
                tempChar.hold_char = heldCard.getImagePiece(i,j);
                playspace_image[placeY+i][placeX+j] = tempChar;
            }
        }
    }
}

void Playspace::hard_reset_space(void) //Resets all spaces within the border of the playspace.
{
    color_char tempChar;
    tempChar.color_escape_code = black_c;
    tempChar.hold_char = ' ';
    for (int i = 1; i < height-1; i++)
    {
        for (int j = 1 ;j < width-1; j++)
        {
            playspace_image[i][j] = tempChar;
        }
    }
    for (int i = 0; i < 7; i++)
    {
        if (i == 6)
            this->place_card(emptySpaceFront, 1, 37);
        this->place_card(emptySpaceFront, 1, 8*i+1);
    }
    for (int i = 0; i < 7; i++)
    {
        this->place_card(emptySpaceFront, 8, 8*i+1);
    }

    char stacklabels[7] = {'1', '2', '3', '4', '5', '6', '7'};
    for (int i = 0; i < 7; i++)
    {
        tempChar.hold_char = stacklabels[i];
        this->set_single_char(tempChar, 7, 8*i+3);
    }
}

void Playspace::place_stacks(vector<FieldStack> stacks_container)
{
    for (int i = 0; i < 7; i++) //!Places the stacks in the Tableau.
    {
        for (int j = 1; j < stacks_container[i].cardsInStack.size(); j++) //Does not do anything if there are no cards in the container.
        {
            this->place_card(stacks_container[i].cardsInStack[j], 2*(j-1)+8, 1+(8*i));
        }
    }

    if (stacks_container[draw_stock_index].cardsInStack.size() != 0)
        this->place_card(stacks_container[draw_stock_index].cardsInStack.back(), 1, 49); //!Places last card in the draw stock.

    //!Next block places the last cards of the foundations. Note that they are only drawn if they have a card in them.
    if (stacks_container[spade_foundation_index].cardsInStack.size() != 0)
        this->place_card(stacks_container[spade_foundation_index].cardsInStack.back(), 1, 1);
    if (stacks_container[heart_foundation_index].cardsInStack.size() != 0)
        this->place_card(stacks_container[heart_foundation_index].cardsInStack.back(), 1, 9);
    if (stacks_container[club_foundation_index].cardsInStack.size() != 0)
        this->place_card(stacks_container[club_foundation_index].cardsInStack.back(), 1, 17);
    if (stacks_container[diamond_foundation_index].cardsInStack.size() != 0)
        this->place_card(stacks_container[diamond_foundation_index].cardsInStack.back(), 1, 25);

    //!Last block places the last three cards present in the Talon.
    int talonSize = stacks_container[talon_pile_index].cardsInStack.size();
    switch(talonSize)
    {
        case 0: break;
        case 1: this->place_card(stacks_container[talon_pile_index].cardsInStack.back(), 1, 33);
                break;
        case 2: this->place_card(stacks_container[talon_pile_index].cardsInStack[talonSize-2], 1, 33);
                this->place_card(stacks_container[talon_pile_index].cardsInStack.back(), 1, 37);
                break;
        default: this->place_card(stacks_container[talon_pile_index].cardsInStack[talonSize-3], 1, 33);
                this->place_card(stacks_container[talon_pile_index].cardsInStack[talonSize-2], 1, 37);
                this->place_card(stacks_container[talon_pile_index].cardsInStack.back(), 1, 41);
    }
}

void Playspace::full_refresh_space(vector<FieldStack> stacks_container)
{
    this->hard_reset_space();
    this->place_stacks(stacks_container);
    this->display_space();
}

//!==================================================================================!//

//!These are the global objects I need to play the game. Methods in the "Game" class use these to "communicate" with each other and with methods in other classes.
vector<FieldStack> ALLStacks = {};
vector<Card> original_deck = {};
Playspace* MyPlayspace = new Playspace ( 55, 55 );
vector<move_composition> FullMoveList = {};

//!Functions to handle controls.
bool DrawCard(void)
{
    if (ALLStacks[draw_stock_index].cardsInStack.size() == 0) //Recycles Talon if there are no cards in the stock.
    {
        if (ALLStacks[talon_pile_index].cardsInStack.size() == 0) //!Does nothing if there are no cards in Talon.
        {
            return false;
        }
        else
        {
            int staticInt = ALLStacks[talon_pile_index].cardsInStack.size();
            for (int i = 0; i < staticInt; i++)
            {
                ALLStacks[draw_stock_index].cardsInStack.push_back(ALLStacks[talon_pile_index].cardsInStack.back());
                ALLStacks[draw_stock_index].cardsInStack.back().FlipCard();
                ALLStacks[talon_pile_index].cardsInStack.pop_back();
            }
            return true;
        }
    }
    else //Draws a card normally otherwise.
    {
        ALLStacks[talon_pile_index].cardsInStack.push_back(ALLStacks[draw_stock_index].cardsInStack.back());
        ALLStacks[talon_pile_index].cardsInStack.back().FlipCard();
        ALLStacks[draw_stock_index].cardsInStack.pop_back();
        return true;
    }
}



void MoveCard(int searchCardValue, stackindex StackFromIndex, stackindex StackToIndex)
{
    move_composition this_move;
    this_move.card_value = searchCardValue;
    this_move.from_index = StackFromIndex;
    this_move.to_index = StackToIndex;
    this_move.move_type = 'U'; //'U' for "Unknown".
    this_move.do_a_flip = 'f';

    int LCValue_In_FromStack = ALLStacks[StackFromIndex].cardsInStack.back().getValue();
    int LCValue_In_ToStack = ALLStacks[StackToIndex].cardsInStack.back().getValue();
    char LCSuit_In_FromStack = static_cast<char> (ALLStacks[StackFromIndex].cardsInStack.back().getSuit());
    char LCSuit_In_ToStack = static_cast<char> (ALLStacks[StackToIndex].getID());

    //! Placement to the foundation.
    if (searchCardValue == LCValue_In_FromStack && LCSuit_In_FromStack == LCSuit_In_ToStack && StackToIndex >= spade_foundation_index) //Checks to see if the top of the foundation is 1 less than the moving card, and if the foundation is of the correct suit.
    {
        if (searchCardValue == 1 && LCValue_In_FromStack == LCValue_In_ToStack + 1) //Special case for aces.
        {
            ALLStacks[StackToIndex].cardsInStack.push_back(ALLStacks[StackFromIndex].cardsInStack.back());
            ALLStacks[StackFromIndex].cardsInStack.pop_back();
            this_move.move_type = 'n'; //Normal
        }
        else if (LCValue_In_FromStack == LCValue_In_ToStack + 1)
        {
            ALLStacks[StackToIndex].cardsInStack.push_back(ALLStacks[StackFromIndex].cardsInStack.back());
            ALLStacks[StackFromIndex].cardsInStack.pop_back();
            this_move.move_type = 'n';
        }
    }
    else if ((StackFromIndex >= spade_foundation_index || StackFromIndex == 7 && searchCardValue) && searchCardValue == LCValue_In_FromStack) //!Moving card from Foundation or Talon (only the last card will be moved).
    {
        if (LCValue_In_ToStack - 1 == LCValue_In_FromStack && ALLStacks[StackFromIndex].cardsInStack.back().getMovableColor() == ALLStacks[StackToIndex].cardsInStack.back().getThisColor())
        {
            ALLStacks[StackToIndex].cardsInStack.push_back(ALLStacks[StackFromIndex].cardsInStack.back());
            ALLStacks[StackFromIndex].cardsInStack.pop_back();
            this_move.move_type = 'n';
        }
        else if (LCValue_In_ToStack - 1 == LCValue_In_FromStack && ALLStacks[StackToIndex].cardsInStack.back().getThisColor() == 4)
        {
            ALLStacks[StackToIndex].cardsInStack.push_back(ALLStacks[StackFromIndex].cardsInStack.back());
            ALLStacks[StackFromIndex].cardsInStack.pop_back();
            this_move.move_type = 'n';
        }
    }
    else //!Moving Cards between piles in Tableau (meaning multiple cards can be moved at once).
    {
        if (StackFromIndex == 7)
            goto end_of_function; //You're not supposed to be in here!
        if (LCValue_In_ToStack - 1 == searchCardValue && ALLStacks[StackFromIndex].searchForCardInStack(searchCardValue).getMovableColor() == ALLStacks[StackToIndex].cardsInStack.back().getThisColor())
        {
            vector<Card> cardsInPlayerHand = ALLStacks[StackFromIndex].getCardsAfter(searchCardValue);
            for (int i = cardsInPlayerHand.size(); i > 0;  i--)
            {
                ALLStacks[StackToIndex].cardsInStack.push_back(cardsInPlayerHand.back());
                cardsInPlayerHand.pop_back();
                this_move.move_type = 'i'; //'i' for Interstack
            }
        }
        else if (LCValue_In_ToStack - 1 == searchCardValue && ALLStacks[StackToIndex].cardsInStack.back().getThisColor() == 4)
        {
            vector<Card> cardsInPlayerHand = ALLStacks[StackFromIndex].getCardsAfter(searchCardValue);
            for (int i = cardsInPlayerHand.size(); i > 0;  i--)
            {
                ALLStacks[StackToIndex].cardsInStack.push_back(cardsInPlayerHand.back());
                cardsInPlayerHand.pop_back();
                this_move.move_type = 'i';
            }
        }
    }

    if (ALLStacks[StackFromIndex].cardsInStack.back().getFlip() == 0)
    {
        ALLStacks[StackFromIndex].cardsInStack.back().FlipCard();
        this_move.do_a_flip = 't';
    }

    end_of_function:

    FullMoveList.push_back(this_move);
}


void ForceUndo(void)
{
    move_composition moveComp = FullMoveList.back();

    if (moveComp.move_type == 'U')
        goto end_of_function;

    if (moveComp.move_type == 'd')
    {
        if (ALLStacks[talon_pile_index].cardsInStack.size() == 0 && ALLStacks[draw_stock_index].cardsInStack.size() != 0) //Recycles Talon if there are no cards in the stock.
        {
            int staticInt = ALLStacks[draw_stock_index].cardsInStack.size();
            for (int i = 0; i < staticInt; i++)
            {
                ALLStacks[talon_pile_index].cardsInStack.push_back(ALLStacks[draw_stock_index].cardsInStack.back());
                ALLStacks[talon_pile_index].cardsInStack.back().FlipCard();
                ALLStacks[draw_stock_index].cardsInStack.pop_back();
            }
        }
        else
        {
            ALLStacks[draw_stock_index].cardsInStack.push_back(ALLStacks[talon_pile_index].cardsInStack.back());
            ALLStacks[draw_stock_index].cardsInStack.back().FlipCard();
            ALLStacks[talon_pile_index].cardsInStack.pop_back();
        }
    }

    if (moveComp.do_a_flip == 't')
        ALLStacks[moveComp.from_index].cardsInStack.back().FlipCard();

    if (moveComp.move_type == 'n')
    {
        ALLStacks[moveComp.from_index].cardsInStack.push_back(ALLStacks[moveComp.to_index].cardsInStack.back());
        ALLStacks[moveComp.to_index].cardsInStack.pop_back();
    }
    if (moveComp.move_type == 'i')
    {
        vector<Card> cardsInPlayerHand = ALLStacks[moveComp.to_index].getCardsAfter(moveComp.card_value);
        for (int i = cardsInPlayerHand.size(); i > 0;  i--)
        {
            ALLStacks[moveComp.from_index].cardsInStack.push_back(cardsInPlayerHand.back());
            cardsInPlayerHand.pop_back();
        }
    }

    end_of_function:

    FullMoveList.pop_back();
}

char PlayerControlHandeler(string full_player_control_string)
{
    string external_from_stack;
    string external_moving_card;
    string external_to_stack;

    istringstream control_interpreter(full_player_control_string);
    control_interpreter >> external_moving_card;
    control_interpreter >> external_from_stack;
    control_interpreter >> external_to_stack;

    stackindex local_from_stack;
    stackindex local_to_stack;
    int local_moving_card_value;
    char returnChar = 'T'; //Default return is T for "True", meaning the function executed successfully

    if (external_moving_card == "quit") //Translates external_from
    {
        returnChar = 'q';
        goto end_of_function;
    }
    else if (external_moving_card == "help")
    {
        returnChar = 'c';
        goto end_of_function;
    }
    else if (external_moving_card == "refresh")
    {
        returnChar = 'r';
        goto end_of_function;
    }
    else if (external_moving_card == "d")
    {
        returnChar = 'd';
        goto end_of_function;
    }
    else if (external_moving_card == "u")
    {
        returnChar = 'u';
        goto end_of_function;
    }
    else if (external_moving_card == "a")
        local_moving_card_value = 1;
    else if (external_moving_card == "2")
        local_moving_card_value = 2;
    else if (external_moving_card == "3")
        local_moving_card_value = 3;
    else if (external_moving_card == "4")
        local_moving_card_value = 4;
    else if (external_moving_card == "5")
        local_moving_card_value = 5;
    else if (external_moving_card == "6")
        local_moving_card_value = 6;
    else if (external_moving_card == "7")
        local_moving_card_value = 7;
    else if (external_moving_card == "8")
        local_moving_card_value = 8;
    else if (external_moving_card == "9")
        local_moving_card_value = 9;
    else if (external_moving_card == "10")
        local_moving_card_value = 10;
    else if (external_moving_card == "j")
        local_moving_card_value = 11;
    else if (external_moving_card == "q")
        local_moving_card_value = 12;
    else if (external_moving_card == "k")
        local_moving_card_value = 13;
    else
    {
        returnChar = 'e';
        goto end_of_function;
    }

    if (external_from_stack == "t")
        local_from_stack = talon_pile_index;
    else if (external_from_stack == "sf")
        local_from_stack = spade_foundation_index;
    else if (external_from_stack == "hf")
        local_from_stack = heart_foundation_index;
    else if (external_from_stack == "cf")
        local_from_stack = club_foundation_index;
    else if (external_from_stack == "df")
        local_from_stack = diamond_foundation_index;
    else if (external_from_stack == "1")
        local_from_stack = pile_1_index;
    else if (external_from_stack == "2")
        local_from_stack = pile_2_index;
    else if (external_from_stack == "3")
        local_from_stack = pile_3_index;
    else if (external_from_stack == "4")
        local_from_stack = pile_4_index;
    else if (external_from_stack == "5")
        local_from_stack = pile_5_index;
    else if (external_from_stack == "6")
        local_from_stack = pile_6_index;
    else if (external_from_stack == "7")
        local_from_stack = pile_7_index;
    else
    {
        returnChar = 'e';
        goto end_of_function;
    }

    if (external_to_stack == "sf") //Translates external_to
        local_to_stack = spade_foundation_index;
    else if (external_to_stack == "hf")
        local_to_stack = heart_foundation_index;
    else if (external_to_stack == "cf")
        local_to_stack = club_foundation_index;
    else if (external_to_stack == "df")
        local_to_stack = diamond_foundation_index;
    else if (external_to_stack == "1")
        local_to_stack = pile_1_index;
    else if (external_to_stack == "2")
        local_to_stack = pile_2_index;
    else if (external_to_stack == "3")
        local_to_stack = pile_3_index;
    else if (external_to_stack == "4")
        local_to_stack = pile_4_index;
    else if (external_to_stack == "5")
        local_to_stack = pile_5_index;
    else if (external_to_stack == "6")
        local_to_stack = pile_6_index;
    else if (external_to_stack == "7")
        local_to_stack = pile_7_index;
    else
    {
        returnChar = 'e';
        goto end_of_function;
    }

    //!Handle card movement between stacks
    MoveCard(local_moving_card_value, local_from_stack, local_to_stack);

    end_of_function:
    return returnChar;
}

void Complete_Game(int moves)
{
    system("CLS");
    cout << "CONGRATULATIONS!!" << endl << "You won in " << moves << " moves.";
}

//!==================================================================================!//
class Game
{
private:
    int number_of_moves;
public:
    Game();
    void game_setup(void);
    void start_game(void);
};

Game::Game()
{
    system("Color F0");

    vector<Card> tempDeck = {};
    for (int i = 1; i < 14; i++) //Creates cards for all suits and all values (that are not already created).
    {
        for (int j = 0; j < 4; j++)
        {
            Card tempCard(i, ALL_SUITS[j]);
            tempDeck.push_back(tempCard);
        }
    }
    random_shuffle(tempDeck.begin(), tempDeck.end());

    int k = 0;
    for (int i = 0; i < 12; i++) //Places all cards in the deck onto the Playspace as a test image.
    {
        for (int j = 0; j < 4; j++)
        {
            MyPlayspace->place_card(tempDeck[k], 4*i+1, 7+(12*j));
            k++;
        }
    }
    for (int j = 0; j < 4; j++) //Places kings 1 space higher to not overlap the border.
    {
        MyPlayspace->place_card(tempDeck[k], 4*12, 7+(12*j));
        k++;
    }


    char settings_input = 'n';
    cout << black_c << "Please adjust this window to your preference before continuing. (Press the \"Enter\" key on your keyboard to continue)" << endl;
    getchar();
    cout << "Please enter your refresh type ('c' for clear [default], 's' for scroll, 't' to test the refresh, or 'x' to continue)." << endl << "Make sure you are able to see the entire field each time the screen is refreshed." << endl;
    while (settings_input != 'x')//Gets the refresh type from the user until they are ready to continue.
    {
        cin >> settings_input;
        if (settings_input == 'x' || settings_input == 'c' || settings_input == 's' || settings_input == 't')
        {
        switch(settings_input)
            {
                case 'c': MyPlayspace->set_scroll_type(settings_input);
                        cout << "Your refresh setting has been set to " << settings_input << ". " << endl << "You can choose to change your refresh type again or continue." << endl << "Please enter 'c' for clear, 's' for scroll, 't' to test the refresh, or 'x' to continue." << endl;
                        break;
                case 's': MyPlayspace->set_scroll_type(settings_input);
                        cout << "Your refresh setting has been set to " << settings_input << ". "  << endl << "You can choose to change your refresh type again or continue." << endl << "Please enter 'c' for clear, 's' for scroll, 't' to test the refresh, or 'x' to continue." << endl;
                        break;
                case 't': MyPlayspace->display_space();
                        cout << "You can choose to change your refresh type again or continue." << endl << "Please enter 'c' for clear, 's' for scroll, 't' to test the refresh, or 'x' to continue." << endl;
                        break;
            }
        }
        else //Fires on invalid refresh types.
        {
            cout << "That is an invalid refresh type." << endl << "You can choose to change your refresh type again or continue." << endl << "Please enter 'c' for clear, 's' for scroll, 't' to test the refresh, or 'x' to continue." << endl;
        }
    }

    system("CLS");

    cout << endl << "Would you like to know the rules and controls of the game? (enter 'y' for yes or 'n' for no): ";
    char explain_rules;
    cin >> explain_rules;
    if (explain_rules == 'y') //Explains the rules of the game to the player if they specify that they want to see the rules
    {
        explainRulesFunction();
        cout << endl;
        explainControlsFunction();
    }

    cout << endl << "Press the \"Enter\" key on your keyboard to start the game. Remember that at any point during the game you can enter \"help\" to see the rules and controls." << endl << "Have fun :D!" << endl;
    getchar();
    getchar();
}

void Game::game_setup(void)
{
    number_of_moves = 0; //Initializes the number of moves.

    for (int i = 1; i <= 13; i++) //!Original set of 52 cards are created and placed into original_deck.
    {
        for (int j = 0; j < 4; j++)
        {
            Card tempCard(i, ALL_SUITS[j]);
            original_deck.push_back(tempCard);
        }
    }
    random_shuffle(original_deck.begin(), original_deck.end()); //!Deck is shuffled.
    original_deck = flipDeck(original_deck); //!All cards in the deck are flipped.

    for (int i = 0; i < 13; i++) //!All Stacks are created and put into the vector "ALLStacks".
    {
        FieldStack tempStack(ALL_STACKTYPES[i]);
        ALLStacks.push_back(tempStack);
    }

    for (int i = 0; i < 7; i++) //!Stacks in Tableau have their cards.
    {
        for (int j = 1; j < i+2; j++) // j<i+2 ensures each card gets its proper amount of cards.
        {
            ALLStacks[i].pushCard(original_deck.back());
            original_deck.pop_back();
        }
        ALLStacks[i].cardsInStack.back().FlipCard();
    }

    int cards_left = original_deck.size();
    for (int i = 0; i < cards_left; i++) //!Cards that were not put into the Tableau were put into the Stock.
    {
        ALLStacks[draw_stock_index].pushCard(original_deck.back());
        original_deck.pop_back();
    }
}

void Game::start_game(void)
{
    string player_control = "initialize";
    char move_status;

    while (player_control != "quit")
    {
        MyPlayspace->full_refresh_space(ALLStacks);

        new_control_without_refresh:
        cout << "Enter move: ";
        getline(cin, player_control);

        move_status = PlayerControlHandeler(player_control);
        if (move_status == 'c')
        {
            explainRulesFunction();
            cout << endl;
            explainControlsFunction();
            goto new_control_without_refresh;
        }
        else if (move_status == 'e')
        {
            cout << "That is an invalid control statement." << endl;
            goto new_control_without_refresh;
        }
        else if (move_status == 'd')
        {
            if (DrawCard())
            {
                number_of_moves++;
                move_composition this_move;
                this_move.move_type = 'd';
                FullMoveList.push_back(this_move);
            }
        }
        else if (move_status == 'u' && FullMoveList.size() == 0)
        {
            cout << "No moves to undo" << endl;
            goto new_control_without_refresh;
        }
        else if (move_status == 'u' && FullMoveList.size() != 0)
        {
            ForceUndo();
            number_of_moves++;
        }
        else if (move_status == 'T')
        {
            number_of_moves++;
        }

        if (ALLStacks[spade_foundation_index].cardsInStack.size() == 14 && ALLStacks[heart_foundation_index].cardsInStack.size() == 14 && ALLStacks[club_foundation_index].cardsInStack.size() == 14 && ALLStacks[diamond_foundation_index].cardsInStack.size() == 14)
        {
            Complete_Game(number_of_moves);
            goto end_of_function;
        }
    }
    end_of_function: {}
}

int main()
{
system("mode 200");

cout << "This program allows the user to play a game of solitaire." << endl;

srand(time(0));
//srand(0); //!This is a test line that has a set start which is already routed out.
system("Color F0"); //Sets "hard" console colors to black text on a white background.

Game Solitaire;
Solitaire.game_setup();
Solitaire.start_game();

cout << endl << "Press the enter key on your keyboard a few times to close the game.";
getchar();
getchar();
return 0;
}
