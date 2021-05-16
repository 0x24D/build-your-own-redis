# build-your-own-redis

Redis server clone written in C++20. Initially based on [Build Your Own Redis](https://rohitpaulk.com/articles/redis-0) Ruby tutorial.

Implemented commands:
|Command|Arguments|Implemented|Comments|
---|---|---|---
|COMMAND||:green_circle:||
|COMMAND|COUNT|:green_circle:||
|COMMAND|GETKEYS|:red_circle:||
|COMMAND|INFO [command-name...]|:green_circle:||
|ECHO|message|:yellow_circle:|Need to handle error when wrong number of arguments provided|
|GET|key|:red_circle:||
|PING||:green_circle:||
|PING|message|:yellow_circle:|Need to handle error when wrong number of arguments provided|
|SET|key value|:red_circle:||

Additional resources:
* [Redis documentation](https://redis.io/documentation)
* [Redis commands](https://redis.io/commands)
* [Redis protocol](https://redis.io/topics/protocol)
