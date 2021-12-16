#include <tgbot/tgbot.h>
#include <string>
#include <csignal>
#include <cstdlib>
#include <iostream>

#include "handlers/handlers.h"
#include "services/utils.h"

#include "settings.h"


int main() {
    std::cout << "CURRENT_PATH " << get_current_path() << std::endl;

    TgBot::Bot bot(BOT_TOKEN);

    RegisterHandlers(bot);

    signal(SIGINT, [](int s) {
        std::cout << "SIGINT got" << std::endl;
        exit(0);
    });

    try {
        std::cout << "Bot username: " + bot.getApi().getMe()->username << std::endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "Long poll started\n";
            longPoll.start();
        }
    } catch (TgBot::TgException &e) {
        std::cout << "error: " << e.what() << std::endl;
    }

    return 0;
}