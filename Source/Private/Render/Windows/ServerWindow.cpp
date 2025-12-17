// Copyright BattleDash. All Rights Reserved.

#include <Render/Windows/ServerWindow.h>

#include <Base/Log.h>
#include <Core/Program.h>
#include <Render/Windows/MainWindow.h>
#include <SDK/Modes.h>

#include <vector>
#include <algorithm>
#include <map>

namespace Kyber
{
ServerWindow::ServerWindow()
{
    /* g_program->m_api->GetProxies([&](std::optional<std::vector<KyberProxy>> kyberProxies) {
        std::sort(kyberProxies->begin(), kyberProxies->end(), [](const KyberProxy& a, const KyberProxy& b) {
            return a.ping < b.ping;
        });
        kyberProxies->push_back(KyberProxy{ "", "", "", "No Proxy", 0 });
        m_proxies = kyberProxies;
    }) */
    ;
}

bool ServerWindow::IsEnabled()
{
    return g_mainWindow->IsEnabled() && m_isEnabled;
}

bool DrawScoreboardPlayer(std::vector<ServerPlayer*> playerList, int index)
{
    if (playerList.size() <= index)
    {
        return false;
    }
    ServerPlayer* player = playerList[index];
    ImGui::Text("%s", player->m_name);
    ImGui::SameLine();
    if (ImGui::SmallButton(("SWAP TEAM##" + std::string(player->m_name)).c_str()))
    {
        g_program->m_server->SetPlayerTeam(player, player->m_teamId == 1 ? 2 : 1);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton(("KICK##" + std::string(player->m_name)).c_str()))
    {
        g_program->m_server->KickPlayer(player, "You have been kicked.");
    }
    return true;
}

void ServerWindow::Draw()
{
    ImGui::Begin("SERVER SETTINGS", &m_isEnabled, ImGuiWindowFlags_AlwaysAutoResize);

    static GameMode currentMode = s_game_modes[0]; // Default to first mode
    static GameLevel currentLevel = { "", "" };

    // Initialize currentLevel if not already
    if (currentLevel.level[0] == '\0')
    {
        if (!currentMode.levelOverrides.empty())
            currentLevel = currentMode.levelOverrides[0];
        else if (!currentMode.levels.empty())
            currentLevel = { currentMode.levels[0], currentMode.levels[0] };
    }

    ImGui::Text("GAME MODE:");
    if (ImGui::BeginCombo("##modeCombo", currentMode.name))
    {
        for (int n = 0; n < sizeof(s_game_modes) / sizeof(GameMode); n++)
        {
            bool selected = (strcmp(currentMode.mode, s_game_modes[n].mode) == 0);
            if (ImGui::Selectable(s_game_modes[n].name, selected))
            {
                currentMode = s_game_modes[n];

                // Update currentLevel when mode changes
                if (!currentMode.levelOverrides.empty())
                    currentLevel = currentMode.levelOverrides[0];
                else if (!currentMode.levels.empty())
                    currentLevel = { currentMode.levels[0], currentMode.levels[0] };
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("LEVEL:");
    if (ImGui::BeginCombo("##levelCombo", currentLevel.name))
    {
        for (size_t i = 0; i < currentMode.levels.size(); i++)
        {
            // GetGameLevel safely returns a GameLevel by value
            GameLevel level = GetGameLevel(currentMode, currentMode.levels[i]);

            bool selected = (strcmp(currentLevel.level, level.level) == 0);
            if (ImGui::Selectable(level.name, selected))
                currentLevel = level;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static int maxPlayers = 4;
    ImGui::SliderInt("Max Players", &maxPlayers, 1, 8);

    if (ImGui::Button("Start Server"))
    {
        if (currentLevel.level[0] != '\0' && currentMode.mode[0] != '\0')
        {
            g_program->m_server->Start(currentLevel.level, currentMode.mode, maxPlayers, SocketSpawnInfo(false, "", "Test Server"));
        }
    }

    ImGui::End();
}
} // namespace Kyber
