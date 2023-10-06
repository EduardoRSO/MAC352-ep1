FROM gcc:11.4.0

WORKDIR /MAC352-ep1

COPY . .

RUN gcc -o ep1 src/main.c src/amqp.c src/hardcode.c src/queue.c

EXPOSE 5672

CMD ["./ep1", "5672"]
