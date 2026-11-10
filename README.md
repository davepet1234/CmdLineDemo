# CmdLineDemo

This is a demo of an EDK2 command line library for EFI-Shell applications.

For details on the library itself please refer to the [CmdLineLib](https://github.com/davepet1234/CmdLineLib) project.

## Build

Place this directory at `ShellPkg/Application/CmdLineDemo`, add `ShellPkg/Application/CmdLineDemo/CmdLineDemo.inf` to the `[Components]` section of `ShellPkg/ShellPkg.dsc`, then:

    source edksetup.sh
    build -p ShellPkg/ShellPkg.dsc -m ShellPkg/Application/CmdLineDemo/CmdLineDemo.inf -a X64 -t GCC

## Usage

    CmdLineDemo.efi str [num1] [num2] [str8] -d num [options]

| Parameter | Description                    |
| --------- | ------------------------------ |
| `str`     | String (mandatory)             |
| `num1`    | Hexadecimal number (8-bit)     |
| `num2`    | Decimal number                 |
| `str8`    | ASCII (8-bit) string           |

| Switch                | Description                                  |
| --------------------- | -------------------------------------------- |
| `-d, -dec num`        | Decimal value 0-255 (mandatory)              |
| `-f`                  | Boolean flag                                 |
| `-flag2`              | Flag that sets a predefined value            |
| `-c, -colour val`     | Enum: `black\|red\|green\|blue\|white`       |
| `-x, -hex num`        | Hexadecimal value                            |
| `-i num`              | Integer value, decimal or `0x` hex (16-bit)  |
| `-s, -string str`     | String value                                 |
| `-a, -ascii str8`     | ASCII (8-bit) string value                   |
| `-l, -lines num`      | Print `num` lines; press ESC to abort        |
| `-input type`         | Test user input: `str\|dec\|hex\|int`        |
| `-key keylist`        | Test key press; empty list (`""`) = any key  |
| `-b, -break`          | Enable page break                            |
| `-h, -help`           | Display help and exit                        |

The application prints the value of every parameter and switch (`*` marks those entered on the command line), followed by the parser's `ShellStatus`.

## Examples

    CmdLineDemo.efi -h
    CmdLineDemo.efi hello 1f 42 text -d 200 -c red -x 0xAB -i 0x10 -f
    CmdLineDemo.efi hello -d 1 -l 100 -b
    CmdLineDemo.efi hello -d 1 -input int
    CmdLineDemo.efi hello -d 1 -key yn
