#!/bin/sh

# install python3.9 and curl
apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
  python3.9 \
  python3.9-dev \
  python3.9-distutils \
  python3.9-venv \
  curl \
  wget \
  git \
  gcc

# install pip
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py && python3.9 get-pip.py && rm get-pip.py

# install pytorch (~1900 MB)
python3.9 -m venv venv &&
  . venv/bin/activate &&
  pip install cython wheel &&
  pip install -r requirements.txt &&
  pip install torch==1.10.0+cu113 \
    torchvision==0.11.1+cu113 \
    torchaudio==0.10.0+cu113 \
    -f https://download.pytorch.org/whl/cu113/torch_stable.html

#######################################################################################################################

mkdir "libs"
mkdir "services"
mkdir "services/nns"
mkdir "services/nns/pretrained_models"

mkdir "tmp"
mkdir "tmp/inputs"
mkdir "tmp/outputs"

mkdir "logs"

#######################################################################################################################

sh scripts/get_cpplibs.sh

sh scripts/get_realesrgan.sh
sh scripts/get_rudalle.sh
