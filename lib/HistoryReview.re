type t = string;

let make = (json) => Yojson.Basic.Util.to_string(json);
