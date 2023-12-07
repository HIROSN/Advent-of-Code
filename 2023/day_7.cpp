#include <main.h>

#include <algorithm>
#include <unordered_map>
#include <vector>

const std::string camel_cards{
    '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Card
    {
        char letter;

        Card(char ch) : letter(ch) {}

        bool operator<(const Card &right) const
        {
            return camel_cards.find(letter) < camel_cards.find(right.letter);
        }

        bool operator==(const Card &right) const
        {
            return camel_cards.find(letter) == camel_cards.find(right.letter);
        }
    };

    struct Type
    {
        int stack_size = 0;
        int highest_stack = 0;

        bool operator<(const Type &right) const
        {
            if (highest_stack < right.highest_stack)
                return true;

            if (highest_stack == right.highest_stack)
                return stack_size > right.stack_size;

            return false;
        }

        bool operator==(const Type &right) const
        {
            return stack_size == right.stack_size &&
                   highest_stack == right.highest_stack;
        }
    };

    struct Hand
    {
        std::vector<Card> cards;
        Type type;
        int bid;

        Hand(const std::string &str, int i) : bid(i)
        {
            std::unordered_map<char, int> stack;
            for (const auto &ch : str)
            {
                cards.push_back(Card(ch));
                stack[ch]++;
            }

            type.stack_size = stack.size();
            for (auto it : stack)
            {
                const int height = it.second;
                type.highest_stack = std::max({type.highest_stack, height});
            }
        }

        bool operator<(const Hand &right) const
        {
            if (type < right.type)
                return true;

            if (type == right.type)
            {
                for (int i = 0; i < cards.size(); i++)
                {
                    if (cards[i] < right.cards[i])
                        return true;

                    if (cards[i] == right.cards[i])
                        continue;

                    break;
                }
            }
            return false;
        }
    };

    std::string cards;
    int bid;
    std::vector<Hand> hands;

    while (file >> cards >> bid)
        hands.push_back(Hand(cards, bid));
    std::sort(hands.begin(), hands.end());

    int rank = 0;
    uint64_t total_winnings = 0;

    for (const auto &hand : hands)
        total_winnings += hand.bid * ++rank;

    return total_winnings;
}
