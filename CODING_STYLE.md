# LearningEngine Coding Style

This document captures the coding conventions used across the engine. The goal is consistent, readable, high-performance C++ with minimal overhead and predictable ownership.

## Tooling

- Formatting is enforced with clang-format using .clang-format at repo root.
- Format command (Windows PowerShell):
  - `clang-format -i (Get-ChildItem engine -Recurse -Include *.h,*.cpp | Where-Object { $_.FullName -notmatch '\\third_party\\' } | ForEach-Object { $_.FullName })`

## Formatting

- Braces: Allman style.
- Indentation: tabs for indentation (tab width 4). Use spaces only for alignment inside a line.
- Line length: target 180 columns. Prefer breaking before it becomes hard to scan.
- One statement per line. Always use braces for multi-line blocks.

## File layout

### Headers

- Keep headers self-contained and include what they use.
- Prefer forward declarations over includes where practical.

### Source files

- Translation units include the PCH first when available:
  - `#include "PCH.h"`
- Then include the matching header (`Foo.h`), then other engine headers, then system/third-party headers.

## Includes

- Group includes with a blank line between groups.
- Prefer engine headers with quotes, system headers with angle brackets.
- Do not rely on include order for correctness (except PCH).

## Naming

- Types (classes/structs/enums): `PascalCase`.
- Functions and methods: `PascalCase`.
- Local variables: `camelCase`.
- Member variables: `m_` prefix, `camelCase` afterwards (e.g. `m_device`, `m_frameIndex`).
- Boolean naming:
  - Parameters: `bSomething`.
  - Members: `m_bSomething`.
- Global singletons: `GName`.
- Backend-specific globals: prefix with backend (e.g. `GD3D12Rhi`).
- Backend-specific types/files: prefix with backend (`D3D12*`, `Vulkan*`, `Metal*` etc).

## Comments

- Comments describe intent, invariants, constraints, and non-obvious tradeoffs.
- Prefer concise section separators for large functions:
  - `// -----------------------------------------------------------------------------`
- Avoid redundant comments that restate the identifier.
- Don’t use documentation tags (`@brief`, `@param`, etc.). Write directly for developers.

## Error handling

- Use `CHECK(hr)`/`LOG_FATAL(...)` for unrecoverable initialization failures.
- Prefer returning status only at module boundaries; internally keep code simple and fail fast.

## Performance and memory

- Avoid per-frame heap allocations in hot paths.
- Prefer value types and stack allocations for transient data.
- Use RAII for resource lifetime; avoid shared ownership by default.
