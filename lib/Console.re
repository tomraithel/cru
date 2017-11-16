open Core;

let out = (~color: option(int)=?, str: string) =>
  switch color {
  | None => print_endline(str)
  | Some(color) => print_endline("\027[" ++ string_of_int(color) ++ "m" ++ str ++ "\027[0m")
  };