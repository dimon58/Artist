#include <cstdio>
#include <tgbot/tgbot.h>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "../services/upscale.h"
#include "../services/utils.h"

#include "../settings.h"


const std::string CURRENT_PATH = get_current_path();


void RegisterNNSHandlers(TgBot::Bot &bot) {

    bot.getEvents().onAnyMessage([&bot](const TgBot::Message::Ptr &message) {
        std::cout << "User wrote " << message->text;
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);

        if (!message->photo.empty()) {
            auto file_id = message->photo[message->photo.size() - 1]->fileId;
            std::int32_t w = message->photo[message->photo.size() - 1]->width;
            std::int32_t h = message->photo[message->photo.size() - 1]->height;
            auto file_info = bot.getApi().getFile(file_id);


            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            std::string str = to_string(uuid);

            std::string input_image_name = "input_image_" + str + ".jpg";
            std::string input_image = CURRENT_PATH + TEMP_FOLDER + "inputs/" + input_image_name;
            std::string output_image = CURRENT_PATH + TEMP_FOLDER + "outputs/" + input_image_name;

            auto downloaded = bot.getApi().downloadFile(file_info->filePath);
            std::ofstream out(input_image, std::ios::binary);
            out << downloaded;

            bot.getApi().sendMessage(message->chat->id, "Start upscaling");
            bot.getApi().sendMessage(message->chat->id, get_approx_time(w, h));

            upscale(
                    input_image,
                    CURRENT_PATH + TEMP_FOLDER + "outputs",
                    "\"\"",
                    "\"\""
            );

            bot.getApi().sendPhoto(
                    message->chat->id,
                    TgBot::InputFile::fromFile(
                            output_image, "image/jpeg"
                    )
            );

            remove(input_image.c_str());
            remove(output_image.c_str());
        }

    });

}