# System Architecture

## Overview

The **Secure Capability-Based File System** is a modular C++ operating system simulator demonstrating fine-grained resource protection via capability-based access control. Rather than relying on traditional coarse-grained user/group permissions (such as standard UNIX rwx bits), the system enforces access control at the granular process level through unforgeable capability tokens.

```
+-------------------------------------------------------------------------+
|                        Process Request (PID, Path, Op)                  |
+------------------------------------+------------------------------------+
                                     |
                                     v
                  +-------------------------------------+
                  |          Access Controller          |
                  |     (Central Reference Monitor)     |
                  +------------------+------------------+
                                     |
                                     | Checks Token
                                     v
                  +-------------------------------------+
                  |         Capability Manager          |
                  |  PID + Resource + Allowed Operation |
                  +------------------+------------------+
                                     |
                         +-----------+-----------+
                         |                       |
                     [Found]                [Not Found]
                         |                       |
                         v                       v
                   +-----------+           +-----------+
                   |   ALLOW   |           |   DENY    |
                   +-----+-----+           +-----+-----+
                         |                       |
                         | Executes              | Rejects
                         v                       v
              +----------------------+           |
              | Virtual File System  |           |
              | (Files, Directories) |           |
              +----------+-----------+           |
                         |                       |
                         +-----------+-----------+
                                     |
                                     v
                  +-------------------------------------+
                  |            Audit Logger             |
                  |  Timestamp, PID, Op, Path, Decision |
                  +-------------------------------------+
```

## Security Design Principles

1. **Principle of Least Privilege (PoLP)**:
   - Processes are initialized with zero privileges by default.
   - Each process only receives the explicit capabilities required for its specific operational role.

2. **Complete Mediation (Central Reference Monitor)**:
   - Every file system request (READ, WRITE, DELETE, EXECUTE) must be mediated by the `AccessController`.
   - The `VirtualFileSystem` never exposes unprotected public mutation methods directly to simulated processes.

3. **Dynamic Permission Management**:
   - Capabilities can be granted dynamically at runtime.
   - Capabilities can be revoked dynamically at runtime.
   - Revocation takes immediate effect on subsequent access requests without caching or stale state.

4. **Tamper-Evident Security Audit Log**:
   - Both authorized (`ALLOWED`) and unauthorized (`DENIED`) attempts are captured with timestamps and process metadata.
