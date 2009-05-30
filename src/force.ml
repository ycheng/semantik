(*
 Thomas Nagy 2007-2009 GPLV3
*)

open Printf
open Hashtbl
open Apis

let xpad = 25.
let ypad = 25.

type nono = { bx: sbox; mutable child: nono array;}
type packing = { mutable rt: nono; mutable pw: float; mutable ph: float; }
type all = {mutable packs: packing list; }

let rec height p =
	(* COM: height of the tree, sum of all height of boxes *)
	let y = Array.fold_left (fun acc x -> acc +. height x) 0. p.child
	max y (p.bx.sh *. 2.)

let rec height_pad p =
	(* COM: height of the tree taking the y padding into account *)
	let y = Array.fold_left (fun acc x -> acc +. height_pad x) 0. p.child +. float_of_int(Array.length p.child - 1) *. ypad
	max y (p.bx.sh *. 2.)

let find_middle p =
	let y = height p

	(* COM: create an array of the ratio height / total height *)
	let prev = ref 0.
	let fufu x =
		prev := !prev +. height x
		abs_float(0.5 -. !prev /. y)
	let mp = Array.map fufu p.child

	(* COM: keep the optimum, that is closer to 0 *)
	let ev (i, min, v) x =
		if x < v then
			(i+1, i, x)
		else
			(i+1, min, v)

	(* COM: iterate with tuples, hmm :-) *)
	let (a, b, c) = Array.fold_left ev (0, 0, 1.) mp
	b

let rec depth x =
	if Array.length x.child < 1 then
		1
	else
		Array.fold_left (fun v x -> max v (1 + depth x)) 1 x.child

let deb x =
	let mid = find_middle x.rt

	let total = ref (height_pad x.rt)
	if Array.length x.rt.child > 1 then
		total := !total -. ypad

	(* COM: maximum width at each level of the tree *)
	let arr = Array.make (depth x.rt) 0.
	let rec maxi x d =
		arr.(d) <- max arr.(d) x.bx.sw
		Array.iter (fun x -> maxi x (d+1)) x.child
	maxi x.rt 0

	let rec pack_left y pp hh n =
		let x = y.bx and p = pp.bx
		x.sx <- p.sx -. 2. *. arr.(n-1) -. x.sw +. p.sw -. xpad

		let mh = height_pad y
		x.sy <- hh +. mh /. 2.

		let tmp = ref (hh)
		for i=0 to Array.length y.child -1 do
			tmp := pack_left y.child.(i) y (!tmp) (n+1)
			if i < Array.length y.child -1 then
				tmp := !tmp +. ypad
		hh +. mh

	let rec pack_right y pp hh n =
		let x = y.bx and p = pp.bx
		x.sx <- p.sx +. 2. *. arr.(n-1) +. x.sw -. p.sw +. xpad

		let mh = height_pad y
		x.sy <- hh +. mh /. 2.

		let tmp = ref (hh)
		for i=0 to Array.length y.child -1 do
			tmp := pack_right y.child.(i) y (!tmp) (n+1)
			if i < Array.length y.child -1 then
				tmp := !tmp +. ypad
		hh +. mh

	(* COM: for left and right, compute the y position of the item for vertical alignment *)
	let size_left = ref 0.
	if mid > 0 then
		size_left := float_of_int(mid) *. ypad

	for i=0 to mid do
		size_left := !size_left +. height_pad x.rt.child.(i)
	let size_right = ref (!total -. !size_left)

	(* COM: now apply the padding *)
	let buf_left = ref (x.rt.bx.sy -. !size_left /. 2.)
	for i=0 to mid do
		buf_left := ypad +. pack_left x.rt.child.(i) x.rt (!buf_left) 1

	let buf_right = ref (x.rt.bx.sy -. !size_right /. 2.)
	for i=mid+1 to Array.length x.rt.child - 1 do
		buf_right := ypad +. pack_right x.rt.child.(i) x.rt (!buf_right) 1

let reorg_fixed () =

	ensure_init ()

	for i=0 to container.size-1 do
		let a = container.prob.(i)
		a.fx <- 0.0
		a.fy <- 0.0

	let mp = Array.map (fun x -> {bx = x; child = [||]}) container.prob

	let ch x =
		try
			let tmp = Hashtbl.find container.padres x
			Array.map (fun y -> mp.(y)) tmp
		with
			| Not_found ->
				[||]

	(* COM: mp is an array of nono struct *)
	for i=0 to container.size-1 do
		mp.(i).child <- ch i

	(* COM: find the objects which do not have an ancestor, ban is a hashtable of nono struct *)
	let ban = Hashtbl.create container.size
	Hashtbl.iter (fun _ vv -> Array.iter (fun x -> Hashtbl.add ban x 0) vv) container.padres

	(* COM: for all roots, apply the layout - we create a list of tree (packing struct) *)
	let all_root = {packs = [];}
	for i=0 to container.size - 1 do
		try
			let _ = Hashtbl.find ban i
			()
		with
			| Not_found ->
				all_root.packs <- all_root.packs @ [ {rt = mp.(i); pw=0.; ph=0.} ]

	(* COM: Printf.fprintf stderr "found %d roots\n" (List.length all_root.packs) *)
	List.iter (fun x -> if Array.length x.rt.child > 0 then deb x) all_root.packs

	container.count <- 1

