FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    libminiupnpc-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN g++ FileTransferServer.cpp \
    -std=c++17 \
    -lminiupnpc \
    -o server

ENV PORT=55387
EXPOSE 55387

CMD ["./server"]