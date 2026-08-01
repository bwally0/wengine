# wengine

## Build Instructions
```bash
cmake --preset debug
cmake --build build/debug
./build/debug/wengine
```

## Manual
`Modules` are self-contained subsystems with well-defined lifecycles (`init`, `update`, `shutdown`). Each module is responsible for one domain of functionality for the engine and is driven by the core loop every frame. (e.g. `SceneModule`, `InputModule`)

`Services` are shared facilities that modules request and use to communicate or access engine infrastructure. Unlike modules, services have no lifecycle and do no per-frame work themselves. (e.g. `Time`, `EventBus`)