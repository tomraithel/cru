let cmdToList command => {
  let chan = Unix.open_process_in command;
  let res = ref ([]: list string);
  let rec process_otl_aux () => {
    let e = input_line chan;
    res := [e, ...!res];
    process_otl_aux ()
  };
  try (process_otl_aux ()) {
  | End_of_file =>
    let stat = Unix.close_process_in chan;
    (List.rev !res, stat)
  }
};

let cmdToList command => {
  let (l, _) = cmdToList command;
  l
};

let cmdToString command =>
  switch (cmdToList command) {
  | [hd, ...rest] => hd
  | _ => ""
  };
