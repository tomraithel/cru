open Core;

/*
 MORE COLORS:
 -------------
 Reset = "\x1b[0m"
 Bright = "\x1b[1m"
 Dim = "\x1b[2m"
 Underscore = "\x1b[4m"
 Blink = "\x1b[5m"
 Reverse = "\x1b[7m"
 Hidden = "\x1b[8m"

 FgBlack = "\x1b[30m"
 FgRed = "\x1b[31m"
 FgGreen = "\x1b[32m"
 FgYellow = "\x1b[33m"
 FgBlue = "\x1b[34m"
 FgMagenta = "\x1b[35m"
 FgCyan = "\x1b[36m"
 FgWhite = "\x1b[37m"

 BgBlack = "\x1b[40m"
 BgRed = "\x1b[41m"
 BgGreen = "\x1b[42m"
 BgYellow = "\x1b[43m"
 BgBlue = "\x1b[44m"
 BgMagenta = "\x1b[45m"
 BgCyan = "\x1b[46m"
 BgWhite = "\x1b[47m"
 */
type color =
  | Reset
  | Underline
  | Bright
  | Red
  | Green;

let colorCode = (color: color) =>
  switch color {
  | Reset => "0"
  | Underline => "4"
  | Bright => "1"
  | Red => "31"
  | Green => "32"
  };

let colorize = (str, color) => "\027[" ++ colorCode(color) ++ "m" ++ str ++ "\027[0m";

let out = (~color: option(color)=?, str: string) =>
  switch color {
  | None => print_endline(str)
  | Some(color) => print_endline(colorize(str, color))
  };

let outOneLine = (~color: option(color)=?, str: string) => {
  switch color {
  | None => print_string(str)
  | Some(color) => print_string(colorize(str, color))
  };
  Core.Out_channel.flush(Core.Out_channel.stdout)
};