#include <string>
#include <csignal>
#include <cstdlib>

#include <tgbot/tgbot.h>
#include "dotenv.h"
#include "spdlog/spdlog.h"

#include "handlers/handlers.h"
#include "services/utils.h"

#include "settings.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> init_loggers(const std::string &name, const std::string &logfile) {


    try {
        std::shared_ptr<spdlog::sinks::stdout_color_sink_st> console_sink;
        console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
        console_sink->set_level(spdlog::level::debug);

        std::shared_ptr<spdlog::sinks::basic_file_sink_st> file_sink;
        file_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(logfile, false);
        file_sink->set_level(spdlog::level::info);

        spdlog::sinks_init_list sink_list = {file_sink, console_sink};

        std::shared_ptr<spdlog::logger> main_logger =
                std::make_shared<spdlog::logger>(name, sink_list.begin(), sink_list.end());

        main_logger->set_pattern(LOG_FORMATTING);
        main_logger->set_level(spdlog::level::debug);


        spdlog::register_logger(main_logger);
        spdlog::set_default_logger(main_logger);


        for (std::string logger_name: {"Handlers", "Services"}) {
            std::shared_ptr<spdlog::logger> logger =
                    std::make_shared<spdlog::logger>(logger_name, sink_list.begin(), sink_list.end());

            logger->set_pattern(LOG_FORMATTING);
            logger->set_level(spdlog::level::debug);

            spdlog::register_logger(logger);
        }

        return main_logger;
    }

    catch (const spdlog::spdlog_ex &ex) {
        std::shared_ptr<spdlog::logger> default_logger = spdlog::default_logger();
        default_logger->error("Logger initialization failed: {}", ex.what());
        return default_logger;
    }
}

int main() {
    std::shared_ptr<spdlog::logger> logger = init_loggers("MainLogger", get_current_path() + LOG_FILE);
    logger->info("Current path: {}", get_current_path());
    spdlog::flush_every(std::chrono::seconds(LOG_FLUSH_DELAY));

    auto &env = dotenv::env.load_dotenv(get_current_path() + ".env");

    logger->info("Setup bot");
    TgBot::Bot bot(env["BOT_TOKEN"]);

    logger->info("Register handlers");
    RegisterHandlers(bot);

    signal(SIGINT, [](int s) {
        spdlog::get("MainLogger")->debug("SIGINT got");
        exit(0);
    });

    try {
        logger->info("Bot username: {}", bot.getApi().getMe()->username);

        TgBot::TgLongPoll longPoll(bot);
        logger->info("Bot start");

        while (true) {
            logger->trace("Long poll started");
            longPoll.start();
        }
    }

    catch (TgBot::TgException &e) {
        logger->error(e.what());
    }

    return 0;
}