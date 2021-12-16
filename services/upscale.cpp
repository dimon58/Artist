#include <string>
#include <cstdlib>

#include "utils.h"
#include "upscale.h"

const std::string CURRENT_PATH = get_current_path();

const std::string REALESRGAN_PATH = CURRENT_PATH + "services/nns/inference_realesrgan.py";

int upscale(const std::string &input, const std::string &output_folder, const std::string &prefix,
            const std::string &suffix) {

    std::string s = ". " + CURRENT_PATH + "venv/bin/activate && " +
                    "python " + REALESRGAN_PATH +
                    " -i " + input +
                    " -o " + output_folder +
                    " --prefix " + prefix +
                    " --suffix " + suffix +
                    " -t 350 " +
                    "--face_enhance";

    return system(s.c_str());
}