Initially Create a build directory

```bash
cmake -S .  -B ./build
```

To build and compile a .dvss file

```bash
cmake --build build/ && ./build/dvss <filename>.dvss
```

To run the executable

```bash
./out
```

To check the return status of executable run this after executing to verify
```bash
echo $?
```
