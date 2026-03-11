# Raytracer

# Initialize devcontainer
```bash
devcontainer up
devcontainer exec bash
```

## Install dependencies
```bash
vcpkg install
```

## How to build
```bash
cmake --preset default
cmake --build build
```

## How to test
```bash
cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure
```

## TODO:
- [ ] Define a standard for names, methods, and classes.
- [ ] Implement loading and saving for all Netpbm formats.
- [ ] Unify the image upload and save library.
- [ ] Add a gradient option with multiple color points.