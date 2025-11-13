#ifdef _WIN32

#include "core/GameControl.h"

// Включить полные определения для корректной работы unique_ptr в деструкторе
#include "Entity/Player/Player.h"
#include "Board/Board.h"
#include "Magic/Hand.h"

int main() {
    GameControl control;
    control.gameStart();
    return 0;
}
#endif
