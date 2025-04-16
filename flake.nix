{
  description = "Calc flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    allSystems = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
    ];
    forAllSystems = f:
      nixpkgs.lib.genAttrs allSystems (system:
        f {
          pkgs = import nixpkgs {inherit system;};
        });
  in {
    packages = forAllSystems ({pkgs}: {
      default = let
        binName = "calc";
        cDependencies = with pkgs; [readline.dev];
      in
        pkgs.stdenv.mkDerivation {
          name = "calc";
          src = self;
          buildInputs = cDependencies;
          buildPhase = ''
            gcc -o nob nob.c
            ./nob
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp ./build/${binName} $out/bin/
          '';
        };
    });
  };
}
