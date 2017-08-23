open Yojson.Basic.Util;

type t = {
  path: string,
  key: string,
  repo: string,
  allowReviewersToJoin: bool
};

let getFormattedInfo project => project.repo;

let slash = '/';

let isSlash c => c === slash;

let unslashed str => Core.String.rstrip drop::isSlash str;

let slashed str => str ^ Core.Char.to_string slash;

let make json => {
  path: json |> member "path" |> to_string |> unslashed,
  key: json |> member "key" |> to_string,
  repo: json |> member "repo" |> to_string,
  allowReviewersToJoin: true,
};

let getCwd = Core.Sys.getcwd;

let matchesCwd project =>
  Core.String.is_prefix (slashed (getCwd ())) (slashed project.path);
