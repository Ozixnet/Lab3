#include "Magic/Spells/SummonSpell.h"
#include "Entity/EntityManager.h"
#include "Magic/SpellParams.h"
#include <iostream>

SummonSpell::SummonSpell(int baseCount, int allyHp, int allyDmg)
    : baseCount(baseCount), allyHp(allyHp), allyDmg(allyDmg) {}

bool SummonSpell::use(EntityManager& entityManager, int gridSize) {
    try {
        auto [playerX, playerY] = entityManager.getPlayerCoord();
        
        // Создаем параметры
        SummonParams params{baseCount, allyHp, allyDmg};
        
        // Применяем баффы
        entityManager.getBuffContext().applyAndConsumeFor(params);
        
        std::cout << "\n=== Заклинание Summon Spell ===" << std::endl;
        std::cout << "Призываем " << params.count << " союзников..." << std::endl;
        
        int summonedCount = 0;
        
        // Пытаемся призвать params.count союзников
        for (int i = 0; i < params.count; ++i) {
            auto [freeX, freeY] = entityManager.findFreeAdjacentCell(playerX, playerY);
            
            if (freeX >= 0 && freeY >= 0) {
                entityManager.addAlly(freeX, freeY, params.allyHp, params.allyDmg);
                summonedCount++;
            } else {
                std::cout << "Не удалось найти свободную клетку для союзника #" << (i + 1) << std::endl;
            }
        }
        
        if (summonedCount > 0) {
            std::cout << "✓ Призвано союзников: " << summonedCount << std::endl;
            return true;
        } else {
            std::cout << "✗ Не удалось призвать ни одного союзника (нет свободных клеток)" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in SummonSpell::use: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error in SummonSpell::use" << std::endl;
        return false;
    }
}

