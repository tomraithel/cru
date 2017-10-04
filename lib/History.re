open Yojson.Basic.Util;

open Core;

type t = {reviews: list HistoryReview.t};

let historyPath = Core.Sys.getenv_exn "HOME" ^ "/.review.history.json";

let make () =>
  if (Core.Sys.file_exists_exn historyPath) {
    let history = Yojson.Basic.from_file historyPath;
    {
      reviews:
        history |> member "reviews" |> to_list |>
        List.map f::HistoryReview.make
    }
  } else {
    {reviews: []}
  };

let toJson history =>
  `Assoc [("reviews", `List (List.map history.reviews f::(fun v => `String v)))];

let addReview history review => {
  let newHistory = {reviews: [review, ...history.reviews]};
  Yojson.Basic.to_file historyPath (toJson newHistory);
  newHistory
};
