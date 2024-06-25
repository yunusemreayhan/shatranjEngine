#include "shatranj.h"
#include "types.h"

int main()
{
    shatranj::Shatranj shatranj(std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kBlack),
                                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kWhite));
    shatranj.Run();
    return 0;
}
