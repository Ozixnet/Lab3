#ifndef ISPELL_CONTEXT_H
#define ISPELL_CONTEXT_H

#include <vector>
#include <utility>
#include <string>

// Информация о цели для заклинаний
struct TargetInfo {
    int x;
    int y;
    enum class Type { Enemy, Building, Tower, Ally } type;
    int index;  // Индекс в соответствующем массиве
    int health;
    std::string displayName;
};

// Forward declarations
struct DirectDamageParams;
struct AoEParams;
struct TrapParams;
struct SummonParams;
class SpellBuffContext;

/**
 * @brief Интерфейс для взаимодействия заклинаний с игровым миром
 * 
 * Этот интерфейс обеспечивает инкапсуляцию - заклинания не знают
 * о внутреннем устройстве EntityManager, Board, Grid и других классов.
 * Они работают только через этот минимальный интерфейс.
 */
class ISpellContext {
public:
    virtual ~ISpellContext() = default;
    
    // === ПОЛУЧЕНИЕ ИНФОРМАЦИИ ===
    
    /// Получить координаты игрока
    virtual std::pair<int, int> getPlayerPosition() const = 0;
    
    /// Получить размер сетки
    virtual int getGridSize() const = 0;
    
    /// Найти все цели в заданном радиусе от точки
    virtual std::vector<TargetInfo> findTargetsInRadius(int centerX, int centerY, int radius) const = 0;
    
    /// Проверить, свободна ли клетка (можно ли поставить ловушку/призвать союзника)
    virtual bool isCellEmpty(int x, int y) const = 0;
    
    /// Проверить, есть ли уже ловушка на этой клетке
    virtual bool hasTrapAt(int x, int y) const = 0;
    
    // === ДЕЙСТВИЯ ===
    
    /// Нанести урон по цели в указанных координатах
    virtual bool dealDamage(int x, int y, int damage) = 0;
    
    /// Нанести урон конкретной цели (по TargetInfo)
    virtual bool dealDamageToTarget(const TargetInfo& target, int damage) = 0;
    
    /// Разместить ловушку
    virtual bool placeTrap(int x, int y, int damage) = 0;
    
    /// Призвать союзника
    virtual bool summonAlly(int x, int y, int health, int damage) = 0;
    
    /// Найти свободную клетку рядом с указанной позицией
    virtual std::pair<int, int> findFreeAdjacentCell(int centerX, int centerY) const = 0;
    
    // === БАФФЫ ===
    
    /// Получить контекст баффов для заклинаний
    virtual SpellBuffContext& getBuffContext() = 0;
    virtual const SpellBuffContext& getBuffContext() const = 0;
    
    // === ВВОД/ВЫВОД ===
    
    /// Запросить у пользователя выбор цели из списка
    virtual int requestTargetSelection(const std::vector<TargetInfo>& targets) const = 0;
    
    /// Запросить координаты у пользователя
    virtual std::pair<int, int> requestCoordinates(const std::string& prompt) const = 0;
};

#endif // ISPELL_CONTEXT_H

