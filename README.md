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

## Contributing

### Code Style

| Category | Convention | Example |
|---|---|---|
| Classes | PascalCase | `ModuleRegistry`, `EventBus` |
| Interfaces | IPascalCase | `IModule` |
| Template parameters | TPascalCase | `TEvent`, `TService` |
| Methods | camelCase | `registerModule()`, `initGLFW()` |
| Parameters | camelCase | `deltaTime`, `callback` |
| Local variables | camelCase | `frameTime`, `currentTime` |
| Member variables | m_camelCase | `m_sortedModules`, `m_window` |
| Constants | SCREAMING_SNAKE | `UPDATE_INTERVAL` |
| Files | PascalCase | `ModuleRegistry.h`, `Application.cpp` |

### Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/): `<type>(<scope>): <description>`

| Tag | Use |
|---|---|
| `feat` | New feature |
| `fix` | Bug fix |
| `refactor` | Code change that isn't a fix or feature |
| `chore` | Build system, dependencies, tooling |
| `docs` | Documentation only |
| `test` | Adding or fixing tests |
| `perf` | Performance improvement |
| `style` | Formatting, whitespace, no logic change |

Scope is the area of the codebase affected: `core`, `renderer`, `physics`, `input`, etc.