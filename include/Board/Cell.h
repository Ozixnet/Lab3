#pragma once
#include <string>
#include <cstdint>


enum class CellType : uint8_t { Empty, Wall, Player, Enemy, HellBuild, SlowTrap, PlayerTrap, Tower, TowerRange, Ally};

inline const std::string& glyph(CellType t) {
    static const std::string E  = u8" ";
    static const std::string W  = u8"□";
    static const std::string P  = u8"■";
    static const std::string En = u8"▣";
    static const std::string Hb = u8"▒";
    static const std::string Tw = u8"♠";
    static const std::string Pt = u8"○";
    static const std::string St = u8"~";
    static const std::string Tr = u8".";
    static const std::string Al = u8"▲";


    switch (t)
    {
    case CellType::Empty:      return E;
    case CellType::Wall:       return W;
    case CellType::Player:     return P;
    case CellType::Enemy:      return En;
    case CellType::HellBuild:  return Hb;
    case CellType::SlowTrap:   return St;
    case CellType::PlayerTrap: return Pt;
    case CellType::Tower:      return Tw;
    case CellType::TowerRange: return Tr;
    case CellType::Ally:       return Al;
    }

    return E;
}