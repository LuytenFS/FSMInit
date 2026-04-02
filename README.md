# FSMInit

FSMInit is an automation tool designed to simplify the creation of directories and tables needed for FreeSpace mods. This process is usually tedious, but FSMInit makes it quick and easy by accepting command-line arguments.

---

### Parameters

1. **`<command>`**  
   The operation to perform. Allowed values:  
   - `-stdm`    : Standard Mod (no tables)  
   - `-stdmc`   : Standard Mod Complex (with tables)  
   - `-help`    : Display usage information  

2. **`<path>`**  
   The directory path where the tool will create directories and tables.

3. **`<-tbl/-tbm>`**  
   Specifies the type of files to create:  
   - `-tbl` : Create standard `.tbl` files  
   - `-tbm` : Create modular `.tbm` files  

4. **`[prefix]`**  
   Optional prefix for `.tbm` files. This is **required only** if `-tbm` is specified. Ignored when `-tbl` is used.

5. **`[-debug]`**  
   Optional flag. Enables debug output, which will be written to a `log.txt` file in the program's current directory.

---

## Example Usage

**`FSMInit -stdmc "path" -tbm MOD_ -debug`**

