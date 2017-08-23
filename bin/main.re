open Lwt;

let rec addChangeset config project git review =>
  Lib.Changeset.add config project git review >>= (
    fun response =>
      switch response {
      | Error e =>
        print_endline e;
        print_endline "Retrying in 3 seconds...";
        Lwt_unix.sleep 3. >>= (
          fun () => addChangeset config project git review
        )
      | Success =>
        print_endline "changeset added";
        Lwt.return_unit
      }
  );

let createReviewAndAddChangeset config project git =>
  Lib.Review.create config project git >>= (
    fun a =>
      switch a {
      | Error e =>
        print_endline e;
        Lwt.return_unit
      | Success review =>
        print_endline ("Review created: " ^ review.permaId);
        addChangeset config project git review
      }
  );

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
        let thread = createReviewAndAddChangeset config project git;
        Lwt_main.run thread;
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
