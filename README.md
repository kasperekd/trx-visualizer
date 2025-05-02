```bash
sudo apt install libsdl2-dev libgl1-mesa-dev libglew-dev
```

```bash
git submodule update --recursive
```

```bash
mkdir build && cd build
cmake .. -DBUILD_TEST=ON
make -j$(nproc)
```