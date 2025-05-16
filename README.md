# Calc

> [!WARNING]
> This is a finished test project to try out [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h), [nob.h](https://github.com/tsoding/nob.h), and [readline](https://www.gnu.org/software/readline/).
> Any bugs left in this code will probably not be fixed.

A simple CLI calculator app in C.

## Build

### NixOS
Example Nix flake:
```nix
{
    description = "Example flake.nix";
    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
        calc.url = "github:slhernandes/calc";
    };
    outputs = {self, nixpkgs, ...}@inputs: {
        nixosConfigurations = {
            example = nixpkgs.lib.nixosSystem {
            specialArgs = { inherit inputs; };
            modules = [
                ./hosts/example/configuration.nix
            ];
        };
    };
}
```
Then, in your ```configuration.nix```:
```nix
    environment.systemPackages = [
        inputs.calc.packages.${pkgs.system}.default
    ];
```



### Other linux and MacOS

```sh
git clone https://github.com/slhernandes/calc
cd calc
gcc -o nob nob.c
./nob && ./nob install # By default installs to ~/.local/bin
```

## Dependencies

- [readline](https://www.gnu.org/software/readline/)

## Usage

Valid operator:
- ```+```: Addition
- ```-```: Subtraction
- ```*```: multiplication
- ```/```: Division
- ```//```: Integer division
- ```^```: Exponentiation
- ```%```: Remainder
- ```=```: Assignment
- ```(``` and ```)```: Parenthesis

Valid commands:
- `exit`: exits the program
- `help`: show help page
- `0x`: prints previous result in hexadecimal (only if previous result is integer)
- `0b`: prints previous result in binary (only if previous result is integer)

Example:
```
calc> a = (150//(10+2^2) + 7) * (6 % 4)
[RESULT]: 34
calc> b = (1337 - 420)/26.2
[RESULT]: 35.000000
calc> (a+b)//1
[RESULT]: 69
calc> c = prev
[RESULT]: 69
calc> 0x
[RESULT]: 0x45
calc> 0b
[RESULT]: 0b1000101
calc> exit
```

Notice that the variable `prev` is a reserved variable used to store the previous result.
