#ifndef ENEMY_H
#define ENEMY_H

class Enemy {
public:
    Enemy(int h, int d, int id);

    void displayInfo() const;

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

#endif // ENEMY_H
