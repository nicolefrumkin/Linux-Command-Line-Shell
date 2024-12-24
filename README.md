# Shell

Shell is a simple Linux shell written in C. It supports basic commands, including:

- **Internal commands**: `exit`, `cd`, `jobs`.
- **External commands**: Runs standard Linux commands using `fork` and `exec`.
- **Background processes**: Supports up to 4 running at once.

## How to Use
1. Compile the program using the provided `Makefile`:
   ```bash
   make
   ```
2. Run the shell:
   ```bash
   ./hw1shell
   ```
3. Use commands as you would in a regular shell.

## Clean Up
To remove compiled files, run:
```bash
make clean
``` 
