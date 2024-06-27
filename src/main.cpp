#include "shatranj.h"
#include "types.h"

int main()
{
    shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
    shatranj.Run();
    return 0;
}
