#!/bin/sh

. ./venv/bin/activate

cd services/nns
git clone https://github.com/sberbank-ai/ru-dalle.git

cd ru-dalle
pip install torch==1.10.0+cu113 \
  torchvision==0.11.1+cu113 \
  torchaudio==0.10.0+cu113 \
  -f https://download.pytorch.org/whl/cu113/torch_stable.html \
  pip install -r requirements.txt
python setup.py develop
cd ..
cd ..
cd ..

echo "Downloading pretrained models"
python scripts/download_rudalle.py
