open Lwt;

let () = {
  let config = Lib.Config.make ();
  switch config {
  | Some config =>
    let project = Lib.Config.getCwdProject config;
    switch project {
    | Some project =>
      Lib.Console.out ("Found project:" ^ project.path);
      let git = Lib.Git.make ();
      switch git {
      | Some git =>
        Lib.Console.out "Found git dir";
        Lib.Console.out (Lib.Git.toString git);
        /* let response = Lwt_main.run (Lib.Api.createReview config project); */
        let apiResponse = Lib.Review.create config project git;
        let tn =
          apiResponse >|= (
            fun a =>
              switch a {
              | Error e => print_endline e
              | Success r => print_endline ("review created" ^ r.permaId)
              }
          );
        Lwt_main.run tn;
        ()
      | None => Lib.Console.out "Not a git directory - exiting."
      }
    | None =>
      Lib.Console.out (
        "No project found, please make sure you add the current path (" ^
        Lib.Project.getCwd () ^ ") to your config"
      )
    };
    ()
  | None =>
    Lib.Console.out (
      "No config found. Please provide a config file under " ^ Lib.Config.configPath
    )
  }
};
