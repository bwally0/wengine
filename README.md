# wengine
wengine is a modular game engine built around the ECS pattern and organizes functionality into modules, resources, entities, components, and systems.

## Manual

`Modules` are self-contained subsystems with well-defined lifecycles (`init`, `update`, `shutdown`). Each module is responsible for one domain of functionality for the engine and is driven by the core loop every frame. Modules declare dependencies on other modules to control initialization and update order. (e.g. `SceneModule`, `RenderModule`, `InputModule`)

`Resources` are shared, passive objects available to all modules via the `ResourceRegistry`. They have no lifecycle and do no per-frame work. Resources are created during engine startup and provide access to engine-wide state and handles. Modules retrieve resources during `init`. (e.g. `EventBus`, `AppConfig`, `Window`)

`Entities` are unique identifiers that represent objects in the world. They have no data or behavior of their own and are used to group and identify components.

`Components` are data containers that attach to entities. Each component holds one piece of data related to the entity's state. (e.g. `Transform`, `Mesh`)

`Systems` are objects that operate on entities and components within the engine. A system reads and writes component data to produce the next frame's state. Systems have no lifecycles and own no resources.

## Build Instructions
```bash
cmake --preset debug
cmake --build build/debug
./build/debug/wengine
```

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

[Conventional Commits](https://www.conventionalcommits.org/): `<type>(<scope>): <description>`

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