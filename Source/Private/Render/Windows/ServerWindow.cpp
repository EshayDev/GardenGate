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

    static GameMode* currentMode = &s_game_modes[0]; // pointer to GameMode
    static GameLevel* currentLevel = nullptr;        // pointer to GameLevel

    // Initialize currentLevel if not set
    if (!currentLevel)
    {
        if (!currentMode->levelOverrides.empty())
            currentLevel = &currentMode->levelOverrides[0];
        else if (!currentMode->levels.empty())
            currentLevel = new GameLevel{ currentMode->levels[0], currentMode->levels[0] }; // make sure it's valid
    }

    ImGui::Text("GAME MODE:");
    if (ImGui::BeginCombo("##modeCombo", currentMode->name))
    {
        for (int n = 0; n < sizeof(s_game_modes) / sizeof(GameMode); n++)
        {
            bool selected = (currentMode == &s_game_modes[n]);
            if (ImGui::Selectable(s_game_modes[n].name, selected))
            {
                currentMode = &s_game_modes[n];
                // Reset level to first valid one
                if (!currentMode->levelOverrides.empty())
                    currentLevel = &currentMode->levelOverrides[0];
                else if (!currentMode->levels.empty())
                    currentLevel = new GameLevel{ currentMode->levels[0], currentMode->levels[0] };
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("LEVEL:");
    if (ImGui::BeginCombo("##levelCombo", currentLevel->name))
    {
        for (size_t i = 0; i < currentMode->levels.size(); i++)
        {
            GameLevel tempLevel = GetGameLevel(*currentMode, currentMode->levels[i]);
            bool selected = (strcmp(currentLevel->level, tempLevel.level) == 0);
            if (ImGui::Selectable(tempLevel.name, selected))
                currentLevel = new GameLevel{ tempLevel.level, tempLevel.name }; // safe copy
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static int maxPlayers = 4;
    ImGui::SliderInt("Max Players", &maxPlayers, 1, 8);

    static int errorTime = 0;
    if (!g_program->m_server->m_running)
    {
        if (ImGui::Button("Start Server"))
        {
            if (currentLevel && currentMode)
            {
                g_program->m_server->Start(currentLevel->level, currentMode->mode, maxPlayers);
            }
            else
            {
                errorTime = 1000;
            }
        }

        if (errorTime > 0 && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Please select a valid game mode and level.");
            ImGui::EndTooltip();
            errorTime--;
        }
    }
    else
    {
        ImGui::Text("The server is running. Stop it to change settings.");
    }

    ImGui::End();
}
} // namespace Kyber