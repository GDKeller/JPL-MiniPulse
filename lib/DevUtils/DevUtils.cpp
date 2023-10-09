#include "DevUtils.h"


String DevUtils::termColor(const char* color) {
    static const std::map<std::string, const char*> colorMap = {
        {"black", "\e[0;30m"},
        {"red", "\e[0;31m"},
        {"green", "\e[0;32m"},
        {"yellow", "\e[0;33m"},
        {"blue", "\e[0;34m"},
        {"purple", "\e[0;35m"},
        {"cyan", "\e[0;36m"},
        {"white", "\e[0;37m"},
        
        // Bright versions of the colors
        {"bright_black", "\e[0;90m"},
        {"bright_red", "\e[0;91m"},
        {"bright_green", "\e[0;92m"},
        {"bright_yellow", "\e[0;93m"},
        {"bright_blue", "\e[0;94m"},
        {"bright_purple", "\e[0;95m"},
        {"bright_cyan", "\e[0;96m"},
        {"bright_white", "\e[0;97m"},
        
        // Background colors
        {"bg_black", "\e[0;40m"},
        {"bg_red", "\e[0;41m"},
        {"bg_green", "\e[0;42m"},
        {"bg_yellow", "\e[0;43m"},
        {"bg_blue", "\e[0;44m"},
        {"bg_purple", "\e[0;45m"},
        {"bg_cyan", "\e[0;46m"},
        {"bg_white", "\e[0;47m"},
        
        // Bright background colors
        {"bg_bright_black", "\e[0;100m"},
        {"bg_bright_red", "\e[0;101m"},
        {"bg_bright_green", "\e[0;102m"},
        {"bg_bright_yellow", "\e[0;103m"},
        {"bg_bright_blue", "\e[0;104m"},
        {"bg_bright_purple", "\e[0;105m"},
        {"bg_bright_cyan", "\e[0;106m"},
        {"bg_bright_white", "\e[0;107m"},
        
        // Reset
        {"reset", "\e[0m"}
    };

    auto it = colorMap.find(color);
    if (it != colorMap.end()) {
        return it->second;
    }

    return "\e[0m"; // Default to reset if color is not found
}

String DevUtils::getFreeHeap() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "MEM_Free_Heap:%u", ESP.getFreeHeap());
    return buffer;
}

void DevUtils::printFreeHeap() {
    String printString;

    // printString += "\n";
    // printString += termColor("blue");
    printString += "MEM_Free_Heap:";
    printString += ESP.getFreeHeap() * 0.001; // Value being divided for visualization on plotter
    // printString += ESP.getFreeHeap();	// This is the actual value
    printString += "\n";
    // printString += termColor("reset");
    // printString += "\n";

    Serial.print(printString);
}

void DevUtils::handleException() {
    Serial.print(termColor("red"));
    Serial.println("EXCEPTION CAUGHT:");
    try {
        throw;
    }
    catch (const std::exception& e) {
        Serial.println(e.what());

    }
    catch (const int i) {
        Serial.println(i);
    }
    catch (const long l) {
        Serial.println(l);
    }
    catch (const char* p) {
        if (p == nullptr) {
            Serial.println("Exception is null");
            return;
        } else {
            Serial.println(p);
        }
    }
    catch (...) {
        Serial.println("Exception unknown");
    }
    Serial.println(termColor("reset"));
}

String DevUtils::repeatString(String input, int count) {
    String result;
    for (int i = 0; i < count; i++) {
        result += input;
    }
    return result;
}

void DevUtils::SerialBanners::printBootSplashBanner() {
    Serial.print("\n\n::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: :::::::::\n\n");
    Serial.println("  .  ..__. __..__.     ..__ .");
    Serial.println("  |\\ |[__](__ [__]     |[__)|");
    Serial.println("  | \\||  |.__)|  |  \\__||   |___");
    Serial.print("\n");
    Serial.println("   /$$      /$$ /$$           /$$ /$$$$$$$            /$$");
    Serial.println("  | $$$    /$$$|__/          |__/| $$__  $$          | $$");
    Serial.println("  | $$$$  /$$$$ /$$ /$$$$$$$  /$$| $$  \\ $$ /$$   /$$| $$  /$$$$$$$  /$$$$$$");
    Serial.println("  | $$ $$/$$ $$| $$| $$__  $$| $$| $$$$$$$/| $$  | $$| $$ /$$_____/ /$$__  $$");
    Serial.println("  | $$  $$$| $$| $$| $$  \\ $$| $$| $$____/ | $$  | $$| $$|  $$$$$$ | $$$$$$$$");
    Serial.println("  | $$\\  $ | $$| $$| $$  | $$| $$| $$      | $$  | $$| $$ \\____  $$| $$_____/");
    Serial.println("  | $$ \\/  | $$| $$| $$  | $$| $$| $$      |  $$$$$$/| $$ /$$$$$$$/|  $$$$$$$");
    Serial.println("  |__/     |__/|__/|__/  |__/|__/|__/       \\______/ |__/|_______/  \\_______/");
    Serial.print("\n");
    Serial.print("::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: :::::::::\n\n");
}

/* Display Config WiFi info in visually pleasing format */
void DevUtils::SerialBanners::printWiFiConfigBanner(const char* apPassword, WiFiManager& wifiManager) {
    String network = wifiManager.getConfigPortalSSID();
	String password = apPassword;
	if (password.length() == 0) {
		password = "No Password Set (Open Network)";
	}
	String ipAddr = WiFi.softAPIP().toString();

	String networkInfo = "Network: " + network;
	String passwordInfo = "Password: " + password;
	String ipInfo = "IP Address: " + ipAddr;

	int maxLen = max({ networkInfo.length(), passwordInfo.length(), ipInfo.length() });

	String dashLine = DevUtils::repeatString("─", maxLen + 2);

	String formatStr = "│ %-" + String(maxLen) + "s │\n";
	String headerStr =
		"┌" + dashLine + "┐\n"
		"│ Connection to WiFi Portal%*s│\n"
		"├" + dashLine + "┤\n" +
		formatStr +
		formatStr +
		formatStr +
		"└" + dashLine + "┘\n\n";

	char buffer[640];
	snprintf(buffer, sizeof(buffer),
		headerStr.c_str(),
		maxLen - 24, "",
		networkInfo.c_str(),
		passwordInfo.c_str(),
		ipInfo.c_str()
	);
	Serial.print(buffer);
}