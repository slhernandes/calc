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

Example:
```
calc> a = (150//(10+2^2)+7) * (6 % 4)
[RESULT]: 34
calc> b = (1337-420)/26.2
[RESULT]: 35.000000
calc> a + b
[RESULT]: 69.000000
calc> exit
```
