FROM alpine
WORKDIR /app
COPY . .
RUN apk add build-base
RUN apk add libuv-dev
RUN mkdir -p logs
RUN make clean ; make
RUN chmod +x /app/server.bin
EXPOSE 5002
EXPOSE 5002/udp
ENTRYPOINT ["/app/server.bin"]
