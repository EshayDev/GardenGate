#include "Ui.hpp"
#include "Config.hpp"
#include "Patcher.hpp"
#include "Utils.hpp"

#include <Windows.h>
#include <filesystem>
#include <string>

extern Config g_config;
namespace fs = std::filesystem;

static void setupStyle(float dpiScale) {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 baseGreen = ImVec4(0.25f, 0.47f, 0.32f, 1.0f);
	ImVec4 hoverGreen = ImVec4(0.30f, 0.55f, 0.38f, 1.0f);
	ImVec4 activeGreen = ImVec4(0.20f, 0.40f, 0.28f, 1.0f);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	style.Colors[ImGuiCol_Button] = baseGreen;
	style.Colors[ImGuiCol_ButtonHovered] = hoverGreen;
	style.Colors[ImGuiCol_ButtonActive] = activeGreen;
	style.Colors[ImGuiCol_CheckMark] = baseGreen;
	style.Colors[ImGuiCol_FrameBgActive] = activeGreen;
	style.Colors[ImGuiCol_FrameBgHovered] = hoverGreen;
	style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);

	style.WindowPadding = ImVec2(16 * dpiScale, 10 * dpiScale);
	style.ItemSpacing = ImVec2(10 * dpiScale, 6 * dpiScale);
	style.FramePadding = ImVec2(6 * dpiScale, 4 * dpiScale);
	style.WindowBorderSize = 0.0f;
	style.FrameRounding = 4.0f;
}

static void drawTitleBar(HWND hwnd, float dpiScale) {
	float h = 40.0f * dpiScale;
	ImGui::BeginChild("TitleBar", ImVec2(0, h), false);

	ImGui::SetCursorPos(ImVec2(15 * dpiScale, (h - ImGui::GetFontSize() * 1.2f) * 0.5f));
	ImGui::TextUnformatted("GARDEN GATE LAUNCHER");

	float btnW = 40 * dpiScale, btnH = 26 * dpiScale, spacing = 12 * dpiScale, rightPad = 10 * dpiScale;
	float yOffset = (h - btnH) * 0.5f;
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - (btnW * 2) - spacing - rightPad, yOffset));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 1));
	if (ImGui::Button("_", ImVec2(btnW, btnH))) ::ShowWindow(hwnd, SW_MINIMIZE);
	ImGui::SameLine(0, spacing);
	if (ImGui::Button("X", ImVec2(btnW, btnH))) ::PostMessage(hwnd, WM_CLOSE, 0, 0);
	ImGui::PopStyleColor();

	if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		if (!ImGui::IsAnyItemActive()) {
			::ReleaseCapture();
			::SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
	}
	ImGui::EndChild();
}

static void drawTabBar(int& currentTab, float dpiScale) {

	ImVec4 colorBaseGreen = ImVec4(0.25f, 0.47f, 0.32f, 1.0f);

	ImGui::BeginChild("TabBar", ImVec2(0, 45.0f * dpiScale), false,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::SetCursorPos(ImVec2(15.0f * dpiScale, 6.0f * dpiScale));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f * dpiScale, 8.0f * dpiScale));

	float tabWidth = 130.0f * dpiScale;

	ImGui::PushStyleColor(ImGuiCol_Button,
		currentTab == 0 ? colorBaseGreen : ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
	if (ImGui::Button("Launcher", ImVec2(tabWidth, 0))) currentTab = 0;
	ImGui::PopStyleColor();

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button,
		currentTab == 1 ? colorBaseGreen : ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
	if (ImGui::Button("Patcher", ImVec2(tabWidth, 0))) currentTab = 1;
	ImGui::PopStyleColor();

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

static void drawLauncherTab(HWND hwnd, float dpiScale, Utils::UI::Status& status) {
	float safeWidth = ImGui::GetContentRegionAvail().x;
	float fieldWidth = safeWidth * 0.85f;
	float centerOffset = (safeWidth - fieldWidth) / 2.0f;

	int gameSelectedInt = g_config.get_game_selected_int();
	ImGui::SetCursorPosX(centerOffset);
	if (ImGui::RadioButton("GW1", &gameSelectedInt, 0)) g_config.set_game_selected_from_int(0);
	ImGui::SameLine(0.0f, 40 * dpiScale);
	if (ImGui::RadioButton("GW2", &gameSelectedInt, 1)) g_config.set_game_selected_from_int(1);

	ImGui::Separator();
	Utils::UI::CenteredInput("Username:", g_config.username, centerOffset, fieldWidth);
	Utils::UI::CenteredInput("Server IP:", g_config.server_ip, centerOffset, fieldWidth);
	Utils::UI::CenteredInput("Password:", g_config.password, centerOffset, fieldWidth, ImGuiInputTextFlags_Password);

	auto& currentGame = g_config.get_current_game();
	ImGui::SetCursorPosX(centerOffset);
	ImGui::TextUnformatted("Game Path:");
	ImGui::SetCursorPosX(centerOffset);
	ImGui::SetNextItemWidth(fieldWidth - 180 * dpiScale);

	char buf[512];
	strncpy_s(buf, currentGame.game_path.c_str(), sizeof(buf));
	if (ImGui::InputText("##GamePath", buf, sizeof(buf))) currentGame.game_path = buf;

	ImGui::SameLine();
	if (ImGui::Button("Auto-Detect", ImVec2(100 * dpiScale, 0))) {
		std::string detected = Utils::Registry::GetGamePathFromRegistry(gameSelectedInt == 1);
		if (!detected.empty()) { currentGame.game_path = detected; status.Show("Auto-detected: " + detected); }
		else status.Show("Could not auto-detect game path", true);
	}
	ImGui::SameLine();
	if (ImGui::Button("Browse", ImVec2(80 * dpiScale, 0))) {
		std::string chosen = Utils::Dialog::BrowseForExe(hwnd, gameSelectedInt == 1);
		if (!chosen.empty()) currentGame.game_path = chosen;
	}

	Utils::UI::CenteredInput("Custom Arguments:", currentGame.custom_args, centerOffset, fieldWidth);

	ImGui::Dummy(ImVec2(0, 12 * dpiScale));
	if (Utils::UI::CenteredButton("LAUNCH GAME", 180 * dpiScale, dpiScale)) {
		std::string args;
		if (!currentGame.custom_args.empty()) args += currentGame.custom_args + " ";
		if (!g_config.username.empty()) args += "-name " + g_config.username + " ";
		if (!g_config.server_ip.empty()) args += "-Client.ServerIp " + g_config.server_ip + " ";
		if (!g_config.password.empty()) args += "-Server.ServerPassword " + g_config.password + " ";
		if (currentGame.moddata_enabled && currentGame.moddata_selected != "Default") {
			args += "-dataPath ModData/" + currentGame.moddata_selected + " ";
		}
		if (!args.empty() && args.back() == ' ') args.pop_back();

		save_config(g_config, "config.json");

		std::string errorMsg;
		if (Utils::Process::LaunchGame(currentGame.game_path, args, hwnd, errorMsg)) {
			status.Show("Game launched successfully!");
		}
		else {
			status.Show(errorMsg, true);
		}
	}
}

static void drawPatcherTab(HWND hwnd, float dpiScale, Utils::UI::Status& status) {
	float safeWidth = ImGui::GetContentRegionAvail().x;
	float fieldWidth = safeWidth * 0.85f;
	float centerOffset = (safeWidth - fieldWidth) / 2.0f;
	float browseBtnW = 80.0f * dpiScale;

	auto& gw2Config = g_config.gw2;
	bool isPatched = !gw2Config.game_path.empty() && Patcher::IsGW2Patched(gw2Config.game_path);

	ImGui::SetCursorPosX(centerOffset);
	ImGui::TextUnformatted("GW2 Executable:");
	ImGui::SetCursorPosX(centerOffset);
	ImGui::SetNextItemWidth(fieldWidth - browseBtnW - 120 * dpiScale);

	char buf[512];
	strncpy_s(buf, gw2Config.game_path.c_str(), sizeof(buf));
	if (ImGui::InputText("##ExePath", buf, sizeof(buf))) {
		gw2Config.game_path = buf;
		save_config(g_config, "config.json");
		if (!gw2Config.game_path.empty()) isPatched = Patcher::IsGW2Patched(gw2Config.game_path);
	}

	ImGui::SameLine();
	if (ImGui::Button("Auto Detect", ImVec2(120 * dpiScale, 0))) {
		std::string detected = Utils::Registry::GetGamePathFromRegistry(true);
		if (!detected.empty()) {
			gw2Config.game_path = detected;
			save_config(g_config, "config.json");
			status.Show("autodetected GW2 path: " + detected);
			isPatched = Patcher::IsGW2Patched(gw2Config.game_path);
		}
		else status.Show("Could not autodetect GW2 installation", true);
	}
	ImGui::SameLine();
	if (ImGui::Button("Browse", ImVec2(browseBtnW, 0))) {
		std::string chosen = Utils::Dialog::BrowseForExe(hwnd, true);
		if (!chosen.empty()) {
			gw2Config.game_path = chosen;
			save_config(g_config, "config.json");
			isPatched = Patcher::IsGW2Patched(gw2Config.game_path);
		}
	}

	ImGui::Dummy(ImVec2(0, 20 * dpiScale));
	fs::path launcherDir = fs::path([] {
		char path[MAX_PATH]; GetModuleFileNameA(NULL, path, MAX_PATH); return std::string(path);
		}()).parent_path();
	std::string patchFile = (launcherDir / "gw2_patch.xdelta").string();
	std::string dllFile = (launcherDir / "dinput8.dll").string();
	bool hasPatch = fs::exists(patchFile), hasDLL = fs::exists(dllFile);

	if (!hasPatch || !hasDLL) {
		ImGui::SetCursorPosX(centerOffset);
		ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Missing required files:");
		if (!hasPatch) { ImGui::SetCursorPosX(centerOffset); ImGui::Text("- gw2_patch.xdelta"); }
		if (!hasDLL) { ImGui::SetCursorPosX(centerOffset); ImGui::Text("- dinput8.dll"); }
	}

	if (isPatched) {
		ImGui::SetCursorPosX(centerOffset);
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "GW2 is already patched");
		float btnW = 200 * dpiScale, btnH = 40 * dpiScale;
		ImGui::SetCursorPosX((safeWidth - btnW) / 2.0f);
		if (ImGui::Button("RESTORE ORIGINAL GW2", ImVec2(btnW, btnH))) {
			std::string err;
			if (Patcher::RestoreGW2(gw2Config.game_path, err)) {
				status.Show("GW2 restored to original!");
				isPatched = false;
			}
			else status.Show("Restore failed: " + err, true);
		}
	}
	else {
		ImGui::SetCursorPosX(centerOffset);
		ImGui::Text("GW2 DOWNGRADER");
		float btnW = 200 * dpiScale, btnH = 40 * dpiScale;
		ImGui::SetCursorPosX((safeWidth - btnW) / 2.0f);
		ImGui::BeginDisabled(!hasPatch || !hasDLL || gw2Config.game_path.empty());
		if (ImGui::Button("PATCH GW2", ImVec2(btnW, btnH))) {
			std::string err;
			if (Patcher::AutoPatchGW2(gw2Config.game_path, patchFile, dllFile, err)) {
				status.Show("Auto patch successful!");
				isPatched = true;
			}
			else status.Show("Auto patch failed: " + err, true);
		}
		ImGui::EndDisabled();
	}
}

void UI::DrawUI(HWND hwnd, float dpiScale) {
	static int currentTab = 0;
	static Utils::UI::Status status;

	setupStyle(dpiScale);

	RECT rc; GetClientRect(hwnd, &rc);
	ImVec2 size((float)(rc.right - rc.left), (float)(rc.bottom - rc.top));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(size);

	ImGui::Begin("Garden Gate", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	drawTitleBar(hwnd, dpiScale);
	drawTabBar(currentTab, dpiScale);

	ImGui::BeginChild("ContentArea", ImVec2(0, ImGui::GetContentRegionAvail().y), false);
	if (currentTab == 0) drawLauncherTab(hwnd, dpiScale, status);
	else               drawPatcherTab(hwnd, dpiScale, status);

	if (status.visible) {
		ImGui::Dummy(ImVec2(0, 10 * dpiScale));
		ImGui::TextColored(status.color, "%s", status.message.c_str());
		status.Update(ImGui::GetIO().DeltaTime);
	}
	ImGui::EndChild();

	ImGui::End();
}
