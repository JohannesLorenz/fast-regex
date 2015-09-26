# 0 Greetings
Welcome to the installation!

**Contents**

  1. Requirements
  2. Installation
  3. Realtime check with stoat
  4. Running
  5. Debugging

# 1 Requirements
You will only need the standard C and C++ libraries.

# 2 Installation (all TODO)
The initial installation and updating are different processes.

For the initial installation, type in this directory:
```sh
git submodule init
git submodule update

mkdir build
cd build
# for a release build using clang (suggested)
cmake	-DCOMPILER=clang \
	-DCMAKE_BUILD_TYPE=Release \
	..
```

In order to update the code, go to the main directory and type
```sh
./update
```

# 4 Running (all TODO)
TODO
```sh
./src/core
```

# 5 Debugging
TODO

