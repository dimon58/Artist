# Описание

Проект представляет из себя телеграм бота, который работает с изображениями посредством использования нейронных сетей.

1. Можно отправить боту картинку и он улучшит её качество

2. По текстовому описанию он может сгенерировать картинку

# Использование

Смотри `static/help.md`

# Требования к системе

1. Лучше всего будет операционная система **Linux**

2. Наличие видеокарты от NVIDIA с поддержкой cuda 11.3 и объёмом памяти 6 гб

   (конечно, можно использовать только процессор, но тогда некоторые функции проекта юудут выполняться непозволительно
   медленно)

3. [CUDA Toolkit 11.3](https://developer.nvidia.com/cuda-11.3.0-download-archive)

4. Около 9 Гб места на диске

# Зависимости

В проекте использованы:

1. Библиотека для создания телеграм ботов на C++ [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp)
2. Фреймворк машинного обучения [PyTorch](https://pytorch.org/) (весит около 2 ГБ)
3. Предобученная нейросеть [ruDALL-E](https://github.com/sberbank-ai/ru-dalle) для рисования картинок по текстовому
   описанию
4. Предобученная нейросеть [Real-ESRGAN](https://github.com/xinntao/Real-ESRGAN) для улучшения качества изображений
5. Предобученная нейросеть [GFPGAN (CVPR 2021)](https://github.com/TencentARC/GFPGAN) для улучшения качества лиц на
   изображении
6. Предобученная нейросеть [RetinaFace](https://github.com/TencentARC/GFPGAN) для выделения лиц на изображении

7. Еще некоторые более маленькие библиотеки: логрирования и работы с виртуальным окружением(которая создает сверхогромный оверхед, но пусть будет)
# Установка и запуск

1. Запустить скрипт, который установит все зависимости

   ```shell
   sudo sh scripts/setup.sh
   ```

2. Сборка проекта

   ```shell
   cmake .
   make -j$(nproc)
   ```

3. Запуск

   ```shell
   ./Artist
   ```

# Запуск через Docker

Для этого должен быть установлен [Docker](https://www.docker.com/) и [NVIDIA Container Toolkit](https://github.com/NVIDIA/nvidia-docker)

1. Сборка образа (получится около 11.5 ГБ, скорость выполнения напрямую зависит от скорости интернета, так как докеру надо будет выкачать около 5 ГБ из сети)

   `docker build -t artist_bot .`

2. Запуск

   `docker run --rm --gpus all --name artist_bot_container -v artist_bot_logs:/usr/src/app/logs artist_bot`

3. Остановка

   `docker stop artist_bot_container`