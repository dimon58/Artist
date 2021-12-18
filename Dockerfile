FROM nvidia/cuda:11.3.1-base-ubuntu20.04 as torch

# install python3.9 and curl
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3.9 \
    python3.9-dev \
    python3.9-distutils \
    python3.9-venv \
    gcc \
    curl \
    wget \
    git

# install pip
RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
    python3.9 get-pip.py

WORKDIR /usr/src/app

# install pytorch (~1900 MB)
RUN python3.9 -m venv venv && \
    . venv/bin/activate && \
    pip install torch==1.10.0+cu113 \
        torchvision==0.11.1+cu113 \
        torchaudio==0.10.0+cu113 \
        -f https://download.pytorch.org/whl/cu113/torch_stable.html

# install pip
RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py && python3.9 get-pip.py && rm get-pip.py

FROM torch

RUN mkdir "libs" && \
    mkdir "services" && \
    mkdir "services/nns" && \
    mkdir "services/nns/pretrained_models" && \
    mkdir "tmp" && \
    mkdir "tmp/inputs" && \
    mkdir "tmp/outputs" && \
    mkdir "logs" && \
    mkdir "scripts"

#######################################################################################################################

COPY scripts/get_cpplibs.sh scripts

RUN sh scripts/get_cpplibs.sh

COPY requirements.txt .
RUN python3.9 -m venv venv && \
    . venv/bin/activate && \
    pip install -r requirements.txt

COPY scripts/get_realesrgan.sh scripts
RUN sh scripts/get_realesrgan.sh

RUN cp "venv/lib/python3.9/site-packages/facexlib/weights/detection_Resnet50_Final.pth" \
  "/usr/src/app/venv/lib/python3.9/site-packages/facexlib-0.2.1.1-py3.9.egg/facexlib/weights/detection_Resnet50_Final.pth" && \
    cp "venv/lib/python3.9/site-packages/gfpgan/weights/GFPGANCleanv1-NoCE-C2.pth" \
    "/usr/src/app/venv/lib/python3.9/site-packages/gfpgan-0.2.4-py3.9.egg/gfpgan/weights/GFPGANCleanv1-NoCE-C2.pth"

COPY scripts/get_rudalle.sh scripts
COPY scripts/download_rudalle.py scripts
RUN sh scripts/get_rudalle.sh

#######################################################################################################################

COPY . .
RUN cmake .
RUN make -j$(nproc)

VOLUME ./logs /usr/src/app/logs

CMD ["./Artist"]