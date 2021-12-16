#include "base.h"

#include <tgbot/tgbot.h>

void RegisterBaseHandlers(TgBot::Bot &bot) {

    bot.getEvents().onCommand("start", [&bot](const TgBot::Message::Ptr &message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });

}