#include "DevUtils.h"


String DevUtils::termColor(const char* color) {
    if (strcmp(color, "reset") == 0) return "\e[0m";

    // Standard colors
    if (strcmp(color, "black") == 0) return "\e[0;30m";
    if (strcmp(color, "red") == 0) return "\e[0;31m";
    if (strcmp(color, "green") == 0) return "\e[0;32m";
    if (strcmp(color, "yellow") == 0) return "\e[0;33m";
    if (strcmp(color, "blue") == 0) return "\e[0;34m";
    if (strcmp(color, "purple") == 0) return "\e[0;35m";
    if (strcmp(color, "cyan") == 0) return "\e[0;36m";
    if (strcmp(color, "white") == 0) return "\e[0;37m";

    // Bright colors
    if (strcmp(color, "bright_black") == 0) return "\e[0;90m";
    if (strcmp(color, "bright_red") == 0) return "\e[0;91m";
    if (strcmp(color, "bright_green") == 0) return "\e[0;92m";
    if (strcmp(color, "bright_yellow") == 0) return "\e[0;93m";
    if (strcmp(color, "bright_blue") == 0) return "\e[0;94m";
    if (strcmp(color, "bright_purple") == 0) return "\e[0;95m";
    if (strcmp(color, "bright_cyan") == 0) return "\e[0;96m";
    if (strcmp(color, "bright_white") == 0) return "\e[0;97m";

    // Background colors
    if (strcmp(color, "bg_black") == 0) return "\e[0;40m";
    if (strcmp(color, "bg_red") == 0) return "\e[0;41m";
    if (strcmp(color, "bg_green") == 0) return "\e[0;42m";
    if (strcmp(color, "bg_yellow") == 0) return "\e[0;43m";
    if (strcmp(color, "bg_blue") == 0) return "\e[0;44m";
    if (strcmp(color, "bg_purple") == 0) return "\e[0;45m";
    if (strcmp(color, "bg_cyan") == 0) return "\e[0;46m";
    if (strcmp(color, "bg_white") == 0) return "\e[0;47m";

    // Bright background colors
    if (strcmp(color, "bg_bright_black") == 0) return "\e[0;100m";
    if (strcmp(color, "bg_bright_red") == 0) return "\e[0;101m";
    if (strcmp(color, "bg_bright_green") == 0) return "\e[0;102m";
    if (strcmp(color, "bg_bright_yellow") == 0) return "\e[0;103m";
    if (strcmp(color, "bg_bright_blue") == 0) return "\e[0;104m";
    if (strcmp(color, "bg_bright_purple") == 0) return "\e[0;105m";
    if (strcmp(color, "bg_bright_cyan") == 0) return "\e[0;106m";
    if (strcmp(color, "bg_bright_white") == 0) return "\e[0;107m";

    return "\e[0m"; // Default to reset
}

String DevUtils::getFreeHeap() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "MEM_Free_Heap:%u", ESP.getFreeHeap());
    return buffer;
}

void DevUtils::printFreeHeap() {
    String printString;

    printString += "MEM_Free_Heap:";
    printString += ESP.getFreeHeap() * 0.001; // Value being divided for visualization on plotter
    // printString += ESP.getFreeHeap();	// This is the actual value
    printString += "\n";

    Serial.print(printString);
}

void DevUtils::handleException() {
    Serial.print(
        DevUtils::termColor("red") +
        "[!] EXCEPTION CAUGHT - The error is not being printed for runtime safety " +
        DevUtils::termColor("reset") +
        "\n"
    );


    // try {
    //     throw;
    // }
    // catch (const std::exception& e) {
    //     Serial.println(e.what());

    // }
    // catch (const int i) {
    //     Serial.println(i);
    // }
    // catch (const long l) {
    //     Serial.println(l);
    // }
    // catch (const char* p) {
    //     if (p == nullptr) {
    //         Serial.println("Exception is null");
    //         return;
    //     } else {
    //         Serial.println(p);
    //     }
    // }
    // catch (...) {
    //     Serial.println("Exception unknown");
    // }
    // Serial.print(termColor("reset") + "\n");
}

String DevUtils::repeatString(String input, int count) {
    String result;
    for (int i = 0; i < count; i++) {
        result += input;
    }
    return result;
}

void DevUtils::SerialBanners::printBootSplashBanner() {
    String output;
    // output += DevUtils::termColor("bg_white");
    // output += DevUtils::termColor("black");
    output += "\n\n::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: :::::::::\n\n";
    output += "  .  ..__. __..__.     ..__ .\n";
    output += "  |\\ |[__](__ [__]     |[__)|\n";
    output += "  | \\||  |.__)|  |  \\__||   |___\n\n";
    output += "   /$$      /$$ /$$           /$$ /$$$$$$$            /$$\n";
    output += "  | $$$    /$$$|__/          |__/| $$__  $$          | $$\n";
    output += "  | $$$$  /$$$$ /$$ /$$$$$$$  /$$| $$  \\ $$ /$$   /$$| $$  /$$$$$$$  /$$$$$$\n";
    output += "  | $$ $$/$$ $$| $$| $$__  $$| $$| $$$$$$$/| $$  | $$| $$ /$$_____/ /$$__  $$\n";
    output += "  | $$  $$$| $$| $$| $$  \\ $$| $$| $$____/ | $$  | $$| $$|  $$$$$$ | $$$$$$$$\n";
    output += "  | $$\\  $ | $$| $$| $$  | $$| $$| $$      | $$  | $$| $$ \\____  $$| $$_____/\n";
    output += "  | $$ \\/  | $$| $$| $$  | $$| $$| $$      |  $$$$$$/| $$ /$$$$$$$/|  $$$$$$$\n";
    output += "  |__/     |__/|__/|__/  |__/|__/|__/       \\______/ |__/|_______/  \\_______/\n\n";
    output += "::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: ::::::::::: :::::::::\n\n";
    output += DevUtils::termColor("reset");
    Serial.print(output);
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