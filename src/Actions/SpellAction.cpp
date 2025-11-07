#include "Actions/SpellAction.h"
#include "Magic/SpellCard.h"
#include "Magic/Hand.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include <iostream>

SpellAction::SpellAction(SpellCard* s, EntityManager* em, Hand* h, int gs, int idx)
    : spell(s), entityManager(em), hand(h), gridSize(gs), spellIndex(idx) {}

int SpellAction::execute(Board& board, Player& player) {
    if (!spell || !entityManager) {
        std::cout << "Ошибка: заклинание недоступно!\n";
        return 2;  // Действие заблокировано
    }

    bool used = spell->use(*entityManager, gridSize);

    if (used) {
        std::cout << "Заклинание " << spell->getName() << " успешно использовано!\n";

        // Удаляем заклинание из руки после успешного использования
        if (hand && spellIndex >= 0 && spellIndex < static_cast<int>(hand->size())) {
            hand->removeSpell(spellIndex);
            std::cout << "Заклинание удалено из руки.\n";
        }

        // После заклинания двигаем врагов
        board.getEntityManager().moveAllEnemies();
        board.getEntityManager().processBuildingSpawns();

        return 0;  // Успешное действие, завершаем ход
    } else {
        std::cout << "Заклинание не было использовано.\n";
        return 2;  // Действие заблокировано
    }
}
