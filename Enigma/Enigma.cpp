#include "Enigma.h"

ThreeRotorEnigma::ThreeRotorEnigma(
    std::set<std::string>&& plug_panel,
    std::size_t reflector_setting,
    array_3d&& rotors_positions,
    array_3d&& rotors_settings,
    array_3d&& rings_settings
) : plug_panel(std::forward<std::set<std::string>>(plug_panel)),
    reflector_setting(reflector_setting),
    rotors_positions(std::forward<array_3d>(rotors_positions)),
    rotors_settings(std::forward<array_3d>(rotors_settings)),
    rings_settings(std::forward<array_3d>(rings_settings)) {
    // Check plug panel
    std::for_each(this->plug_panel.begin(), this->plug_panel.end(),
        [this](auto& value) {
            auto palindrome_or_equal = std::find_if(this->plug_panel.begin(), this->plug_panel.end(),
                [&value_arg = value](auto& value) {
                    return (value == std::string{ value_arg.rbegin(), value_arg.rend() })
                        || (value[0] == value[1]);
                }
            );
            if (palindrome_or_equal != this->plug_panel.end()) this->plug_panel.erase(palindrome_or_equal);

            auto count{ std::count_if(this->plug_panel.cbegin(), this->plug_panel.cend(),
                [&value_arg = value](auto& value) {
                    return (value_arg[0] == value[0])
                        || (value_arg[0] == value[1])
                        || (value_arg[1] == value[0])
                        || (value_arg[1] == value[1]);
                }
            ) };

            if (count > 1) {
                std::cout << "\x1b[31m[Enigma] \x1b[33mWrong plug panel!\x1b[0m" << std::endl;
                std::exit(0);
            }
        }
    );

    if (this->reflector_setting < 0 || this->reflector_setting > 1) {
        std::cout << "\x1b[31m[Enigma] \x1b[33mWrong rotors positions! For example (I-III): II I III\x1b[0m" << std::endl;
        std::exit(0);
    }

    // Check rotors positions
    if (std::find_if(this->rotors_positions.cbegin(), this->rotors_positions.cend(),
        [](auto value) { return value < 0 || value > 2; }) != this->rotors_positions.cend()) {
        std::cout << "\x1b[31m[Enigma] \x1b[33mWrong rotors positions! For example (I-III): II I III\x1b[0m" << std::endl;
        std::exit(0);
    }

    // Check rotors settings
    if (std::find_if(this->rotors_settings.cbegin(), this->rotors_settings.cend(),
        [](auto value) { return value < 0 || value > 25; }) != this->rotors_settings.cend()) {
        std::cout << "\x1b[31m[Enigma] \x1b[33mWrong rotors settings! For example (A-Z): X A F\x1b[0m" << std::endl;
        std::exit(0);
    }

    // Check ring settings
    if (std::find_if(this->rings_settings.cbegin(), this->rings_settings.cend(),
        [](auto value) { return value < 0 || value > 25; }) != this->rings_settings.cend()) {
        std::cout << "\x1b[31m[Enigma] \x1b[33mWrong rings settings! For example (A-Z): B J T\x1b[0m" << std::endl;
        std::exit(0);
    }
}

std::string ThreeRotorEnigma::encrypt_message(std::string msg) const {
    std::transform(msg.begin(), msg.end(), msg.begin(), std::toupper);
    msg.erase(std::remove_if(msg.begin(), msg.end(), [](auto x) { return std::isspace(x); }), msg.end());
    std::cout << "msg = " << msg << std::endl;
    std::set<std::string> plug_panel{ this->plug_panel };
    array_3d rotors_positions{ this->rotors_positions };
    array_3d rotors_settings{ this->rotors_settings };
    array_3d rings_settings{ this->rings_settings };

    auto fix_char = [](char value) {
        if (value > 'Z') return static_cast<char>('A' + value - 'Z' - 1);
        else if (value < 'A') return static_cast<char>('Z' - std::abs(value - 'A') + 1);

        return value;
    };
    auto fix_index = [](int value) {
        if (value > 25) return value - 25 - 1;
        else if (value < 0) return 25 - std::abs(value) + 1;

        return value;
    };

    for (auto& ch : msg) {
        // Moving to plug panel
        if (!plug_panel.empty()) {
            std::for_each(plug_panel.cbegin(), plug_panel.cend(),
                [&ch](auto& value) {
                    if (ch == value[0]) ch = value[1];
                    else if (ch == value[1]) ch = value[0];
                }
            );
        }

        // Moving rotors
        rotors_settings[rotors_positions[0]]++;
        if (rotate_next_rotor(rotors_positions[0], rotors_settings[rotors_positions[0]])) rotors_settings[rotors_positions[1]]++;
        if (rotate_next_rotor(rotors_positions[1], rotors_settings[rotors_positions[1]])) rotors_settings[rotors_positions[2]]++;

        // Moving to rotors
        auto index{ fix_index(fix_char(ch + rotors_settings[rotors_positions[0]]) - 'A') };
        ch = this->rotors_letters[rotors_positions[0]][index];

        index = fix_index(fix_char(ch + (rotors_settings[rotors_positions[1]] - rotors_settings[rotors_positions[0]])) - 'A');
        ch = this->rotors_letters[rotors_positions[1]][index];

        index = fix_index(fix_char(ch + (rotors_settings[rotors_positions[2]] - rotors_settings[rotors_positions[1]])) - 'A');
        ch = this->rotors_letters[rotors_positions[2]][index];

        index = fix_index(fix_char(ch - rotors_settings[rotors_positions[2]]) - 'A');
        ch = fix_char(index + 'A');

        // Moving to reflector
        for (std::size_t i = 0; i < reflectors[this->reflector_setting].size(); i += 2) {
            if (ch == this->reflectors[this->reflector_setting][i]) {
                ch = this->reflectors[this->reflector_setting][i + 1];
                break;
            }
            else if (ch == this->reflectors[this->reflector_setting][i + 1]) {
                ch = this->reflectors[this->reflector_setting][i];
                break;
            }
        }

        // Moving from reflector into rotors
        index = rotors_letters[rotors_positions[2]].find(fix_char(ch + rotors_settings[rotors_positions[2]]));
        ch = 'A' + index;

        index = rotors_letters[rotors_positions[1]].find(
            fix_char(ch - (rotors_settings[rotors_positions[2]] - rotors_settings[rotors_positions[1]]))
        );
        ch = 'A' + index;

        index = rotors_letters[rotors_positions[0]].find(
            fix_char(ch - (rotors_settings[rotors_positions[1]] - rotors_settings[rotors_positions[0]]))
        );
        ch = fix_char('A' + index - rotors_settings[rotors_positions[0]]);

        // Moving to plugpanel
        if (!plug_panel.empty()) {
            std::for_each(plug_panel.cbegin(), plug_panel.cend(),
                [&ch](auto& value) {
                    if (ch == value[0]) ch = value[1];
                    else if (ch == value[1]) ch = value[0];
                }
            );
        }
    }

    return msg;
}

ThreeRotorEnigma::array_3d ThreeRotorEnigma::convert_rotors_positions(std::array<std::string, 3>&& rotors_positions) {
    array_3d result{};
    for (std::size_t i = 0; i < rotors_positions.size(); ++i) {
        result[i] = std::count(rotors_positions[i].cbegin(), rotors_positions[i].cend(), 'I') - 1;
    }

    return result;
}

ThreeRotorEnigma::array_3d ThreeRotorEnigma::convert_rotors_or_rings_settings(std::array<char, 3>&& settings) {
    array_3d result{};
    for (std::size_t i = 0; i < settings.size(); ++i) {
        result[i] = settings[i] - 'A';
    }

    return result;
}

std::size_t ThreeRotorEnigma::convert_reflector_setting(char setting) {
    if (setting < 'B' || setting > 'C') {
        std::cout << "\x1b[31m[Enigma] \x1b[33mWrong reflector setting! For example (B-C): B\x1b[0m" << std::endl;
        std::exit(0);
    }

    return 'B' - setting;
}
std::string ThreeRotorEnigma::rotate_ring(std::string_view rotor, int ring_pos) const {
    std::string result{ rotor };
    for (auto& ch : result) {
        ch += ring_pos;

        if (ch > 'Z') {
            ch = 'A' + ch - 'Z' - 1;
        }
    }

    return result.back() + std::string{ result.begin(), result.end() - 1 };
}

constexpr bool ThreeRotorEnigma::rotate_next_rotor(int current_rotor, char current_pos) const {
    current_pos += 'A';
    return (current_rotor == 0 && current_pos == 'R')
        || (current_rotor == 1 && current_pos == 'F')
        || (current_rotor == 2 && current_pos == 'W');
}
