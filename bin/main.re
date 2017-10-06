open Lwt;

let rec addChangeset config project git review =>
  Lib.Changeset.add config project git review >>= (
    fun response =>
      switch response {
      | Error e =>
        Lib.Console.out e;
        Lib.Console.out "\226\154\153  Retrying in 3 seconds...";
        Lwt_unix.sleep 3. >>= (
          fun () => addChangeset config project git review
        )
      | Success =>
        Lib.Console.out "\226\156\133  Changeset added.";
        Lwt.return review
      }
  );

let createReview config project git =>
  Lib.Review.create config project git >>= (
    fun a =>
      switch a {
      | CreateError e =>
        Lib.Console.out e;
        Lwt.return None
      | CreateSuccess review =>
        Lib.Console.out (
          "\226\156\133  Review created: " ^
          Lib.Review.getDetailPageUrl config review
        );
        Lwt.return (Some review)
      }
  );

let abandonAndDeleteReview config review =>
  Lib.Review.abandon config review >>= (
    fun a =>
      switch a {
      | AbandonError e =>
        Lib.Console.out e;
        Lwt.return_unit
      | AbandonSuccess =>
        Lib.Review.delete config review >>= (
          fun a =>
            switch a {
            | DeleteError e =>
              Lib.Console.out e;
              Lwt.return_unit
            | DeleteSuccess =>
              Lib.Console.out "\226\157\140  Review has been deleted";
              Lwt.return_unit
            }
        )
      }
  );

let showHistory () => {
  let h = Lib.History.make ();
  switch h.reviews {
  | [] => Lib.Console.out "no history :("
  | [entry, ...tl] => Lib.Console.out entry
  }
};

let createReview () =>
  try {
    let config = Lib.Config.make ();
    switch config {
    | Some config =>
      let project = Lib.Config.getCwdProject config;
      switch project {
      | Some project =>
        Lib.Console.out ("\027[32mFound project:" ^ project.path);
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
              let thread2 =
                thread >>= (
                  fun review => {
                    let _ =
                      Lib.History.addReview
                        (Lib.History.make ()) review.permaId;
                    Lwt.return_unit
                  }
                );
              Lwt_main.run thread2;
              ()
            } {
            | Sys.Break =>
              let thread = abandonAndDeleteReview config review;
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
  | Yojson.Json_error err => Lib.Console.out ("JSON error: " ^ err)
  | Yojson.Basic.Util.Type_error err _ =>
    Lib.Console.out ("JSON type error: " ^ err)
  | Failure err => Lib.Console.out ("General error: " ^ err)
  };

/* review (); */
/* Command stuff */
let create =
  Core.Command.basic
    summary::"[default] Creates a new review based on the current working directory"
    Core.Command.Spec.(empty)
    createReview;

let add =
  Core.Command.basic
    summary::"Adds the changeset that has been created in current directory to the last review in history"
    Core.Command.Spec.(empty)
    (fun () => Core.printf "TODO\n");

let history =
  Core.Command.basic
    summary::"Shows the history of the last created reviews (new ones on top)"
    Core.Command.Spec.(empty)
    showHistory;

let command =
  Core.Command.group
    summary::"Create and modify crucible reviews"
    body::(fun path::_ => createReview ()) /* Use createReview if no subcommand is passed*/
    [("create", create), ("add", add), ("history", history)];

let () = Core.Command.run command;
