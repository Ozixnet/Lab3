#pragma once

// Структуры параметров для заклинаний (используются с SpellBuffContext)
struct DirectDamageParams {
    int damage;
    int radius;
};

struct AoEParams {
    int damage;
    int areaSize;
};

struct TrapParams {
    int damage;
};

struct SummonParams {
    int count;
    int allyHp;
    int allyDmg;
};

