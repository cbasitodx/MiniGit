# minigit

A minimal C clone of Git

# Current state

## `minigit init`

- Creates `.minigit/` with subdirs `hooks/`, `info/`, `objects/`, `refs/` and files `config`, `description`, `HEAD`
- Fails with a message if a repo already exists in the current directory
- ❌ `config`, `description`, and `HEAD` are created empty — no default content is written

---

## `minigit hash-content (--path=<file> | --stdin) [-w]`

- Reads content from a file or stdin, prepends a `blob <size>\0` header, computes the SHA1 and prints the 40-char hex hash
- `--path=` and `--stdin` are mutually exclusive; exactly one is required
- ❌ `-w` is parsed but **not implemented** — nothing is written to `.minigit/objects/`
- ❌ Only `blob` type — no `tree` or `commit` objects

---

## `minigit cat-file (-t | -s | -e | -p) <hash>`

- Resolves the hash via `rev-parse` then operates on the object:
  - `-t` — prints the object type (e.g. `blob`)
  - `-s` — prints the object size in bytes
  - `-e` — prints whether the object exists
  - `-p` — decompresses the object with zlib and prints its raw content
- ❌ Exactly one flag + one full 40-char hash required — no other combinations accepted
- ❌ `-e` prints a string instead of exiting with a status code
- ❌ Short hashes and symbolic refs (e.g. `HEAD`) are not accepted

---

## `minigit rev-parse <hash>`

- Validates the input as a full 40-char lowercase hex SHA1
- Looks up `.minigit/objects/<xx>/<38-char-remainder>`, reads the header up to `\0`, and prints the resolved hash
- ❌ Symbolic names (`HEAD`, branch names, tags) are not supported
- ❌ Only resolves a single hash per invocation

---

## `minigit --help` / `-h`

- Prints a list of available commands
- ❌ Command descriptions are placeholder stubs

---

# Object storage

Objects are stored under `.minigit/objects/<xx>/<38-char-remainder>`, mirroring Git's two-character directory sharding. Each object is prefixed with a `blob <size>\0` header. `cat-file -p` reads objects back using zlib decompression.
