# MAC352-ep1

## Folders

- **docs**: Has the problem description.
- **src**: Has the problem source code.
- **scripts**: Has the scripts for tests.

## Running

- Just run ./main 5672 and use amqp-tools commands, such as:
    amqp-declare-queue -q "q1"
    amqp-publish -r q1 -b OlaQ1
    amqp-consume -q q1 cat

## Tests

- To run the tests on /scripts you need to create a docker container as above:
    docker build -t src .
    sudo docker run --name ep1 -p 5672:5672 -p 15672:15672 src
    ./run_all_tests CONSUMER_NUMBER

