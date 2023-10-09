# MAC352-ep1

## Folders

- **docs**: Has the problem description.
- **src**: Has the problem source code.
- **scripts**: Has the scripts for tests.

## Running

- Run make
- Run ./main 5672 and use amqp-tools commands, such as:
- Run amqp-declare-queue -q "q1"
- Run amqp-publish -r q1 -b OlaQ1
- Run amqp-consume -q q1 cat

## Tests

- To run the tests on /scripts you need to create a docker container as above:
- Run docker build -t src .
- Run sudo docker run --name ep1 -p 5672:5672 -p 15672:15672 src
- Run ./run_all_tests CONSUMER_NUMBER

