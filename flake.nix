{
  description = "A nix flake to manage dependencies for RP2040 projects";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: 
    flake-utils.lib.eachDefaultSystem (system:
      let 
        pkgs = import nixpkgs { inherit system; };

        picoSdk = pkgs.fetchgit{
          url = "https://github.com/raspberrypi/pico-sdk.git";
          rev = "2.1.1";
          sha256 = "1a5s5pvqv8kzvj1f0ryydbrcm7akyc5l96hgr6v59mzcd2wbbfzj";
          # Rembember to update the hash if another version is desired: 
          # nix run nixpkgs#nix-prefetch-git -- --url https://github.com/raspberrypi/pico-sdk.git --rev 2.1.1 --fetch-submodules
          fetchSubmodules = true;
        };

        freeRTOS = pkgs.fetchgit {
          url = "https://github.com/FreeRTOS/FreeRTOS-Kernel.git";
          rev = "V11.2.0";
          sha256="1nblmq54jl1hjz5brvxwisg20pp7d8k816ssqpm7d44250v7kiwn";
          # Rembember to update the hash if another version is desired: 
          # nix run nixpkgs#nix-prefetch-git -- --url https://github.com/FreeRTOS/FreeRTOS-Kernel.git --rev V11.2.0 --fetch-submodules
          fetchSubmodules = true;
        };

        picoHD44780 = pkgs.fetchFromGitHub{
          owner = "Eifoen";
          repo = "pico-hd44780";
          rev = "main";
          sha256 = "0jzww1p08cycdawivkwwf33469073m3lspb21z4z1nqps4mzmi4p";
        };

      in{
        devShells.default = pkgs.mkShell {
          buildInputs = [
            pkgs.cmake
            pkgs.gcc
          ];
        env = {
          PICO_SDK_PATH = picoSdk;
          FREERTOS_KERNEL_PATH = freeRTOS;
        };
        };
      });

  }
