# Console chess engine
Simple console chess engine. Just for fun. You can run chess server locally or use my own remote server (while it works).

## Preview
<img src="assets/preview.jpg" alt="preview" width="50%" />

## Run Locally
For a start clone the repository:

```bash
git clone --recurse-submodules https://github.com/f01zy/Chess
```

Next compile client with cmake:

```bash
cd client
mkdir build
cd build
cmake ..
cmake --build .
```

If you want to run server locally, you need to install [bun](https://bun.com/):

```bash
cd server
bun --port=3000 index.ts
```
