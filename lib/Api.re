open Lwt;

open Core;

open Cohttp;

open Cohttp_lwt_unix;

open Config;

type error = string;

type success = string;

type response =
  | Error error
  | Success success;

let getApiEndpoint config => config.crucible.url ^ "/rest-service/reviews-v1";

let callClient m config relativeUrl payload => {
  let h =
    Header.of_list [
      ("Content-Type", "application/json"),
      ("Accept", "application/json")
    ];
  let auth = `Basic (config.crucible.userName, config.crucible.password);
  let headers = Header.add_authorization h auth;
  let body = Cohttp_lwt_body.of_string (Yojson.Basic.to_string payload);
  let url = getApiEndpoint config ^ relativeUrl;
  m (Uri.of_string url) ::headers ::body >>= (
    fun (resp, body) => {
      let isError =
        resp |> Response.status |> Code.code_of_status |> Code.is_error;
      switch isError {
      | false =>
        body |> Cohttp_lwt.Body.to_string >|= (fun body => Success body)
      | true =>
        body |> Cohttp_lwt.Body.to_string >|= (
          fun body => Error ("Error: \n" ^ body)
        )
      }
    }
  )
};

let post config relativeUrl payload =>
  callClient
    (fun url ::headers ::body => Client.post url ::headers ::body)
    config
    relativeUrl
    payload;

let delete config relativeUrl payload =>
  callClient
    (fun url ::headers ::body => Client.delete url ::headers ::body)
    config
    relativeUrl
    payload;
