# System Modules & Responsibilities

The codebase consists of five core decoupled modules implemented in standard C++17:

## 1. Process Manager (`ProcessManager.h`, `ProcessManager.cpp`)
- **Responsibilities**:
  - Creates simulated operating system processes.
  - Automatically generates and assigns unique sequential Process IDs (PIDs, starting at 101).
  - Maintains process metadata (`pid`, `name`, `creationTime`).
  - Supports process lookup, listing, validation, and termination.
  - Handles invalid, duplicate, or non-existent PIDs gracefully without crashing.

## 2. Virtual File System (`VirtualFileSystem.h`, `VirtualFileSystem.cpp`)
- **Responsibilities**:
  - In-memory hierarchical namespace composed of `VFSNode`, `VFSFile`, and `VFSDirectory`.
  - Supports file creation, directory creation, recursive path normalization, file reading, file writing, resource deletion, and directory listing.
  - Maintains node metadata (`size`, `createdAt`, `modifiedAt`).
  - Completely isolated in program memory to prevent accidental access or modification of real host files.
  - Deleting a node cleanly cascades to its children and unregisters associated capabilities.

## 3. Capability Manager (`CapabilityManager.h`, `CapabilityManager.cpp`)
- **Responsibilities**:
  - Maintains the capability lookup table mapping each PID to its assigned capability tokens.
  - Implements dynamic capability granting and validation against duplicate entries.
  - Implements dynamic capability revocation with immediate effect.
  - Provides query APIs: `hasCapability(pid, path, op)`, `getCapabilitiesForProcess(pid)`, `getAllCapabilities()`.
  - Cleans up stale capabilities when resources or processes are deleted.

## 4. Access Controller (`AccessController.h`, `AccessController.cpp`)
- **Responsibilities**:
  - Central Reference Monitor mediating all process requests to the virtual file system.
  - Exposes system-call-like interfaces:
    - `requestRead(pid, path)`
    - `requestWrite(pid, path, content)`
    - `requestDelete(pid, path)`
    - `requestExecute(pid, path)`
  - Verifies subject existence, object existence, and capability ownership before dispatching to VFS.
  - Dispatches execution to Audit Logger for recording every authorization decision.
  - Safe simulated execution sandbox for virtual binaries.

## 5. Audit Logger (`AuditLogger.h`, `AuditLogger.cpp`)
- **Responsibilities**:
  - Records an append-only audit trail of every access decision (`ALLOWED` or `DENIED`).
  - Stores: `id`, `timestamp`, `pid`, `processName`, `operation`, `resourcePath`, `result`, `details`.
  - Provides formatted tabular display for CLI inspection.
  - Computes audit statistics (total events, allowed attempts, denied attempts).
