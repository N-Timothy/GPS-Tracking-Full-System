FROM ubuntu:22.04

USER 0

RUN apt-get update 
RUN apt-get -y install git cmake g++ gcc libbson-1.0-0 libbson-dev libmongoc-1.0-0 libmongoc-dev libmosquitto-dev libmosquitto1 nlohmann-json3-dev curl
RUN mkdir driver && cd driver && pwd && cd && \
   curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.0/mongo-cxx-driver-r3.7.0.tar.gz && \
   tar -xzf mongo-cxx-driver-r3.7.0.tar.gz && \
   cd mongo-cxx-driver-r3.7.0/build && pwd && \ 
   cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local && \
   cmake --build . --target EP_mnmlstc_core && \
   cmake --build . && cmake --build . --target install  && \
    cd /usr/local/include/mongocxx/v_noabi && mv mongocxx ../../mongocxx2 && cd ../.. && rm -r mongocxx && mv mongocxx2 mongocxx && \
    cd /usr/local/include/bsoncxx/v_noabi && mv bsoncxx ../../bsoncxx2 && cd ../.. && rm -r bsoncxx && mv bsoncxx2 bsoncxx && \
    cd 
RUN git clone https://github.com/N-Timothy/GPS-Tracking-Full-System.git && cd GPS-Tracking-Full-System && ./build.sh

WORKDIR /
