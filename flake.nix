{
  description = "building dinput8 proxy thing";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      sources = "dinput8.c keyboard.c proxy_idi.c proxy_idid.c";
      cflags  = "-std=gnu99 -O2 -Wall -Wextra -Wno-unused-parameter "
              + "-DDINPUT8_EXPORTS -DDIRECTINPUT_VERSION=0x0800";
      ldflags = "-static-libgcc -Wl,--kill-at,--enable-stdcall-fixup";
      build   = "i686-w64-mingw32-gcc ${cflags} -shared -o dinput8.dll "
              + "${sources} dinput8.def -ldxguid ${ldflags}";
    in {
      packages.${system}.default = pkgs.pkgsCross.mingw32.stdenv.mkDerivation {
        pname = "th-carpal-tunnel-fix";
        version = "0.1";
        src = ./src;

        dontConfigure = true;
        dontFixup = true;
        hardeningDisable = [ "all" ];

        buildPhase = ''
          runHook preBuild
          $CC ${cflags} -shared -o dinput8.dll ${sources} dinput8.def -ldxguid ${ldflags}
          runHook postBuild
        '';

        installPhase = ''
          runHook preInstall
          install -Dm755 dinput8.dll $out/dinput8.dll
          runHook postInstall
        '';
      };

      devShells.${system}.default = pkgs.mkShell {
        packages = [
          pkgs.pkgsCross.mingw32.stdenv.cc
          pkgs.gnumake
        ];
      };
    };
}
