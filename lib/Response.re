open Yojson;

open Yojson.Basic.Util;

let json =
  Yojson.Basic.from_string {|
{
  "reviewData": {
    "author": {
      "displayName": "Thomas Raithel",
      "userName": "thomas.raithel"
    },
    "name": "#{branch}: #{commit_abbrev}",
    "description": "test",
    "projectKey": "COMMERZ-REAL",
    "state": "Review",
    "allowReviewersToJoin": true
  }
}
|};

let name = json |> member "reviewData" |> member "name" |> to_string;
