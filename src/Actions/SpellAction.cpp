#include "Actions/SpellAction.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Events/EventBus.h"
#include "Events/GameEvent.h"
#include "Magic/GameSpellContext.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include <iostream>

SpellAction::SpellAction(SpellCard* s, EntityManager* em, Hand* h, int gs, int idx)
    : spell(s), entityManager(em), hand(h), gridSize(gs), spellIndex(idx) {}

int SpellAction::execute(Board& board, Player& player) {
    if (!spell || !entityManager) {
        std::cout << "Ошибка: заклинание недоступно!\n";
        return 2;  // Действие заблокировано
    }

    // Создаем контекст для заклинания
    GameSpellContext context(*entityManager, board, gridSize);
    
    // Используем заклинание через контекст
    bool used = spell->use(context);

    if (used) {
        std::cout << "Заклинание " << spell->getName() << " успешно использовано!\n";
        auto [x, y] = entityManager->getPlayerCoord();
        EventBus::getInstance().publish(SpellCastEvent(spell->getName(), x, y));

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
