IOT Rest Server based on boost beast and [beast_http_server](https://github.com/0xdead4ead/beast_http_server)

This is a Visual Studio 2017 Solution, it is planned to build it on a Raspberry PI, too.

IotHttpServer is a configurable HTTP server able to serve files and REST endpoints.

SerialHost communicates with an Arduion Nano using a serial port over USB. The Arduino has got
a IO Hardware with Relais and Inputs.

[Boost](https://www.boost.org/) is needed to compile this, currently V1.68.0. Download and build it as described on the boost page.
The VC++ Directories has to be modified to find the Boost Headers and Libs.
