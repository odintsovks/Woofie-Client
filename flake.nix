{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
  };
  outputs = { self, nixpkgs, ... }@inputs:
  let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    qtEnv = with pkgs.qt6; env "qt-custom-${qtbase.version}" (
      [
      ]
    );
  in
  {
    devShell.${system} = pkgs.mkShell {
      buildInputs = with pkgs; [
        qtEnv
        qtcreator
        libGL
        gdb
        cmake
      ];
    };
  };
}
