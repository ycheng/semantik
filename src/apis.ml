(*
 Thomas Nagy 2007-2009 GPLV3
*)

open Printf
open Hashtbl
open Random

let cst_electric = 16.0
let cst_spring = 190.0
let cst_time = 0.1
let pow = 1.82
let maxiter = 2000

let do_trace = false

let maxf x y =
	if x > y then x else y

let trace msg =
	if do_trace then
		Printf.fprintf stderr "%s\n" msg
		flush_all()
	else
		()

type box = { id: int; mutable x:float; mutable y:float; w:float; h:float; }
type pair = { a: int; b: int; }
type sbox = { mutable sx: float; mutable sy: float; mutable sw: float; mutable sh: float;
mutable vx: float; mutable vy: float; mutable fx: float; mutable fy: float; }

type problem = {
	mutable boxes: box list;
	mutable links: pair list;
	mutable id_to_idx: (int, int) Hashtbl.t;
	mutable prob: sbox array;
	mutable size: int;
	mutable count: int;
	mutable padres: (int, int array) Hashtbl.t;
	mutable rel: (int, int) Hashtbl.t;
	}

let empty_problem () =
	{
	boxes = [];
	links = [];
	id_to_idx = Hashtbl.create 10;
	prob = [||];
	size = 0;
	count = 0;
	padres = Hashtbl.create 10;
	rel = Hashtbl.create 100;
	}

let container = empty_problem()
let cleanup () =
	trace "cleanup";
	container.boxes <- []
	container.links <- []
	container.id_to_idx <- Hashtbl.create 10
	container.prob <- [||]
	container.size <- 0
	container.count <- 0
	container.padres <- Hashtbl.create 10
	container.rel <- Hashtbl.create 100

let add_box_fun i a b c d =
	container.boxes <- container.boxes @ [{id=i;x=a;y=b;w=c;h=d;}]

let add_cstr_fun x y =
	container.links <- container.links @ [{a=x; b=y;}]

let get_x_fun id =
	container.prob.(Hashtbl.find container.id_to_idx id).sx

let get_y_fun id =
	container.prob.(Hashtbl.find container.id_to_idx id).sy

let debug_all() =
	let toto = open_out "/tmp/debug.txt"
	let debug_sbox a =
		Printf.fprintf toto "%f %f %f %f     %f %f %f %f\n" a.sx a.sy a.sw a.sh    a.vx a.vy a.fx a.fy
	Array.iter debug_sbox container.prob
	close_out toto

let ensure_init () =

	if container.count == 0 then
		let sbox_ctor b =
			{sx=b.x; sy=b.y; sw=b.w; sh=b.h; vx=0.; vy=0.; fx=0.; fy=0.;}

		let tmphash = Hashtbl.create 10
		let link_add b =
			let x = Hashtbl.find container.id_to_idx b.a and y = Hashtbl.find container.id_to_idx b.b
			Hashtbl.replace container.rel y x
			try
				let lst = Hashtbl.find tmphash x
				Hashtbl.replace tmphash x (lst @ [y])
			with
				| Not_found ->
					Hashtbl.add tmphash x [y]
			(*Hashtbl.add container.rel y x*)

		let lst_sbox = ref []
		let hash_add i b =
			let _ = Hashtbl.add container.id_to_idx b.id i
			lst_sbox := (!lst_sbox) @ [sbox_ctor b]
			i + 1

		container.size <- List.fold_left hash_add 0 container.boxes
		container.prob <- Array.of_list (!lst_sbox)
		List.iter link_add container.links

		let arrayze key value =
			Hashtbl.replace container.padres key (Array.of_list value)
		Hashtbl.iter arrayze tmphash

Callback.register "cleanup_fun" cleanup
Callback.register "add_box_fun" add_box_fun
Callback.register "get_x_fun" get_x_fun
Callback.register "get_y_fun" get_y_fun
Callback.register "add_cstr_fun" add_cstr_fun

