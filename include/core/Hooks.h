#pragma once

#include "core/HookManager.h"
#include "core/Game.h"
#include "core/Offsets.h"
#include "sdk/Types.h"
#include "sdk/ServerSpawnInfo.h"
#include "sdk/ServerSpawnOverrides.h"
#include "sdk/SocketManagerFactory.h"
#include "sdk/SecureReason.h"

namespace fb
{
    // ============================================================================
    // PvZ: Garden Warfare 1
    // ============================================================================
    namespace gw1
    {
        using tGetOptionParameter = char* (*)(const char*, const char*, int*);
        inline tGetOptionParameter GetOptionParameter =
            reinterpret_cast<tGetOptionParameter>(offsets::gw1::fn_GetOptionParameter);

        __int64 ServerStart(intptr_t inst, ServerSpawnInfo& info, ServerSpawnOverrides* spawnOverrides)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ServerStart);

            g_game->logServerSpawnInfo(info);
            g_game->prepareServerSpawn(inst, info, spawnOverrides);
            
            info.isCoop = false;

            return trampoline(inst, info, spawnOverrides);
        }

        bool ClientInitNetwork(intptr_t inst, bool singleplayer, bool localhost, bool coop, bool hosted)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ClientInitNetwork);

            g_game->logClientInitNetwork(singleplayer, localhost, coop, hosted);

            if (hosted || !singleplayer)
            {
                g_game->injectSocketManagerFactory(inst, 0xA8);
                
                singleplayer = false;
                localhost = false;
                coop = false;
                hosted = false;
            }

            return trampoline(inst, singleplayer, localhost, coop, hosted);
        }

        void ClientConnectToAddress(intptr_t inst, const char* ipAddress, const char* serverPassword)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ClientConnectToAddress);

            ipAddress = g_game->redirectHostingAddress(ipAddress);
            serverPassword = GetOptionParameter("Server.ServerPassword", "", 0);
            g_game->logClientConnect(ipAddress, serverPassword);

            return trampoline(inst, ipAddress, serverPassword);
        }

        char NetworkEnginePeerInit(intptr_t inst, __int64 crypto, intptr_t socketManager, 
            const char* address, int titleId, int versionId, bool debug, bool voice, bool live)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&NetworkEnginePeerInit);

            std::string_view addressView{ address };
            if (const auto colon = addressView.find(':'); colon != std::string_view::npos)
            {
                const auto port = addressView.substr(colon + 1);
                address = g_game->adjustPeerAddress(address, port);
            }

            return trampoline(inst, crypto, socketManager, address, titleId, versionId, debug, voice, live);
        }

        void PeerHasJoined(intptr_t inst, intptr_t playerInstance, const char* player)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&PeerHasJoined);

            if (strlen(player) > 0)
                g_game->logPeerJoined(player);

            trampoline(inst, playerInstance, player);
        }

        void ClientDisconnect(intptr_t inst)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ClientDisconnect);

            SecureReason reason = *reinterpret_cast<const SecureReason*>(inst + 0x670);
            g_game->handleDisconnection(reason);

            trampoline(inst);
        }

        void __fastcall ClientInactivityTimer(intptr_t, float)
        {
            return;
        }

        char* GetPlayerName()
        {
            return GetOptionParameter("name", "Player", 0);
        }
    }

    // ============================================================================
    // PvZ: Garden Warfare 2
    // ============================================================================
    namespace gw2
    {
        using tGetOptionParameter = char* (*)(const char*, const char*, int*);
        inline tGetOptionParameter GetOptionParameter = 
            reinterpret_cast<tGetOptionParameter>(offsets::gw2::fn_GetOptionParameter);

        __int64 ServerStart(intptr_t inst, ServerSpawnInfo& info, ServerSpawnOverrides* spawnOverrides)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ServerStart);

            g_game->logServerSpawnInfo(info);
            g_game->prepareServerSpawn(inst, info, spawnOverrides);

            // GW2 doesn't force isCoop to false (unlike GW1)

            return trampoline(inst, info, spawnOverrides);
        }

        bool ClientInitNetwork(intptr_t inst, char idk, bool localhost, bool coop, bool hosted, bool singleplayer)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ClientInitNetwork);

            GG_LOG(GG::LogLevel::Info, 
                "fb::client::InitNetwork(idk: %d, singleplayer: %d, localhost: %d, coop: %d, hosted: %d)",
                (char)idk, (char)singleplayer, (char)localhost, (char)coop, (char)hosted);

            if (!localhost)
            {
                g_game->injectSocketManagerFactory(inst, 0xB8);
                
                singleplayer = false;
                coop = false;
                hosted = true;
            }

            return trampoline(inst, idk, localhost, coop, hosted, singleplayer);
        }

        void ClientConnectToAddress(intptr_t inst, const char* ipAddress, const char* serverPassword)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ClientConnectToAddress);

            ipAddress = g_game->redirectHostingAddress(ipAddress);
            serverPassword = GetOptionParameter("Server.ServerPassword", "", 0);
            g_game->logClientConnect(ipAddress, serverPassword);

            return trampoline(inst, ipAddress, serverPassword);
        }

        char NetworkEnginePeerInit(intptr_t inst, __int64 crypto, intptr_t socketManager, 
            const char* address, int titleId, int versionId)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&NetworkEnginePeerInit);

            std::string_view addressView{ address };
            if (const auto colon = addressView.find(':'); colon != std::string_view::npos)
            {
                const auto port = addressView.substr(colon + 1);
                address = g_game->adjustPeerAddress(address, port);
            }

            return trampoline(inst, crypto, socketManager, address, titleId, versionId);
        }

        __int64 ClientDisconnected(intptr_t inst, SecureReason reason, char* reasonText)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&ClientDisconnected);

            g_game->handleDisconnection(reason);

            return trampoline(inst, reason, reasonText);
        }

        void PeerHasJoined(intptr_t inst, intptr_t playerInstance, const char* player)
        {
            const auto trampoline = GG::HookManager::getManager().Call(&PeerHasJoined);

            if (strlen(player) > 0)
                g_game->logPeerJoined(player);

            trampoline(inst, playerInstance, player);
        }

        char* GetPlayerName()
        {
            return GetOptionParameter("name", "Player", 0);
        }
    }
}

static GG::HookTemplate g_PvZGW1_Hooks[] = {
    {offsets::gw1::fn_ServerStart,              reinterpret_cast<void*>(fb::gw1::ServerStart),              true},
    {offsets::gw1::fn_ClientInitNetwork,        reinterpret_cast<void*>(fb::gw1::ClientInitNetwork),        true},
    {offsets::gw1::fn_ClientConnectToAddress,   reinterpret_cast<void*>(fb::gw1::ClientConnectToAddress),   true},
    {offsets::gw1::fn_NetworkEnginePeerInit,    reinterpret_cast<void*>(fb::gw1::NetworkEnginePeerInit),    true},
    {offsets::gw1::fn_PeerHasJoined,            reinterpret_cast<void*>(fb::gw1::PeerHasJoined),            true},
    {offsets::gw1::fn_ClientDisconnect,         reinterpret_cast<void*>(fb::gw1::ClientDisconnect),         true},
    {offsets::gw1::fn_ClientInactivityTimer,    reinterpret_cast<void*>(fb::gw1::ClientInactivityTimer),    true},
    {offsets::gw1::fn_GetPlayerName,            reinterpret_cast<void*>(fb::gw1::GetPlayerName),            true},
};

static GG::HookTemplate g_PvZGW2_Hooks[] = {
    {offsets::gw2::fn_ServerStart,              reinterpret_cast<void*>(fb::gw2::ServerStart),              true},
    {offsets::gw2::fn_ClientInitNetwork,        reinterpret_cast<void*>(fb::gw2::ClientInitNetwork),        true},
    {offsets::gw2::fn_ClientConnectToAddress,   reinterpret_cast<void*>(fb::gw2::ClientConnectToAddress),   true},
    {offsets::gw2::fn_NetworkEnginePeerInit,    reinterpret_cast<void*>(fb::gw2::NetworkEnginePeerInit),    true},
    {offsets::gw2::fn_ClientDisconnected,       reinterpret_cast<void*>(fb::gw2::ClientDisconnected),       true},
    {offsets::gw2::fn_PeerHasJoined,            reinterpret_cast<void*>(fb::gw2::PeerHasJoined),            true},
    {offsets::gw2::fn_GetPlayerName,            reinterpret_cast<void*>(fb::gw2::GetPlayerName),            true},
};
