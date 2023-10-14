FROM debian:stable-slim as builder

RUN apt-get update && apt-get upgrade

RUN apt install -y build-essential libgl1-mesa-dev git wget cmake libdbus-1-3  libxcb-glx0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-render-util0 libxcb-shape0 \
    libxcb-xinerama0 libxcb-xkb1 libxkbcommon-x11-0 libxkbcommon0 libxkbcommon-x11-dev libxkbcommon-dev libx11-xcb1

RUN useradd builder -m -s /bin/bash -u 1000

WORKDIR /tmp/qt-temp
COPY ./docker-tools/qt-unified-linux-x64-4.6.1-online.run .
RUN chmod ugo+x qt-unified-linux-x64-4.6.1-online.run

USER 1000


RUN mkdir -p /home/builder/.local/share/Qt && chmod 777 /home/builder/.local/share/Qt
COPY ./docker-tools/qtaccount.ini /home/builder/.local/share/Qt/qtaccount.ini


RUN /tmp/qt-temp/qt-unified-linux-x64-4.6.1-online.run \
    --platform minimal \
    --verbose \
    --accept-licenses \
    --rootdir /home/builder/qt \
    --accept-obligations \
    --default-answer 
# install qt.qt6.660.gcc_64

ENTRYPOINT [ "tail", "-f", "/dev/null" ]