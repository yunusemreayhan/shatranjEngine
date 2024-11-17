FROM ubuntu:24.04

RUN apt update
RUN apt install libgtest-dev -y
RUN apt install libgmock-dev -y
RUN apt install cmake -y
RUN apt install build-essential libssl-dev -y

WORKDIR /

COPY ./* /shatranjEngine/.

RUN cd /shatranjEngine && mkdir -p build/release && cmake -S . -B ./build/release -DCMAKE_BUILD_TYPE=Release && cmake --build build/release --target fen_calculator
RUN ls -al /shatranjEngine/build/release/src
RUN cd / && mv /shatranjEngine/build/release/src/bin/fencalc/fen_calculator /

ENTRYPOINT ["/fen_calculator 8 2048 5 \"8/8/8/1k6/8/1KQ5/8/q7 w - - 0 1\""]
