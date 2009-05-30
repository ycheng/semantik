(*
 Thomas Nagy 2007-2009 GPLV3
*)

open Printf
open Hashtbl
open Random
open Apis

exception Finished

let sign x =
	if x > 0. then 1.0 else - 1.0

let ortho_comp p v =
	if p > 0. then
		if v > 0. then
			2. -. v
		else
			0. -. v -. 2.
	else
		v

let ortho key val_lst =

	(* pivot and father *)
	let p_a = container.prob.(Hashtbl.find container.rel key)
	let p_b = container.prob.(key)
	let vrx = p_a.sx -. p_b.sx
	let vry = p_a.sy -. p_b.sy

	let ab = sqrt (vrx *. vrx +. vry *. vry)

	for i = 0 to Array.length val_lst - 1 do
		for j = i + 1 to Array.length val_lst - 1 do

			(* COM: recompute everytime *)
			let p_c = container.prob.(val_lst.(i))
			let vcx = p_c.sx -. p_b.sx
			let vcy = p_c.sy -. p_b.sy
			let cb = sqrt (vcx *. vcx +. vcy *. vcy)


			let p_d = container.prob.(val_lst.(j))
			let vdx = p_d.sx -. p_b.sx
			let vdy = p_d.sy -. p_b.sy

			let db = sqrt (vdx *. vdx +. vdy *. vdy)

			let x1 = cb *. ab and x2 = db *. ab

			if true (*|| ((x1< -0.1 || x1>0.1) && (x2< -0.1 || x2>0.1)) *) then
				let p1 = ortho_comp (vcx *. vrx +. vcy *. vry) ((vcx *. vry -. vcy *. vrx) /. x1)
				let p2 = ortho_comp (vdx *. vrx +. vdy *. vry) ((vdx *. vry -. vdy *. vrx) /. x2)

				if p1 < p2 then
					(* apply the sort *)
					(*Printf.fprintf stderr "replacing %d and %d\n" val_lst.(i) val_lst.(j)*)
					let tmpx = p_c.sx and tmpy = p_c.sy
					p_c.sx <- p_d.sx
					p_c.sy <- p_d.sy
					p_d.sx <- tmpx
					p_d.sy <- tmpy

let ortho2 key val_lst =
	try
		ortho key val_lst
	with
		| Not_found ->
			()

let calc_gap ddx ddy w h =
	let xdos = 1. /. (w *. w) and ydos = 1. /. (h *. h)
	let absdx = abs_float(ddx) and absdy = abs_float(ddy)

	if absdx > absdy then
		let k = absdy /. absdx
		let kdos = k *. k
		sqrt( (1. +. kdos) /. (kdos *. ydos +. xdos) )
	else
		let k = absdx /. absdy
		let kdos = k *. k
		sqrt( (1. +. kdos) /. (kdos *. xdos +. ydos) )

let check_pos () =
	let size_max = container.size-1
	for i=0 to size_max do
		let a = container.prob.(i)
		for j=i+1 to size_max do
			let b = container.prob.(j)

			let dx = b.sx -. a.sx and dy = b.sy -. a.sy
			let dist_dos = dx *. dx +. dy *. dy
			let dist = sqrt(dist_dos)

			if dist < 1. then
				let rr = Random.float 20.0
				if dx > dy then
					a.sx <- a.sx +. rr
					b.sx <- b.sx -. rr
				else
					a.sy <- a.sy +. rr
					b.sy <- b.sy -. rr

let spring key val_lst =
	let size = Array.length val_lst - 1
	for i = 0 to size do
		let c = val_lst.(i)
		let a = container.prob.(key) and b = container.prob.(c)

		let dx = b.sx -. a.sx and dy = b.sy -. a.sy
		let ddx = abs_float(dx) and ddy = abs_float(dy)
		let gap1 = calc_gap ddx ddy a.sw a.sh
		let gap2 = calc_gap ddx ddy b.sw b.sh

		let dist = sqrt(dx *. dx +. dy *. dy)

		let fac =
			if dist > gap1 +. gap2 then
				(cst_spring -. dist +. gap1 +. gap2) /. dist
			else
				(cst_spring -. dist) /. dist

		a.fx <- a.fx -. dx *. fac
		a.fy <- a.fy -. dy *. fac

		b.fx <- b.fx +. dx *. fac
		b.fy <- b.fy +. dy *. fac

let iter_forces arr =
	let size_max = Array.length arr - 1
	(*let ideal_ = (cst_electric) ** (2.)*)
	for i=0 to size_max do

		let a = container.prob.(arr.(i))
		for j=i+1 to size_max do
			let b = container.prob.(arr.(j))

			let dx = b.sx -. a.sx and dy = b.sy -. a.sy
			let dist = sqrt(dx *. dx +. dy *. dy)

			(* COM: pow 7 : non-overlapping rectangles *)
			let fac = (200. ** 7.) /. (dist ** 7. +. 0.01)  +.  (150. ** 2.5) /. (dist ** 2.5 +. 0.01)

			a.fx <- a.fx -. dx *. fac
			a.fy <- a.fy -. dy *. fac

			b.fx <- b.fx +. dx *. fac
			b.fy <- b.fy +. dy *. fac

let iterate_impl gee =

	container.count <- container.count + 1
	check_pos ()

	(*if container.count mod 47 = 0 then
		Hashtbl.iter ortho2 container.padres
		check_pos ()*)

	for i=0 to container.size-1 do
		let a = container.prob.(i)
		a.fx <- 0.0
		a.fy <- 0.0

	Hashtbl.iter spring container.padres
	Hashtbl.iter (fun _ v -> iter_forces (Array.of_list v)) gee

	let size_max = Array.length container.prob - 1

	(* COM: now add the forces *)
	let energy = ref 0.0
	for i=0 to size_max do
		let a = container.prob.(i)

		(* COM: not too far *)
		if a.fx > 1000. then a.fx <- 1000.
		if a.fy > 1000. then a.fy <- 1000.
		if a.fx < -1000. then a.fx <- -1000.
		if a.fy < -1000. then a.fy <- -1000.

		if a.fx *. a.fx < 0.0001 then a.fx <- 0.0
		if a.fy *. a.fy < 0.0001 then a.fy <- 0.0

		(* COM: compute the speed for the energy *)
		let vx = (a.fx *. cst_time)
		let vy = (a.fy *. cst_time)

		energy := !energy +. (vx *. vx +. vy *. vy) (* *. (a.sw *. a.sh) *)

		a.sx <- a.sx +. (a.fx *. cst_time)
		a.sy <- a.sy +. (a.fy *. cst_time)

	!energy

	(* debug_all() *)

let reorg_force x =

	ensure_init ()


	let groups = Hashtbl.create 5

	let rec father_luke ivv gru =
		try
			let nu = Hashtbl.find gru ivv
			father_luke nu gru
		with
			| Not_found -> ivv

	let foufou b =
		let x = Hashtbl.find container.id_to_idx b.a and y = Hashtbl.find container.id_to_idx b.b

		let vati = father_luke x container.rel
		try
			let vec = Hashtbl.find groups vati
			Hashtbl.replace groups vati (vec@[y])
		with
			| Not_found -> Hashtbl.replace groups vati [y]


	List.iter foufou container.links


	(* COM: now the groups should contain all fathers from the map, with the list of all descendants *)

	let size = float_of_int(container.size)
	try
		while true do
			let en = iterate_impl groups
			if en < 0.05 *. size then raise Finished
			if container.count > maxiter then raise Finished
			(* Printf.fprintf stderr "%f %d\n" en container.size *)
	with
		| Finished ->
			()
		| _ ->
			Printf.fprintf Pervasives.stderr "error in iterate"



			flush_all()

	(* COM: now scale the view *)
	(*
	let scx = ref 5000000
	let scy = ref 5000000
	for i = 0 to Array.length val_lst - 1 do
		for j = i + 1 to Array.length val_lst - 1 do

			(* COM: recompute everytime *)
			let p_c = container.prob.(val_lst.(i))
			let vcx = p_c.sx -. p_b.sx
			let vcy = p_c.sy -. p_b.sy
			let cb = sqrt (vcx *. vcx +. vcy *. vcy)


			let p_d = container.prob.(val_lst.(j))
			let vdx = p_d.sx -. p_b.sx
			let vdy = p_d.sy -. p_b.sy

			let db = sqrt (vdx *. vdx +. vdy *. vdy)

			let x1 = cb *. ab and x2 = db *. ab

			let gap1 = calc_gap ddx ddy a.sw a.sh
			let gap2 = calc_gap ddx ddy b.sw b.sh

			scx := min !scx gap1
			scy := min !scy gap2

	Printf.fprintf stderr "could reduce by %d %d"
	Printf.fprintf stderr "end in iterate %d\n" container.count
	flush_all ()
	*)


