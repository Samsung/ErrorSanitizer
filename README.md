# ErrorSanitizer
ErrorSanitizer (ESAN in short) is a library that can be used to detect incorrect handling of error conditions (e.g. unhandled memory allocation errors).
ESAN injects failures to selected standard library function calls.
For example it can make malloc() fail and return NULL or ftello return -1 as return value.
Failures injections are based on a provided map file, so it can be integrated with fuzzers like afl (American Fuzzy Lop) or custom solutions.
It can be used to test how application behaves in unexpected conditions.
It is also useful in unit testing for checking rare corner cases without writing additional stubs.

## Execution workflow:
1. ErrorSanitizer library internal initialization
2. Read injections map from:

    a) file provided as argument

    b) standard input
3. Continue execution to tested binary main() function.
4. Inject failures to standard library calls based on provided map.

## Compilation
Compile requirements:
  - C compiler which is compatible with C GNU89 dialect such as GNU gcc or clang
  - GNU make
  - libssl-dev (optional for openssl hooks)
  - diffutils - (optional it is required to execute tests)

At the moment project supports only GNU/Linux operating system.
Feel free to open issue if you want to test it on different platform.

To compile project simply execute:

```shell
make
```
To compile tests:
```shell
make test
```
And then execute them:
```shell
make run
```
### Build flags
Additional build flags can passed by environment variables:<br />
  a) CFLAGS_LOCAL - sets additional CFLAGS<br />
  b) LDFLAGS_LOCAL - sets additional LDFLAGS

Build time defines:

a) AFL - disable logging:
  - on failure exit
  - on failure injection
  - statistics printing during exit
  - statistics counting

b) CP_WRONG_MAP - enable copying maps with wrong format to `/tmp/esan_debug%d.txt`
where `%d` is random integer, useful only for debugging.

c) DEBUG - enable logging

d) ESAN_DISABLE_HOOKS_OPENSSL - disables openssl hooks, set by default.

e) ESAN_NO_SIGNALS - disable signal handlers use by ESAN.

```shell
CFLAGS_LOCAL="-DDEBUG -DCP_WRONG_MAP" make
```
## Sample usage

In this sample we want to execute `cat` to read contents of `/etc/passwd` file.
Normally we would just type:
   ```shell
   cat "/etc/passwd"
   ```
Now let's do the same using ErrorSanitizer.

1. Clone project
   ```shell
   git clone https://github.com/Samsung/ErrorSanitizer.git
   ```
2. Compile project

   Please follow [Compilation](#compilation) section.

3. Execute sample binary:

   To execute binary under ErrorSanitizer, we will need to use LD_PRELOAD functionality.
   It is used to inject the shared library before executing the target binary.
   Furthermore ErrorSanitizer needs to read failure map so it knows when to inject failures.

   In this example we will use map that always succeed just to demonstrate how to run sample binary.

   There are two posible execution variants:

    a) by passing path to map as one argument.
	Argument order does not matter, it just has to have ".cur_input" extension.
    Copy premade map so it will have correct .cur_input extension.
    ```
      cp tests/esan_always_succeed.map succeed.cur_input
    ```
    File succeed.cur_input should have only _FAILURE_MAP_ and XXXX field separator
	as we don't want to pass any additional data to cat application.
    In this example we don't have to worry about it beacause it is already true.
    Now just execute:
    ```shell
      LD_PRELOAD="$(pwd)/error_sanitizer_preload.so" cat /etc/passwd succeed.cur_input
    ```
    Before execution reaches cat's main() function Errorsanitizer will create new file `succeed.esn_input`
    and change the second application argument to `succeed.esn_input`.
    It will be empty since `succeed.cur_input` had only `_FAILURE_MAP_` and `XXXX` field separator in it.
    Therefore the result will be the same as just executing `cat /etc/passwd`

    Note: If you do not like this behavior use "ESAN_WRITE_PROGRAM_DATA_TO_MAP_FILE" environment
	variable. Errorsanitizer will then override file contents and write only
	`_OPTIONAL_SAMPLE_BINARY_DATA_` to it.

    b) by passing data as standard input. It requires environment variable `AFL_USE_STDIO` to be set.

    ```shell
      AFL_USE_STDIO="Y" LD_PRELOAD="$(pwd)/error_sanitizer_preload.so" \
      cat /etc/passwd < tests/esan_always_succeed.map
    ```

## Tutorials

If you want to read how to use ESAN on simple real-life applications - see [Tutorial 1 - ezxml](./doc/tutorial_000/desc.md).

## Map file Structure

`_FAILURE_MAP_XXXX_OPTIONAL_SAMPLE_BINARY_DATA_`

where:

`_FAILURE_MAP_` - failure map in binary format, where 0 means succeed and 1 means fail.

`XXXX` - special field separator

`_OPTIONAL_SAMPLE_BINARY_DATA_` - data for tested application. For example filepath for cat binary "/etc/passwd"

ErrorSanitizer in it's constructor reads this data from the input file / STDIN
and passes only `_OPTIONAL_SAMPLE_BINARY_DATA_` to the application.
When the map is provided as file, ErrorSanitizer creates a new file with only `_OPTIONAL_SAMPLE_BINARY_DATA_`.
The newly created file resides in the same path but the file extension is changed from ".cur_input" to ".esn_input".
This is done transparently for the tested application.

## Hooks
The list of currently implemented hooks can be found in the [Hooks](./doc/Hooks.md) section.

## AFL Integration
1. Execute step 1. from sample usage.
2. Compile project
    ```shell
    CFLAGS_LOCAL="-DAFL" make
    ```
2. Add necessary environment variables:
   ```shell
   AFL_PRELOAD=PATH_TO_ESAN/error_sanitizer_preload.so
   AFL_NO_FORKSRV=1
   ```
   Make sure to use the absolute file path.

3. Compile tested binary using afl wrappers (e.g. afl-gcc, afl-clang-fast)
4. Execute afl-fuzz:
   ```shell
   afl-fuzz -i PATH_TO_INPUT_DIR -o PATH_TO_OUTPUT_DIR -m none TARGET_BINARY @@
   ```
arguments:


*  `-i` - input directory with test cases
*  `-o` - output directory for fuzzer findings
*  `-m none` - do not use memory limit for child process, default value: 50MB
*  `@@` - afl-fuzz will replace it with path to input data.

   This file will have .cur_input extension.
ErrorSanitizer will replace this file with ".esn_input". It is required due to the [map file structure](#map-file-structure).

## NOTES
This project is in alpha state.

## Contribution
Before contribution please create a Github issue.
The [CI](./ci) directory contains useful scripts e.g. for [code-formatting](./ci/run-clang-format.sh).
