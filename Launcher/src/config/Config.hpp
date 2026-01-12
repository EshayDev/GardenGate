#pragma once
#include <string>
#include <vector>

#include "json.hpp"

struct GameConfig {
    std::string game_path;
    std::string custom_args;
    bool moddata_enabled = true;
    std::vector<std::string> moddata_items;
    std::string moddata_selected = "Default";

    void from_json(const nlohmann::json& j);
    nlohmann::json to_json() const;
};

struct Config {
    std::string username;
    std::string server_ip;
    std::string password;

    std::string game_selected = "GW1";

    GameConfig gw1;
    GameConfig gw2;

    void set_username(const char* val) {
        username = val;
        if (username.length() > 15) username.resize(15);
    }

    GameConfig& get_current_game() {
        return (game_selected == "GW2") ? gw2 : gw1;
    }

    const GameConfig& get_current_game() const {
        return (game_selected == "GW2") ? gw2 : gw1;
    }

    int get_game_selected_int() const {
        return (game_selected == "GW2") ? 1 : 0;
    }

    void set_game_selected_from_int(int val) {
        game_selected = (val == 1) ? "GW2" : "GW1";
    }
};

Config load_config(const std::string& file);
void save_config(const Config& cfg, const std::string& file);
