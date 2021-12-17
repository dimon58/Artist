#include "base.h"

#include <tgbot/tgbot.h>
#include "spdlog/spdlog.h"


void RegisterBaseHandlers(TgBot::Bot &bot) {

    bot.getEvents().onCommand("start", [&bot](const TgBot::Message::Ptr &message) {
//        logger->info("Start command request by {}", message->chat->username); // какая-то непонятная ошибка на этой строчке
        spdlog::get("Handlers")->info("Start command request by {}", message->chat->username);
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });

}