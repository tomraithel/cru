open Lwt;

open Yojson.Basic.Util;

type createSuccess = {permaId: string};

type createResponse =
  | CreateError string
  | CreateSuccess createSuccess;

type abandonResponse =
  | AbandonError string
  | AbandonSuccess;

let makeCreateResponse responseString => {
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
      | Error error => CreateError error
      | Success body => CreateSuccess (makeCreateResponse body)
      }
  )
};

let abandon config review =>
  Api.post
    config
    ("/" ^ review.permaId ^ "/transition?action=action:abandonReview")
    (`Assoc []) >|= (
    fun a =>
      switch a {
      | Error error => AbandonError error
      | Success _ => AbandonSuccess
      }
  );
