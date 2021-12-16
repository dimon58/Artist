#include <cstdio>
#include <string>
#include <stdexcept>
#include <cstring>
#include <filesystem>

#include "utils.h"

std::string get_approx_time(int w, int h) {
    int time = w * h * 155 / (960 * 1280);

    int seconds = time % 60;
    int minutes = time / 60;

    std::string approx_time = "Expected time";

    if (minutes > 0) {
        approx_time += " " + std::to_string(minutes) + " min";
    }

    if (seconds > 0) {
        approx_time += " " + std::to_string(seconds) + " sec";
    }

    return approx_time;
}

std::string exec(const char *cmd) {
    char buffer[128];
    std::string result = "";
    FILE *pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

// Костыль
std::string get_current_path() {
    std::filesystem::path cwd = std::filesystem::current_path().string();
    if (cwd.string().ends_with("/cmake-build-debug")) {
        return cwd.parent_path().string() + "/";
    }

    return cwd.string() + "/";
}


