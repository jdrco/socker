# Socker

This project contains a simple socket-based client-server application. The client and server programs are written in C and utilize a common utility module.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

## Prerequisites

Before building this project, ensure you have CMake and a C compiler (like GCC or Clang) installed on your system.

## Building the Project

```
git clone git@github.com:jdrco/socker.git
cd socker
mkdir build && cd build
cmake ..
make
```

## Running the Project

Ensure you are in the root directory.

Run the server in a separate terminal.
```
build/src/server
```

Run client(s) in a separate terminal.
```
build/src/client
```
