# Viva & Evaluation Demonstration Walkthrough

This guide provides a step-by-step presentation script for the faculty review / project viva.

## Quick 1-Command Automated Demonstration

You can run the full automated demonstration during the viva by running:
```bash
# Using Makefile
make demo

# Or using build.bat
build.bat demo

# Or directly executing binary
./bin/secure_fs.exe --demo
```

---

## Interactive Manual Step-by-Step Demonstration

If the faculty asks you to demonstrate each operation manually through the interactive CLI, follow this exact sequence:

### Launch Interactive CLI
```bash
./bin/secure_fs.exe
```

### Step 1: Create Simulated Processes (Option 1)
1. Select `1` (Create Process). Enter name: `ReaderProcess` $\rightarrow$ Assigned PID `101`.
2. Select `1` (Create Process). Enter name: `WriterProcess` $\rightarrow$ Assigned PID `102`.
3. Select `1` (Create Process). Enter name: `RestrictedProcess` $\rightarrow$ Assigned PID `103`.
4. Select `2` (List Processes) to display all three active processes.

### Step 2: Create Virtual File System Resources (Options 3 & 4)
1. Select `4` (Create Directory). Path: `/documents`
2. Select `3` (Create File). Path: `/documents/report.txt`, Content: `Confidential OS Project Findings`
3. Select `3` (Create File). Path: `/bin/service.bin`, Content: `BinaryExecutable`
4. Select `5` (List Resources) to display the created virtual file system hierarchy.

### Step 3: Grant Capabilities (Option 6)
Demonstrate the **Principle of Least Privilege**:
1. Grant `READ` on `/documents/report.txt` to PID `101`:
   - Enter `6`, PID `101`, Path `/documents/report.txt`, Operation `READ`.
2. Grant `WRITE` on `/documents/report.txt` to PID `102`:
   - Enter `6`, PID `102`, Path `/documents/report.txt`, Operation `WRITE`.
3. Select `8` (List Capabilities) to display the capability table. Notice PID `103` has no privileges.

### Step 4: Demonstrate Authorized Access (Option 9)
1. Select `9` (Read File). Requesting PID: `101`, Path: `/documents/report.txt`.
   - **Result**: `[ALLOWED]` — Content `Confidential OS Project Findings` is displayed.

### Step 5: Demonstrate Unauthorized Access (Option 9 & 10)
1. Select `10` (Write File). Requesting PID: `101`, Path: `/documents/report.txt`, Content: `Unauthorized overwrite attempt`.
   - **Result**: `[DENIED]` — Process 101 only holds READ capability; WRITE is blocked!
2. Select `9` (Read File). Requesting PID: `103`, Path: `/documents/report.txt`.
   - **Result**: `[DENIED]` — Process 103 holds no capabilities; access is blocked!

### Step 6: Dynamic Capability Grant & Verification (Options 6 & 10)
1. Select `6` (Grant Capability). PID `101`, Path `/documents/report.txt`, Operation `WRITE`.
2. Select `10` (Write File). PID `101`, Path `/documents/report.txt`, Content `Authorized update by Process 101`.
   - **Result**: `[ALLOWED]` — Write is permitted now that capability is held!

### Step 7: Dynamic Capability Revocation & Immediate Denial (Options 7 & 10)
1. Select `7` (Revoke Capability). PID `101`, Path `/documents/report.txt`, Operation `WRITE`.
   - **Result**: Capability revoked!
2. Select `10` (Write File). PID `101`, Path `/documents/report.txt`, Content `Write after revocation`.
   - **Result**: `[DENIED]` — Immediately rejected! Proves dynamic revocation without caching delays.

### Step 8: Safe Simulated EXECUTE (Option 12)
1. Grant `EXECUTE` on `/bin/service.bin` to PID `101` (Option `6`).
2. Select `12` (Execute Resource). PID `101`, Path `/bin/service.bin`.
   - **Result**: `[EXECUTE]` — Virtual resource executed in simulated sandbox!
3. Select `12` (Execute Resource). PID `102`, Path `/bin/service.bin`.
   - **Result**: `[DENIED]` — Unauthorized execution prevented!

### Step 9: Review Audit Trail (Option 13)
1. Select `13` (View Audit Log).
   - Shows complete tamper-evident audit history of all allowed and denied attempts with exact timestamps.
