#include "Actions/ToggleRangeAction.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include <iostream>

int ToggleRangeAction::execute(Board& board, Player& player) {
    player.ToggleDoubleDistance();
    std::cout << "Дальняя атака переключена! Дистанция: "
              << player.GetDistance() << "\n";
    return -1;  // Не завершает ход
}
