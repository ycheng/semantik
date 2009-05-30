(*
 Thomas Nagy 2007-2009 GPLV3
*)

val cst_electric : float
val cst_spring : float
val cst_time : float
val pow : float
val maxiter : int
val do_trace : bool
val trace : string -> unit
type box = {
  id : int;
  mutable x : float;
  mutable y : float;
  w : float;
  h : float;
}
type pair = { a : int; b : int; }
type sbox = {
  mutable sx : float;
  mutable sy : float;
  mutable sw : float;
  mutable sh : float;
  mutable vx : float;
  mutable vy : float;
  mutable fx : float;
  mutable fy : float;
}
type problem = {
  mutable boxes : box list;
  mutable links : pair list;
  mutable id_to_idx : (int, int) Hashtbl.t;
  mutable prob : sbox array;
  mutable size : int;
  mutable count : int;
  mutable padres : (int, int array) Hashtbl.t;
  mutable rel : (int, int) Hashtbl.t;
}
val empty_problem : unit -> problem
val container : problem
val ensure_init : unit -> unit

val debug_all : unit -> unit

