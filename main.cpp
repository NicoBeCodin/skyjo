#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

/* TODO
/ Initial Card for players isnt random
/ Column discarding managment + how to replace the empty column
/ Let player with biggest two cards start
/ EndGame final round
/ Point Calculation

*/
using namespace std;

class Card
{
public:
    int value;
    bool faceUp;
    bool discarded;
    Card(int v) : value(v), faceUp(false), discarded(false) {}
};

class Player
{
public:
    vector<vector<Card>> grid; // 3x4 grid of cards
    int score;

    Player() : score(0)
    {
        grid.resize(3, vector<Card>(4, Card(0)));
    }

    void displayGrid()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (grid[i][j].faceUp && !grid[i][j].discarded)
                {
                    cout << grid[i][j].value << "\t";
                }
                else if (grid[i][j].discarded)
                {
                    cout << " \t"; // discarded row
                }
                else
                {
                    cout << "X\t"; // Hidden card
                }
            }
            cout << endl;
        }
    }

    int calculateScore()
    {
        int total = 0;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (!grid[i][j].discarded)
                {
                    total += grid[i][j].value;
                }
            }
        }
        return total;
    }
};

vector<int> cardsDistribution = {5, 10, 15, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
class Deck
{
public:
    vector<int> cards;
    vector<int> discardPile;

    Deck()
    {
        // Create deck with values -2 to 12
        for (int i = -2; i <= 12; ++i)
        {
            for (int j = 0; j < cardsDistribution[i + 2]; ++j)
            { // Assume there are multiple copies of each card
                cards.push_back(i);
            }
        }
        shuffleDeck();
        discardCard(drawCard());
    }

    void shuffleDeck()
    {
        srand(time(0));
        random_shuffle(cards.begin(), cards.end());
    }

    int drawCard()
    {
        if (cards.empty())
        {
            // Reshuffle discard pile if the deck is empty
            cards = discardPile;
            discardPile.clear();
            shuffleDeck();
        }
        int card = cards.back();
        cards.pop_back();
        return card;
    }

    void discardCard(int card)
    {
        discardPile.push_back(card);
    }

    int peekDiscard()
    {
        if (!discardPile.empty())
        {
            return discardPile.back();
        }
        return -99; // Indicates no cards in discard
    }
};

class Game
{
public:
    Deck deck;
    vector<Player> players;
    vector<int> finalScores;
    int currentPlayer;
    bool lastTurn;
    int endPlayer;
    bool gameRunning;
    

    Game(int numPlayers)
    {
        int max = -10;
        int firstPlayer =0;
        gameRunning =true;
        for (int i = 0; i < numPlayers; ++i)
        {
            players.push_back(Player());
            setupPlayer(players[i]);
            int initScore = players[i].grid[0][1].value + players[i].grid[0][0].value;
            if (initScore > max){
                max = initScore;
                firstPlayer =i;
            } 

        }
        currentPlayer = firstPlayer;

    }

    void setupPlayer(Player &player)
    {
        // Initialize player grid with face-down cards and two face-up cards
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                player.grid[i][j] = Card(deck.drawCard());
            }
        }
        // Flip two random cards face up
        player.grid[0][0].faceUp = true;
        player.grid[0][1].faceUp = true;
    }

    void playTurn()
    {
        Player &player = players[currentPlayer];
        cout << "Player " << currentPlayer + 1 << "'s turn:\n";
        player.displayGrid();
        printf("Pile card: %d\n", deck.peekDiscard());
        int drawChoice;
        cout << "Draw card from (1) Deck or (2) Discard pile: ";
        cin >> drawChoice;

        int drawnCard;
        if (drawChoice == 1)
        {
            drawnCard = deck.drawCard();
            cout << "You drew a " << drawnCard << endl;
        }
        else
        {
            drawnCard = deck.peekDiscard();
            cout << "You took the discard: " << drawnCard << endl;
            deck.discardPile.pop_back();
        }

        if (drawChoice == 1)
        {
            cout << "Replace a card (row and column) or flip a face-down card? (1: Replace, 2: Flip): ";
            int actionChoice;
            cin >> actionChoice;

            if (actionChoice == 1)
            {
                int row, col;
                cout << "Enter row and column to replace (0-2 for row, 0-3 for column): " << endl;
                cin >> row >> col;
                // REPAIR INPUT FUNCTION
                printf("Row: %d Col: %d\n", row, col);

                if (row > 2 || row < 0 || col > 3 || col < 0)
                {
                    printf("Wrong value!");
                }

                int oldCard = player.grid[row][col].value;
                player.grid[row][col] = Card(drawnCard);
                player.grid[row][col].faceUp = true;
                deck.discardCard(oldCard);
                printf("You discarded a %d\n", oldCard);
            }
            else if (actionChoice == 2)
            {
                int row, col;

                cout << "Enter row and column to flip (0-2 for row, 0-3 for column): " << endl;
                cin >> row >> col;
                if (row > 2 || row < 0 || col > 3 || col < 0)
                {
                    printf("Wrong value!");
                }

                player.grid[row][col].faceUp = true;
                deck.discardCard(drawnCard);
                printf("You flipped a %d\n", player.grid[row][col].value);
            }
            else
            {
                printf("Wrong number inputed!");
                // ERROR MANAGMENT CODE
            }
        }
        else {
            int row;
            int col;
            cout << "Enter row and column to replace (0-2 for row, 0-3 for column): " << endl;
            cin >> row >> col;

            int oldCard = player.grid[row][col].value;
            deck.discardCard(oldCard);
            player.grid[row][col] = drawnCard;
            printf("You discarded a %d\n", oldCard);

        }

        // Checks if any column was made
        checkColumn(player);

        //If one player is finished, last turn starts
        if (!lastTurn){
        lastTurn = checkFinished(player);
        
        }
        else {
            //reveal all cards after action on lastTurn
            lastTurnReveal(player);
            printf("Final grid is : \n");
            player.displayGrid();
        }
        

        // End turn and check if the game should end
        if (checkEndCondition())
        {
            endGame();
        }
        else
        {
            nextPlayer();
        }
    }

    void nextPlayer()
    {
        currentPlayer = (currentPlayer + 1) % players.size();
    }
    void checkColumn(Player player)
    {
        // Check if column was made and discard it

        for (int j = 0; j < 4; ++j)
        {
            Card col = player.grid[0][j];
            if (col.faceUp && player.grid[1][j].faceUp && player.grid[2][j].faceUp)
            {
                if (col.value == player.grid[1][j].value && col.value == player.grid[2][j].value)
                {
                    printf("Column made...\n");
                    player.grid[0][j].discarded = true;
                    player.grid[1][j].discarded = true;
                    player.grid[2][j].discarded = true;

                    for (int k = 0; k < 3; ++k)
                    {
                        deck.discardCard(player.grid[0][j].value);
                    }
                }
            }
        }
    }

    bool checkFinished(Player player){
        for (int i =0; i< 3; ++i){
            for(int j =0; j<4; ++j){
                if (!player.grid[i][j].faceUp) return false;
            }
        }
        //Keep track of who is the game ender since score can be x2 if another player is equal or superior
        endPlayer = currentPlayer;
        return true;
    }

    bool lastTurnReveal(Player player){
        for(int i=0; i<3; ++i){
            for (int j; j<4; ++j){
                player.grid[i][j].faceUp =true;
            }
        }
    }

    bool checkEndCondition()
    {
        // Check if any player has revealed all cards
        for (Player &player : players)
        {
            bool allRevealed = true;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    if (!player.grid[i][j].faceUp)
                    {
                        allRevealed = false;
                    }
                }
            }
            if (allRevealed)
            {
                return true;
            }
        }
        return false;
    }

    void endGame()
    {
        cout << "Game over! Scores:\n";
        for (int i = 0; i < players.size(); ++i)
        {
            players[i].score = players[i].calculateScore();

        }
        // Determine the winner

        //Skyjo rule to punish the end player if he is beaten
        for (int k =0; k<players.size(); ++k){
            if (players[endPlayer].score >= players[k].score && k != endPlayer){
                printf("Ending player was beaten!\n");

                if (players[endPlayer].score < 0) {
                    players[endPlayer].score = 0;
                }
                else {
                    players[endPlayer].score = players[endPlayer].score * 2;
                }
            }

        }

        for (int i =0; i<players.size(); ++i){
            printf("Final score: ");
            finalScores[i] = players[i].score;
            cout << "Player " << i + 1 << ": " << players[i].score << " points\n";

        }

        int minScore = players[0].score;
        int winner = 0;
        for (int i = 1; i < players.size(); ++i)
        {
            if (players[i].score < minScore)
            {
                minScore = players[i].score;
                winner = i;
            }
        }


        cout << "Player " << winner + 1 << " wins!\n";
        gameRunning =false;

    }
};

int main()
{
    int numPlayers;
    cout << "Enter number of players: ";
    cin >> numPlayers;

    Game game(numPlayers);

    while (game.gameRunning)
    {
        game.playTurn();
    }
    

    return 0;
}
