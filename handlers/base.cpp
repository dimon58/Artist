#include "fstream"

#include "base.h"

#include <tgbot/tgbot.h>
#include "spdlog/spdlog.h"

#include "../services/utils.h"

const std::string CURRENT_PATH = get_current_path();


void RegisterBaseHandlers(TgBot::Bot &bot) {

    bot.getEvents().onCommand("start", [&bot](const TgBot::Message::Ptr &message) {
//        logger->info("Start command request by {}", message->chat->username); // какая-то непонятная ошибка на этой строчке
        spdlog::get("Handlers")->info("Start command request by {}", message->chat->username);
        bot.getApi().sendMessage(message->chat->id, "Привет! Чтобы узнать что я могу напиши /help");
    });

    bot.getEvents().onCommand({"help", "h"}, [&bot](const TgBot::Message::Ptr &message) {
        auto logger = spdlog::get("Handlers");
        logger->info("Help command request by {}", message->chat->username);

        std::ifstream help_text_file;
        help_text_file.open(CURRENT_PATH + "static/help.md", std::ios::in);

        if (help_text_file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(help_text_file)),
                                (std::istreambuf_iterator<char>()));
            bot.getApi().sendMessage(message->chat->id, content);

        } else {
            logger->error("Can't open file with help text static/help.md");
            bot.getApi().sendMessage(message->chat->id, "Что-то пошло не так");
        }

    });

}