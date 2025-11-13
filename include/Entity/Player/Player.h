#ifndef PLAYER_H
#define PLAYER_H

class Player {

public:
    Player(int h, int d);

    void displayInfo() const;


    // Гетеры
    int GetHealth()   const { return health;   }
    int GetMaxHealth() const { return maxHealth; }
    int GetDamage()   const { return damage;   }
    int GetBaseDamage() const { return baseDamage; }
    int GetDistance() const { return distance; }
    bool IsSlowed()   const { return slowed;   }
    bool IsDoubleDistance() const { return DoubleDistance; }
    int GetUpgradePoints() const { return upgradePoints; }


    // Сетеры
    void ReduceHealth(int amount){health-= amount;}
    void RestoreHealth() { health = maxHealth; }  // Восстановить HP до максимума
    void ToggleDoubleDistance() {
        DoubleDistance = !DoubleDistance;
        distance = DoubleDistance ? 2: 1;
        // При дальнем бое урон уменьшается в 2 раза, при ближнем - исходный
        damage = DoubleDistance ? baseDamage / 2 : baseDamage;
    }

    void SetSlowed(bool s)       { slowed = s;}
    
    // Методы для загрузки сохранения
    void SetHealth(int h) { health = h; }
    void SetBaseDamage(int d) { baseDamage = d; damage = DoubleDistance ? baseDamage / 2 : baseDamage; }
    void SetDistance(int dist) { distance = dist; DoubleDistance = (dist == 2); }
    void SetUpgradePoints(int points) { upgradePoints = points; }
    
    // Методы прокачки
    void AddUpgradePoints(int points) { upgradePoints += points; }
    bool SpendUpgradePoint() { 
        if (upgradePoints > 0) { 
            upgradePoints--; 
            return true; 
        } 
        return false; 
    }
    
    // Улучшения характеристик
    void UpgradeMaxHealth(int amount) { maxHealth += amount; health += amount; }
    void UpgradeBaseDamage(int amount) { 
        baseDamage += amount; 
        damage = DoubleDistance ? baseDamage / 2 : baseDamage;
    }


private:
    int health;
    int maxHealth;
    int damage;
    int baseDamage;  // Базовый урон (сохраняем исходное значение)
    int distance = 1;
    bool slowed = false;
    bool DoubleDistance = false;
    int upgradePoints = 0;  // Очки прокачки
};

#endif // PLAYER_H
