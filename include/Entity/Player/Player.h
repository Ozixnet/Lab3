#ifndef PLAYER_H
#define PLAYER_H

class Player {

public:
    Player(int h, int d);

    void displayInfo() const;


    // Гетеры
    int GetHealth()   const { return health;   }
    int GetDamage()   const { return damage;   }
    int GetDistance() const { return distance; }
    bool IsSlowed()   const { return slowed;   }


    // Сетеры
    void ReduceHealth(int amount){health-= amount;}
    void ToggleDoubleDistance() {
        DoubleDistance = !DoubleDistance;
        distance = DoubleDistance ? 2: 1;
        damage = DoubleDistance ? 1 : 2;
    }

    void SetSlowed(bool s)       { slowed = s;}


private:
    int health;
    int damage;
    int distance = 1;
    bool slowed = false;
    bool DoubleDistance = false;
};

#endif // PLAYER_H
