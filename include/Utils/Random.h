#ifndef RANDOM_H
#define RANDOM_H

class Random {
public:
    // Инициализировать генератор случайных чисел
    static void initialize();
    
    // Получить случайное число от 0 до max-1
    static int getRange(int max);
    
    // Получить случайное число от min до max (включительно)
    static int getRange(int min, int max);
    
    // Получить вероятность в процентах (true если выпадает)
    static bool chance(int percent);

private:
    Random() = delete;  // Утилитарный класс
};

#endif // RANDOM_H
