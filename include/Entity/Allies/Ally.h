#ifndef ALLY_H
#define ALLY_H

class Ally {
public:
    Ally(int h, int d, int id);

    int GetHealth() const { return health; }
    int GetDamage() const { return damage; }

    void ReduceHealth(int amount) {
        health -= amount;
        if (health <= 0) {
            alive = false;
        }
    }

    bool IsAlive() const { return alive; }
    int GetID() const { return id; }

private:
    int id;
    bool alive = true;
    int health;
    int damage;
};

#endif // ALLY_H

