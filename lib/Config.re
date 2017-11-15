open Yojson.Basic.Util;

open Core;

type t = {
  crucible: Crucible.t,
  projects: list(Project.t)
};

let configPath = Core.Sys.getenv_exn("HOME") ++ "/.review.json";

let make = () =>
  if (Core.Sys.file_exists_exn(configPath)) {
    let config = Yojson.Basic.from_file(configPath);
    Some({
      crucible: config |> member("crucible") |> Crucible.make,
      projects: config |> member("projects") |> to_list |> List.map(~f=Project.make)
    })
  } else {
    None
  };

let getCwdProject = (config) => {
  let projects = List.filter(config.projects, ~f=Project.matchesCwd);
  switch projects {
  | [] => None
  | [hd, ...tl] => Some(hd)
  }
};
