
/* Getting a value from another module within this directory */
let message = "value from " ^ (Something.format);

/* Getting a value from an external library (specified in jbuild) */
let rx = Cohttp_lwt_body.to_string;
