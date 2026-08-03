# wengine

## Build Instructions
```bash
cmake --preset debug
cmake --build build/debug
./build/debug/wengine
```

## Manual
wengine is a modular game engine built around the ECS pattern. Functionality is organized into distinct modules, services, entities, components, and systems. 

`Modules` are self-contained subsystems with well-defined lifecycles (`init`, `update`, `shutdown`). Each module is responsible for one domain of functionality for the engine and is driven by the core loop every frame. (e.g. `SceneModule`, `InputModule`)

`Services` are shared facilities that modules request and use to communicate or access engine infrastructure. Unlike modules, services have no lifecycle and do no per-frame work themselves. (e.g. `Time`, `EventBus`)

`Entities` are unique identifiers that represent objects in the world. They have no data or behavior of their own and are used to group and identify components.

`Components` are data containers that attach to entities. Each component holds one piece of data related to the entity's state. (e.g. `TransformComponent`, `MeshComponent`)

`Systems` are objects that operate on entities and components within the engine. A system reads and writes component data to produce the next frame's state. Systems have no lifecycles and own no resources. (e.g. `RenderSystem`, `PhysicsSystem`)

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