# Sparkle Engine - Unified Logging Style Guide

This document defines the unified logging conventions used across the Sparkle Engine
for both batch scripts and C++ code.

## Separator Characters

| Context | Width | Pattern |
|---------|-------|---------|
| Code comments (file headers) | 76 chars | `:: ============================================================================` |
| UI/Output boxes | 60 chars | `============================================================` |
| Section dividers (code) | 75 chars | `:: ---------------------------------------------------------------------------` |

## Batch Script Prefixes

| Prefix | Purpose | Example |
|--------|---------|---------|
| `[LOG]` | General informational messages | `[LOG] Building project...` |
| `[OK]` | Individual check/operation passed | `[OK] CMake` |
| `[SUCCESS]` | Major operation completed successfully | `[SUCCESS] Build completed.` |
| `[WARN]` | Non-fatal issues, optional features missing | `[WARN] Clang not found` |
| `[ERROR]` | Fatal errors that stop execution | `[ERROR] CMake failed` |
| `[CLEAN]` | File/folder removal actions | `[CLEAN] Removing: build\` |
| `[SKIP]` | Operation intentionally skipped | `[SKIP] Already exists` |
| `[MODIFIED]` | File was changed (clang-format) | `[MODIFIED] src\main.cpp` |
| `[SCAN]` | Progress indicator | `[1/100] Processing file...` |

## C++ Log Levels

| Macro | Tag Width | Purpose |
|-------|-----------|---------|
| `LOG_TRACE` | `[TRACE]   ` | Frame-by-frame diagnostics (debug builds only) |
| `LOG_DEBUG` | `[DEBUG]   ` | Developer-focused flow/state info |
| `LOG_INFO` | `[INFO]    ` | High-level runtime events |
| `LOG_WARNING` | `[WARNING] ` | Unexpected but recoverable conditions |
| `LOG_ERROR` | `[ERROR]   ` | Non-fatal failures |
| `LOG_FATAL` | `[FATAL]   ` | Unrecoverable errors (terminates) |

**Note:** C++ tags are fixed-width (10 chars) for visual alignment in logs.

## Menu Box Format (Batch Scripts)

```
============================================================
  Menu Title
============================================================

  1) First option
  2) Second option
  3) Third option

============================================================
Enter choice [1-3]: 
```

- 60-character separators
- Title indented with 2 spaces
- Options indented with 2 spaces
- Prompt appears after closing separator
- "All" options should be **last** in the list

## Summary Box Format

```
============================================================
  [SUCCESS] Operation completed.
============================================================
```

Or with details:

```
============================================================
  ClangFormat Summary
============================================================

  Scanned:  100 files
  Modified: 5 files

============================================================
```

## Mapping Between Batch and C++

| Batch | C++ Equivalent | Notes |
|-------|----------------|-------|
| `[LOG]` | `LOG_INFO` | General information |
| `[OK]` | `LOG_INFO` | Inline success checks |
| `[SUCCESS]` | `LOG_INFO` | Major completion |
| `[WARN]` | `LOG_WARNING` | Non-fatal issues |
| `[ERROR]` | `LOG_ERROR` / `LOG_FATAL` | Depends on severity |

## Best Practices

### Batch Scripts
1. Use logging bootstrap (`BootstrapLog.bat`) for all user-facing scripts
2. Preserve `PARENT_BATCH` across `endlocal` when calling `:FINISH`
3. Always show `[LOG] Logs: %LOGFILE%` before pause
4. Use `choice` or `set /P` for prompts, never `<nul set /P` patterns

### C++ Code
1. Include context prefix: `LOG_INFO("Renderer: Initializing...")`
2. Use `LOG_FATAL` only for truly unrecoverable states
3. Use `CHECK(hr)` macro for HRESULT validation
4. Prefer `LOG_WARNING` over `LOG_ERROR` for recoverable failures
