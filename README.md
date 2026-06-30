# minigit

A minimal C clone of Git

# Current state

## Project structure

- `src/cli/` contains the process entrypoint and command dispatch table
- `src/commands/` contains thin command wrappers:
  - `plumbing/` — `hash-content`, `cat-file`, `rev-parse`
  - `porcelain/` — `init`
- `src/core/` contains Git-domain logic:
  - `repo` — repository initialization and opening
  - `oid` — object ID validation and formatting
  - `object` — parsed object representation and cleanup
  - `object_store` — object hashing, lookup, header reads, and content reads
  - `revision` — revision-name resolution
- `src/platform/` contains low-level wrappers:
  - `fs` — filesystem checks
  - `hash` — SHA1 hashing through OpenSSL
  - `compress` — zlib inflation
- `src/utils/` contains generic helpers:
  - `errors` — structured error codes/messages
  - `read` — stream-to-buffer reads
- `include/minigit/` mirrors the public headers for those layers
- `tests/` contains a small happy-path test scaffold for the current behavior

---

## Command shape

- Commands keep typed argument structs such as `HashContentArgs`, `CatFileArgs`, and `RevParseArgs`
- Argument structs only represent parsed command input
- Runtime state such as object files, headers, hashes, and decompressed data lives in `src/core/`
- The CLI dispatch table calls command entrypoints such as `hashContentCommand`, `catFileCommand`, and `revParseCommand`
- Adding a command should generally mean:
  - add a command source file under `src/commands/`
  - add a command header under `include/minigit/commands/`
  - register the command in `src/cli/dispatch.c`
  - add a basic test under `tests/`

---

## `minigit init`

- Creates `.minigit/` with subdirs `hooks/`, `info/`, `objects/`, `refs`, `objects/info`, `objects/pack`, `refs/heads`, and `refs/tags`
- Creates `config`, `description`, `HEAD`, and `info/exclude`
- Writes `ref: refs/heads/main` to `HEAD`
- Fails with a message if a repo already exists in the current directory
- ❌ `config`, `description`, and `info/exclude` are still created empty

---

## `minigit hash-content (--path=<file> | --stdin) [-w]`

- Reads content from a file or stdin
- Hashes content as a `blob <size>\0<content>` payload through `core/object_store`
- `--path=` and `--stdin` are mutually exclusive; exactly one is required
- Prints the 40-char hex hash when reading from stdin
- ❌ `--path=` currently computes the hash but does not print it
- ❌ `-w` is parsed but **not implemented** — nothing is written to `.minigit/objects/`
- ❌ Only `blob` type is supported — no `tree` or `commit` objects

---

## `minigit cat-file (-t | -s | -e | -p) <hash>`

- Resolves the hash through `core/revision`
- Reads objects through `core/object_store`
- Supports:
  - `-t` — prints the object type
  - `-s` — prints the object size in bytes
  - `-e` — prints whether the object exists
  - `-p` — decompresses the object content and prints it
- ❌ Exactly one flag + one full 40-char hash is the intended shape
- ❌ `-e` prints a string instead of exiting with a status code
- ❌ Short hashes and symbolic refs such as `HEAD` are not accepted

---

## `minigit rev-parse <hash>`

- Validates the input as a full 40-char lowercase hex object ID
- Checks that the object exists in `.minigit/objects/<xx>/<38-char-remainder>`
- Prints the resolved hash
- ❌ Symbolic names such as `HEAD`, branch names, and tags are not supported
- ❌ Only resolves a single hash per invocation

---

## `minigit --help` / `-h`

- Prints available commands from the CLI dispatch table
- Includes short command descriptions

---

# Object storage

- Objects are stored under `.minigit/objects/<xx>/<38-char-remainder>`
- Object IDs are SHA1 hashes of `blob <size>\0<content>`
- The current test fixture/object reader expects an uncompressed object header followed by zlib-compressed content
- `cat-file -p` inflates the compressed content through `src/platform/compress.c`
- ❌ This is not yet Git-compatible loose-object storage, because Git zlib-compresses the full `header\0content` payload

---

# Tests

- `make tests` builds and runs the test binary
- The current suite intentionally contains one happy-path test per area:
  - `init`
  - `hash-content`
  - `rev-parse`
  - `cat-file`
  - `errors`
  - `read`
- The test scaffold is intentionally small so more complete behavior tests can be added incrementally
