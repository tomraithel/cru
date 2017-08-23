open Lwt;

open Yojson.Basic.Util;

type t = {permaId: string};

type response =
  | Error string
  | Success t;

let make responseString => {
  let json = Yojson.Basic.from_string responseString;
  {permaId: json |> member "permaId" |> member "id" |> to_string}
};

let createPayload config project git =>
  Git.(
    Config.(
      Project.(
        `Assoc [
          (
            "reviewData",
            `Assoc [
              (
                "author",
                `Assoc [
                  ("displayName", `String git.displayName),
                  ("userName", `String config.crucible.userName)
                ]
              ),
              ("name", `String (git.branch ^ ": " ^ git.commitAbbrev)),
              ("description", `String git.commit),
              ("projectKey", `String project.key),
              ("state", `String "Review"),
              ("allowReviewersToJoin", `Bool project.allowReviewersToJoin)
            ]
          )
        ]
      )
    )
  );

let create config project git => {
  let payload = createPayload config project git;
  Api.post config "" payload >|= (
    fun a =>
      switch a {
      | Error error => Error error
      | Success body => Success (make body)
      }
  )
};
