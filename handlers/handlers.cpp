#include "handlers.h"

#include <tgbot/tgbot.h>

#include "base.h"
#include "nns.h"

void RegisterHandlers(TgBot::Bot &bot) {
    RegisterBaseHandlers(bot);
    RegisterNNSHandlers(bot);
}

