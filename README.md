# FSMInit, short for (FreeSpace Mod‑Initialization)
FSMInit is an automation tool designed to simplify the creation of directories and tables needed for FreeSpace mods. This process is usually tedious, but FSMInit makes it quick and easy by accepting command-line arguments.

---

### Parameters

1. **`<command>`**  
   The operation to perform. Allowed values:  
   - `-stdm`    : Standard Mod (no tables)  
   - `-stdmc`   : Standard Mod Complex (with tables)  
   - `-help`    : Display usage information
   - `-version` : Displays the current version of the program

2. **`<path>`**  
   The directory path where the tool will create directories and tables.  
   Maximum length: `PATH_MAX` characters.

3. **`<-tbl/-tbm>`**  
   Specifies the type of files to create (only for `-stdmc`):  
   - `-tbl` : Create standard `.tbl` files  
   - `-tbm` : Create modular `.tbm` files  

4. **`[prefix]`**  
   Prefix for `.tbm` files. **Required** if `-tbm` is specified. Ignored when `-tbl` is used.  
   Maximum length: 32 characters.

5. **`[-debug]`**  
   Optional flag. Enables debug output, written to `log.txt` in the program's current directory.

6. **`[-dry-run]`**  
   Optional flag. Previews what would be created without actually creating anything. Output is printed to the console.

---

### Validation

- The target path must exist and be writable before any operation is performed.
- If a mod structure already exists at the target path, the tool will refuse to overwrite it.
- Path and prefix inputs are checked for length before any operation is performed.

---

## Example Usage

**Standard mod with `.tbl` files:**  
`FSMInit -stdmc "path" -tbl`

**Standard mod with modular `.tbm` files:**  
`FSMInit -stdmc "path" -tbm MOD_`

**Preview without creating anything:**  
`FSMInit -stdmc "path" -tbl -dry-run`

**With debug logging:**  
`FSMInit -stdmc "path" -tbm MOD_ -debug`

**Combined dry-run and debug:**  
`FSMInit -stdmc "path" -tbm MOD_ -dry-run -debug`
