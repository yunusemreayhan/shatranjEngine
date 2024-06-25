#include "shatranj.h"
#include "types.h"

int main()
{
    shatranj::Shatranj shatranj(std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
                                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
    shatranj.Run();
    return 0;
}
