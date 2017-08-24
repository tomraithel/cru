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

let createReview config project git =>
  Lib.Review.create config project git >>= (
    fun a =>
      switch a {
      | CreateError e =>
        print_endline e;
        Lwt.return None
      | CreateSuccess review =>
        print_endline ("Review created: " ^ review.permaId);
        Lwt.return (Some review)
      }
  );

let abandonReview config review =>
  Lib.Review.abandon config review >>= (
    fun a =>
      switch a {
      | AbandonError e =>
        print_endline e;
        Lwt.return_unit
      | AbandonSuccess =>
        print_endline "Review has been abandoned";
        Lwt.return_unit
      }
  );

let () =
  try {
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
          let thread = createReview config project git;
          let review = Lwt_main.run thread;
          switch review {
          | None => ()
          | Some review =>
            /* Enable catching CTRL+C */
            Sys.catch_break true;
            try {
              let thread = addChangeset config project git review;
              Lwt_main.run thread
            } {
            | Sys.Break =>
              let thread = abandonReview config review;
              Lwt_main.run thread
            }
          }
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
  } {
  | Yojson.Json_error err => print_endline ("JSON error: " ^ err)
  | Yojson.Basic.Util.Type_error err _ =>
    print_endline ("JSON type error: " ^ err)
  | Failure err => print_endline ("General error: " ^ err)
  };
