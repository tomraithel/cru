open Lwt;

type t =
  | Error(string)
  | Success;

let createPayload = (project, git) =>
  Git.(
    Project.(
      `Assoc([
        ("repository", `String(project.repo)),
        ("changesets", `Assoc([("changesetData", `List([`Assoc([("id", `String(git.id))])]))]))
      ])
    )
  );

let add = (config, project, git, review) => {
  let payload = createPayload(project, git);
  Api.post(config, "/" ++ (review.Review.permaId ++ "/addChangeset"), payload)
  >|= (
    (a) =>
      switch a {
      | Error(error) => Error(error)
      | Success(_body) => Success
      }
  )
};
