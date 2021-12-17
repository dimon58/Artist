#!/bin/sh

apt-get -qq update &&
  apt-get -qq install g++ make binutils cmake libssl-dev libboost-system-dev libcurl4-openssl-dev zlib1g-dev

cd libs
git clone https://github.com/reo7sp/tgbot-cpp
git clone https://github.com/adeharo9/cpp-dotenv.git
git clone https://github.com/gabime/spdlog.git
