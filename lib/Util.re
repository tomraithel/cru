let stringRepeat = (s, n) => {
  let rec helper = (s1, n1) =>
    switch n1 {
    | 0 => s1
    | _ => helper(s1 ++ s, n1 - 1)
    };
  helper("", n)
};