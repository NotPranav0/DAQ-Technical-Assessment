
FROM ubuntu:22.04 AS dev
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /app

RUN apt-get update && \
    apt-get install -y python3-pip \
    wget autoconf libtool git make g++ valgrind \
    cmake can-utils clangd catch2 libxml2-dev

RUN git clone --recurse-submodules https://github.com/xR3b0rn/dbcppp.git && \
    cd dbcppp && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j && \
    make RunTests && \
    make install && \
    ldconfig

COPY . .

RUN find . -name "CMakeCache.txt" -delete

RUN cd build/ && rm CMakeCache.txt || true

RUN cd build/ && cmake ..

RUN cd build/ && make -j    

# ENTRYPOINT ["./build/solution/answer"]
ENTRYPOINT ["bash"]