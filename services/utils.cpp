#include <string>
#include <filesystem>

#include "utils.h"

#include "../settings.h"

std::string get_approx_time(int w, int h) {

    double a = 0.024538 * GPU_FP32_PERFORMANCE / 1.911;
    double b = 9.79244;

    int time = (int) ((double) w * (double) h * a / 1000 + b);

    int seconds = time % 60;
    int minutes = time / 60;

    std::string approx_time = "Ожидаемое время";

    if (minutes > 0) {
        approx_time += " " + std::to_string(minutes) + " мин";
    }

    if (seconds > 0) {
        approx_time += " " + std::to_string(seconds) + " сек";
    }

    return approx_time;
}

// Костыль
std::string get_current_path() {
    std::filesystem::path cwd = std::filesystem::current_path().string();
    if (cwd.string().ends_with("/cmake-build-debug")) {
        return cwd.parent_path().string() + "/";
    }

    return cwd.string() + "/";
}


