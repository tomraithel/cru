open Lwt;

let rec addChangeset = (~iterator=0, config, project, git, review) =>
  Lib.Changeset.add(config, project, git, review)
  >>= (
    (response) =>
      switch response {
      | Error(e) =>
        /* Lib.Console.out(e); */
        Lib.Console.outOneLine(
          "\rFailed to add changeset. Retrying." ++ Lib.Util.stringRepeat(".", iterator)
        );
        Lwt_unix.sleep(2.)
        >>= (() => addChangeset(~iterator=iterator + 1, config, project, git, review))
      | Success =>
        Lib.Console.out(
          "\nChangeset added to review " ++ Lib.Review.getReadableName(config, review)
        );
        Lwt.return_unit
      }
  );

let createReview = (config, project, git) =>
  Lib.Review.create(config, project, git)
  >>= (
    (a) =>
      switch a {
      | CreateError(e) =>
        Lib.Console.out(e);
        Lwt.return(None)
      | CreateSuccess(review) =>
        Lib.Console.out(~color=Lib.Console.Green, "Review created: " ++ review.permaId);
        Lwt.return(Some(review))
      }
  );

let abandonAndDeleteReview = (config, review) =>
  Lib.Review.abandon(config, review)
  >>= (
    (a) =>
      switch a {
      | AbandonError(e) =>
        Lib.Console.out(e);
        Lwt.return_unit
      | AbandonSuccess =>
        Lib.Review.delete(config, review)
        >>= (
          (a) =>
            switch a {
            | DeleteError(e) =>
              Lib.Console.out(e);
              Lwt.return_unit
            | DeleteSuccess =>
              Lib.Console.out(~color=Lib.Console.Green, "\r\nReview has been deleted");
              Lwt.return_unit
            }
        )
      }
  );

let loadConfig = (fn) =>
  try {
    let config = Lib.Config.make();
    switch config {
    | Some(config) => fn(config)
    | None =>
      Lib.Console.out(
        ~color=Lib.Console.Red,
        "No config found. Please provide a config file at following path: "
        ++ Lib.Config.configPath
      )
    }
  } {
  | Yojson.Json_error(err) => Lib.Console.out(~color=Lib.Console.Red, "JSON error: " ++ err)
  | Yojson.Basic.Util.Type_error(err, _) =>
    Lib.Console.out(~color=Lib.Console.Red, "JSON type error: " ++ err)
  | Failure(err) => Lib.Console.out(~color=Lib.Console.Red, "General error: " ++ err)
  };

let showHistory = () =>
  loadConfig(
    (config) => {
      let h = Lib.History.make();
      switch h.reviews {
      | [] => Lib.Console.out("No history available - you have to create reviews first!")
      | l =>
        Lib.Console.out(~color=Lib.Console.Bright, "History of created reviews:");
        Lib.Console.out(
          Core.String.concat(
            ~sep="\n",
            Core.List.map(
              ~f=(permaId) => Lib.Review.getReadableName(config, {permaId: permaId}),
              l
            )
          )
        )
      }
    }
  );

/* Prepares the current working directory context and calls fn when done */
let prepareContext = (fn) =>
  loadConfig(
    (config) => {
      let project = Lib.Config.getCwdProject(config);
      switch project {
      | Some(project) =>
        Lib.Console.out("Found project: " ++ project.path);
        let git = Lib.Git.make();
        switch git {
        | Some(git) => fn((config, project, git))
        | None => Lib.Console.out(~color=Lib.Console.Red, "Not a git directory - exiting.")
        }
      | None =>
        Lib.Console.out(
          ~color=Lib.Console.Red,
          "No project found, please make sure you add the current path ("
          ++ (Lib.Project.getCwd() ++ ") to your config")
        )
      };
      ()
    }
  );

let createReview = () =>
  prepareContext(
    ((config, project, git)) => {
      let thread = createReview(config, project, git);
      let review = Lwt_main.run(thread);
      switch review {
      | None => ()
      | Some(review) =>
        /* Enable catching CTRL+C */
        Sys.catch_break(true);
        try {
          let thread =
            addChangeset(config, project, git, review)
            >>= (
              () => {
                let _ = Lib.History.addReview(Lib.History.make(), review.permaId);
                Lwt.return_unit
              }
            );
          Lwt_main.run(thread);
          ()
        } {
        | Sys.Break =>
          let thread = abandonAndDeleteReview(config, review);
          Lwt_main.run(thread)
        }
      }
    }
  );

let addReview = () =>
  prepareContext(
    ((config, project, git)) => {
      let h = Lib.History.make();
      switch h.reviews {
      | [] => Lib.Console.out("Could not add changeset - history is empty")
      | [permaId, ...rest] =>
        Lwt_main.run(addChangeset(config, project, git, Lib.Review.make(permaId)))
      }
    }
  );

/* Commands */
let create =
  Core.Command.basic(
    ~summary="[default] Creates a new review based on the current working directory",
    Core.Command.Spec.(empty),
    createReview
  );

let add =
  Core.Command.basic(
    ~summary="Adds the changeset that has been created in current directory to the last review in history",
    Core.Command.Spec.(empty),
    addReview
  );

let history =
  Core.Command.basic(
    ~summary="Shows the history of the last created reviews (new ones on top)",
    Core.Command.Spec.(empty),
    showHistory
  );

let command =
  Core.Command.group(
    ~summary="Create and modify crucible reviews",
    ~body=(~path as _) => createReview(), /* Use createReview if no subcommand is passed*/
    [("create", create), ("add", add), ("history", history)]
  );

let () = Core.Command.run(command);