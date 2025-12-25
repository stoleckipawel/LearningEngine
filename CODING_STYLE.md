# LearningEngine Coding Style

This document captures the coding conventions used across the engine. The repository-level tooling (`.clang-format`, `.clang-tidy`) is authoritative — editors, CI, and local tooling should follow those files.

## Tooling

- **Authoritative configs**: The canonical configs live at the repo root:
  - **.clang-format** — formatting rules for C++ and HLSL
  - **.clang-tidy** — static-analysis checks and options

- **Use the wrapper**: Prefer the provided `RunClangFormat.bat` to apply formatting consistently across environments.

- **Quick commands (Windows PowerShell)**:

```powershell
.\RunClangFormat.bat
# optionally inspect the log at Logs/LogClangFormat.txt
``` 

## Formatting (authoritative: `.clang-format`)

The following settings are the exact, authoritative formatting options from the repository `.clang-format`. Keep these in sync with the file at the repo root.

- **Base style**: BasedOnStyle: LLVM
- **Language**: Cpp
- **Indentation**: tabs for indentation (UseTab: ForIndentation)
  - `IndentWidth`: 4
  - `TabWidth`: 4
  - `ContinuationIndentWidth`: 4
- **Line length**: `ColumnLimit: 140`
- **Braces**: `BreakBeforeBraces: Allman` (Allman style)
- **Constructor initializer lists**:
  - `ConstructorInitializerAllOnOneLineOrOnePerLine: true`
  - `BreakConstructorInitializers: AfterColon`
  - `ConstructorInitializerIndentWidth: 4`
- **Bin-packing**: `BinPackParameters: false`, `BinPackArguments: false` (prefer one item per line when wrapped)
- **Pointer/reference alignment**: `PointerAlignment: Left`, `ReferenceAlignment: Left`
- **Include handling**: `SortIncludes: false`, `IncludeBlocks: Preserve`
- **Lambdas**: `LambdaBodyIndentation: Signature` and `AllowShortLambdasOnASingleLine: None`
- **Comments**: `ReflowComments: false`

If you need a quick reference, open `.clang-format` at the repo root — that file is the source of truth.

## Practical editor rules

- Configure your editor/IDE to: use the repository `.clang-format` and run formatting on save where possible.
- Disable editor-specific format overrides that conflict with the repo config.

## File layout

### Headers

- Keep headers self-contained and `#include` what they use.
- Prefer forward declarations over includes where practical.

### Source files

- Translation units should include the PCH first when available (e.g. `#include "PCH.h"`), then the matching header, then engine headers, then system/third-party headers.

## Includes

- Group includes with a blank line between groups (PCH / engine / third-party / system).
- Prefer engine headers with quotes and system/third-party headers with angle brackets.

## Naming

- **Types** (classes/structs/enums): `PascalCase`
- **Functions and methods**: `PascalCase`
- **Local variables**: `camelCase`
- **Member variables**: `m_` prefix, `camelCase` afterwards (e.g. `m_device`, `m_frameIndex`)
- **Boolean naming**:
  - Parameters: `bSomething`
  - Members: `m_bSomething`
- **Global singletons**: `GName`

## Comments

- Comments should describe intent, invariants, constraints, and non-obvious tradeoffs.
- Prefer concise section separators for large functions:
  - `// -----------------------------------------------------------------------------`
- Avoid redundant comments that merely restate the identifier.

## Error handling

- Use `CHECK(hr)` / `LOG_FATAL(...)` for unrecoverable initialization failures.
- Prefer returning status only at module boundaries; internally keep code simple and fail fast.

## Performance and memory

- Avoid per-frame heap allocations in hot paths.
- Prefer value types and stack allocations for transient data.
- Use RAII for resource lifetime; avoid shared ownership by default.

---

