open Cmd;

type t = {
  displayName: string,
  id: string,
  branch: string,
  commitAbbrev: string,
  commit: string
};

let getUserName () => cmdToString "git config user.name";

let getCommitId () => cmdToString "git rev-parse --short HEAD";

let getBranch () => cmdToString "git rev-parse --abbrev-ref HEAD";

let getCommitAbbrev () => cmdToString "git log -1 --pretty=%B --abbrev-commit";

let getCommit () => cmdToString "git log -1 --pretty=%B";

let getInsideWorkingDir () =>
  cmdToString "git rev-parse --is-inside-work-tree 2>/dev/null";

let make () =>
  switch (getInsideWorkingDir ()) {
  | "true" =>
    Some {
      displayName: getUserName (),
      id: getCommitId (),
      branch: getBranch (),
      commitAbbrev: getCommitAbbrev (),
      commit: getCommit ()
    }
  | _ => None
  };

let toString v =>
  v.displayName ^
  "\n" ^ v.id ^ "\n" ^ v.branch ^ "\n" ^ v.commitAbbrev ^ "\n" ^ v.commit;
