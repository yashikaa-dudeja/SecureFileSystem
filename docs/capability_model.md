# Capability Model Specification

## Capability Representation

In capability-based operating systems, access rights are held as tickets or tokens known as capabilities. Holding the capability confers authorization to execute the specified operation on the specified resource.

In this simulator, a capability is defined as:

$$\text{Capability} = (\text{Process ID}, \text{Resource Path}, \text{Allowed Operation})$$

### Supported Operations
- **READ**: Read file contents or view directory listings.
- **WRITE**: Overwrite or update file contents.
- **DELETE**: Remove a file or directory node from the virtual namespace.
- **EXECUTE**: Safe simulated execution of virtual binaries/scripts in an isolated sandbox environment.

## Comparison: Capabilities vs Access Control Lists (ACLs)

| Feature | Access Control Lists (ACLs) | Capability-Based Security (Our Model) |
|---|---|---|
| **Association** | Stored on the object (resource has a list of authorized users) | Stored by the subject/process (process holds capabilities for resources) |
| **Granularity** | User or Group level | Individual simulated Process level |
| **Confused Deputy Problem**| Prone to confused deputy attacks | Resilient: operations require presenting the specific capability |
| **Revocation** | Must modify ACL stored on the resource | Capability Manager can revoke capability directly from process table |
| **Principle of Least Privilege** | Difficult to enforce on temporary or per-process basis | Trivial: grant only the exact capability needed for task duration |

## Dynamic Lifecycle: Grant and Revocation

1. **Grant**:
   - Authorized administrative command issues `grant(PID, Resource, Operation)`.
   - Validates that target Process exists and Resource exists in VFS.
   - Prevents duplicate registration.

2. **Verification**:
   - Process requests operation on resource.
   - Reference monitor queries `hasCapability(PID, Resource, Operation)`.
   - If present: access is `ALLOWED`.
   - If absent: access is `DENIED`.

3. **Revocation**:
   - Administrator issues `revoke(PID, Resource, Operation)`.
   - The capability tuple is removed from the process's active table.
   - Any subsequent operation is rejected immediately.
