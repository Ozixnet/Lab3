#ifndef SPELL_CARD_H
#define SPELL_CARD_H

class ISpellContext;

/**
 * @brief Базовый класс для всех заклинаний
 * 
 * Заклинания теперь работают через интерфейс ISpellContext,
 * а не напрямую с EntityManager. Это обеспечивает инкапсуляцию
 * и упрощает тестирование.
 */
class SpellCard {
public:
    virtual ~SpellCard() = default;

    /**
     * @brief Использовать заклинание
     * @param context Контекст заклинания (интерфейс для взаимодействия с игровым миром)
     * @return true если заклинание было успешно использовано, false в противном случае
     */
    virtual bool use(ISpellContext& context) = 0;
    
    virtual const char* getName() const = 0;
};

#endif // SPELL_CARD_H
