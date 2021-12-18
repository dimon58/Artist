#include <cstdio>
#include <iostream>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <tgbot/tgbot.h>
#include "spdlog/spdlog.h"

#include "../services/upscale.h"
#include "../services/utils.h"

#include "../settings.h"


const std::string CURRENT_PATH = get_current_path();
const std::string RUDALLE_PATH = CURRENT_PATH + "services/nns/inference_rudalle.py";


void RegisterNNSHandlers(TgBot::Bot &bot) {


    bot.getEvents().onCommand("draw", [&bot](const TgBot::Message::Ptr &message) {
        auto logger = spdlog::get("Handlers");
        logger->info("Draw \"{}\" request by {}", message->text, message->chat->username);

        if (message->text.size() == 5) {
            bot.getApi().sendMessage(message->chat->id, "Напишите описание изображения. /draw *описание*");
            return;
        }

        std::string text = message->text.substr(6, message->text.size() - 6);

        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::string str = to_string(uuid);

        std::string image = "image_" + str + ".jpg";
        std::string image_path = CURRENT_PATH + TEMP_FOLDER + image;

        std::string cmd = ". " + CURRENT_PATH + "venv/bin/activate && " +
                          "python " + RUDALLE_PATH +
                          " -t " + text +
                          " -o " + image_path;

        bot.getApi().sendMessage(message->chat->id, "Скорее всего рисование идет на процессоре, поэтому ждите час");


        logger->debug("Call services/nns/inference_rudalle.py with text {}", text);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        system(cmd.c_str());
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        logger->debug(
                "Image {} generated in {} sec", image,
                (float) std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000
        );
        logger->debug("Drawing {} complete", image);

        try {
            bot.getApi().sendPhoto(
                    message->chat->id,
                    TgBot::InputFile::fromFile(
                            image_path, "image/jpeg"
                    )
            );
        }
        catch (const std::ios_base::failure &) {
            logger->error("Drawing error: output file {} couldn't be loaded. File id: {}", image_path);
            bot.getApi().sendMessage(
                    message->chat->id,
                    "Ошибка при рисовании изображения, свяжитесь с техподдержкой, которой нет."
            );
        }

        logger->debug("Deleting temp files from drawing");
        remove(image_path.c_str());

    });


    bot.getEvents().onAnyMessage([&bot](const TgBot::Message::Ptr &message) {
        if (message->photo.empty()) {
            return;
        }

        auto logger = spdlog::get("Handlers");
        logger->info("Photo upscale request by {}", message->chat->username);

        std::string file_id = message->photo[message->photo.size() - 1]->fileId;
        std::int32_t w = message->photo[message->photo.size() - 1]->width;
        std::int32_t h = message->photo[message->photo.size() - 1]->height;
        TgBot::File::Ptr file_info = bot.getApi().getFile(file_id);


        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::string str = to_string(uuid);

        std::string input_image_name = "input_image_" + str + ".jpg";
        std::string input_image = CURRENT_PATH + TEMP_FOLDER + "inputs/" + input_image_name;
        std::string output_image = CURRENT_PATH + TEMP_FOLDER + "outputs/" + input_image_name;


        try {
            std::string downloaded = bot.getApi().downloadFile(file_info->filePath);
            std::ofstream out(input_image, std::ios::binary);
            out << downloaded;
            logger->debug("Downloaded image. File id: {}", file_id);
        }
        catch (const std::ios_base::failure &) {
            logger->error("Downloaded image error. File id: {}", file_id);
            bot.getApi().sendMessage(
                    message->chat->id, "Ошибка при обработке изображения, свяжитесь с техподдержкой, которой нет.");
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Запуск улучшения");
        bot.getApi().sendMessage(message->chat->id, get_approx_time(w, h));


        logger->debug("Start upscaling. File id: {}", file_id);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        upscale(
                input_image,
                CURRENT_PATH + TEMP_FOLDER + "outputs",
                "\"\"",
                "\"\""
        );
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        logger->debug(
                "{} sec packed in a image with a size of {}x{} = {} KPX",
                (float) std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000,
                w, h, (float) (w * h) / 1000
        );
        logger->debug("Upscaling complete. Most likely saved to {}. File id: {}", output_image, file_id);

        try {
            bot.getApi().sendPhoto(
                    message->chat->id,
                    TgBot::InputFile::fromFile(
                            output_image, "image/jpeg"
                    )
            );
        }
        catch (const std::ios_base::failure &) {
            logger->error(
                    "Upscaling error: output file {} couldn't be loaded. File id: {}",
                    output_image, file_id
            );
            bot.getApi().sendMessage(
                    message->chat->id,
                    "Ошибка при обработке изображения, свяжитесь с техподдержкой, которой нет."
            );
        }

        logger->debug("Deleting temp files from upscaling {}", file_id);
        remove(input_image.c_str());
        remove(output_image.c_str());

    });

}