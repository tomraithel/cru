open Yojson.Basic.Util;

type t = {
  url: string,
  userName: string,
  password: string
};

let make json => {
  userName: json |> member "userName" |> to_string,
  password: json |> member "password" |> to_string,
  url: json |> member "url" |> to_string
};
