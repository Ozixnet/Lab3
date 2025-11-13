#include "Magic/Spells/DirectDamage.h"
#include "Magic/ISpellContext.h"
#include "Magic/SpellParams.h"
#include "Magic/SpellBuffContext.h"
#include <iostream>

DirectDamage::DirectDamage(int dmg, int rad)
    : damage(dmg), radius(rad), autoAim(false), targetX(0), targetY(0) {}

DirectDamage::DirectDamage(int dmg, int rad, bool autoAim, int tx, int ty)
    : damage(dmg), radius(rad), autoAim(autoAim), targetX(tx), targetY(ty) {}

bool DirectDamage::use(ISpellContext& context) {
    // Если авто-таргет (для башен) - сразу атакуем цель (без баффов)
    if (autoAim) {
        if (context.dealDamage(targetX, targetY, damage)) {
            std::cout << "Башня атакует цель на (" << targetX << ", " << targetY 
                      << ") уроном " << damage << "!\n";
            return true;
        }
        return false;
    }
    
    // Создаем параметры и применяем баффы
    DirectDamageParams params{damage, radius};
    context.getBuffContext().applyAndConsumeFor(params);
    
    int finalDamage = params.damage;
    int finalRadius = params.radius;
    
    // Получаем координаты игрока
    auto [playerX, playerY] = context.getPlayerPosition();
    
    // Находим все цели в радиусе через контекст
    auto targets = context.findTargetsInRadius(playerX, playerY, finalRadius);
    
    if (targets.empty()) {
        std::cout << "В радиусе действия (" << finalRadius << ") нет целей!" << std::endl;
        return false;
    }
    
    std::cout << "\n=== Заклинание Direct Damage ===" << std::endl;
    std::cout << "Урон: " << finalDamage << ", Радиус: " << finalRadius << std::endl;
    
    // Запрашиваем выбор цели через контекст
    int choice = context.requestTargetSelection(targets);
    
    if (choice < 0 || choice >= static_cast<int>(targets.size())) {
        std::cout << "Заклинание отменено." << std::endl;
        return false;
    }
    
    const auto& selectedTarget = targets[choice];
    
    std::cout << "Атакуем " << selectedTarget.displayName 
              << " на (" << selectedTarget.x << ", " << selectedTarget.y 
              << ") уроном " << finalDamage << "!" << std::endl;
    
    return context.dealDamageToTarget(selectedTarget, finalDamage);
}

const char* DirectDamage::getName() const {
    return "Direct Damage";
}
