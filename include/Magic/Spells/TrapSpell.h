#ifndef TRAP_SPELL_H
#define TRAP_SPELL_H

#include "../SpellCard.h"
#include <vector>
#include <memory>

class Trap;
class EntityManager;

class TrapSpell : public SpellCard {
private:
    int damage;
    int radius;
    int maxTraps;  // Максимум ловушек на поле

public:
    TrapSpell(int dmg = 5, int rad = 5, int maxTrp = 3);
    
    bool use(EntityManager& entityManager, int gridSize) override;
    const char* getName() const override { return "Trap Spell"; }
    
    // Добавить ловушку
    static void addTrap(int x, int y, int damage);
    
    // Получить все ловушки
    static const std::vector<std::unique_ptr<Trap>>& getTraps();
    
    // Очистить ловушки
    static void clearTraps();
    
    // Проверить триггер ловушки и вернуть урон
    static int checkTrapAt(int x, int y);

private:
    // Статические ловушки на поле
    static std::vector<std::unique_ptr<Trap>> trapsOnField;
    static int trapCount;
};

#endif // TRAP_SPELL_H
