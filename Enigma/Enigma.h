#pragma once

#include <iostream>
#include <string>
#include <array>
#include <set>
#include <algorithm>

class ThreeRotorEnigma {
public:
    using array_3d = std::array<int, 3>;

    ThreeRotorEnigma(
        std::set<std::string>&& plug_panel,
        std::size_t reflector_setting,
        array_3d&& rotors_positions,
        array_3d&& rotors_settings,
        array_3d&& rings_settings
    );

    std::string encrypt_message(std::string msg) const;
    static array_3d convert_rotors_positions(std::array<std::string, 3>&& rotors_positions);
    static array_3d convert_rotors_or_rings_settings(std::array<char, 3>&& settings);
    static std::size_t convert_reflector_setting(char setting);
private:
    std::string rotate_ring(std::string_view rotor, int ring_pos) const;
    constexpr bool rotate_next_rotor(int current_rotor, char current_pos) const;
private:
    inline static const std::string alphabet{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    inline static const std::array<std::string, 3> rotors_letters{
        "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
        "AJDKSIRUXBLHWTMCQGZNPYFVOE",
        "BDFHJLCPRTXVZNYEIWGAKMUSQO"
    };
    inline static const std::array<std::string, 4> reflectors{
        "AYBRCUDHEQFSGLIPJXKNMOTZVW",
        "AFBVCPDJEIGOHYKRLZMXNWTQSU"
    };
    std::set<std::string> plug_panel;
    std::size_t reflector_setting;
    array_3d rotors_positions;
    array_3d rotors_settings;
    array_3d rings_settings;
};