FROM alpine
WORKDIR /app
COPY . .
RUN apk add libuv
RUN apk add build-base
RUN make clean ; make
RUN chmod +x /app/server.bin
EXPOSE 5002
ENTRYPOINT ["/app/server.bin"]
