(*
 Thomas Nagy 2007-2009 GPLV3
*)

open Fixed
open Force
open Printf
open Hashtbl

let iterate x =
	if x = 0 then
		reorg_fixed ()
	else if x = 1 then
		reorg_fixed ()
		reorg_force x
	else
		reorg_force x
	flush_all()

Callback.register "iterate_fun" iterate


type node = {x: float; y: float}
(* COM: the list of arcs in a hash table *)

let pad = 20

exception Finished
exception Disconnected

type line = {x1: int; x2: int; y1: int; y2: int;}
type point = {mutable x: int; y: int;}

(* COM: the result is placed statically into vertices *)
let vertices = ref [||]

let set_rects ax1 ax2 ay1 ay2 bp1  bx1 bx2 by1 by2 bp2 =
	(*
		Yet too difficult for many
		thinking in closures is easy
		can you understand the following ?
	*)
	(* COM: Printf.fprintf stderr "%d %d %d %d  %d         %d %d %d %d  %d\n"  ax1 ax2 ay1 ay2 bp1  bx1 bx2 by1 by2 bp2*)

	let cx1 = - pad + min ax1 bx1
	let cx2 =   pad + max ax2 bx2
	let cy1 = - pad + min ay1 by1
	let cy2 =   pad + max ay2 by2

	let hrz = ref [cx1; cx2]
	let ver = ref [cy1; cy2]

	if bx1 > ax2 then
		hrz := (bx1 + ax2) / 2 :: !hrz
	if ax1 > bx2 then
		hrz := (ax1 + bx2) / 2 :: !hrz

	if by1 > ay2 then
		ver := (by1 + ay2) / 2 :: !ver
	if ay1 > by2 then
		ver := (ay1 + by2) / 2 :: !ver

	let sx = ref 0 and sy = ref 0 and ex = ref 0 and ey = ref 0
	let store a b =
		ver := b::!ver
		hrz := a::!hrz
	let s_store a b =
		store a b
		sx := a
		sy := b
	let e_store a b =
		store a b
		ex := a
		ey := b

	(*ver := ((ax1 + ax2)/2)::!ver
	ver := ((bx1 + bx2)/2)::!ver
	hrz := ((ay1 + ay2)/2)::!hrz
	hrz := ((by1 + by2)/2)::!hrz*)

	if bp1 == 0 then
		s_store ((ax1 + ax2)/2) ay1
	if bp2 == 0 then
		e_store ((bx1 + bx2)/2) by1

	if bp1 == 1 then
		s_store ax1 ((ay1 + ay2)/2)
	if bp2 == 1 then
		e_store bx1 ((by1 + by2)/2)

	if bp1 == 2 then
		s_store ((ax1 + ax2)/2) ay2
	if bp2 == 2 then
		e_store ((bx1 + bx2)/2) by2

	if bp1 == 3 then
		s_store ax2 ((ay1 + ay2)/2)
	if bp2 == 3 then
		e_store bx2 ((by1 + by2)/2)


	if bp1 == 1 && bp2 == 3 && bx2 >= ax1 - pad then
		hrz := ax1 - pad :: bx2 + pad :: !hrz
	else if bp1 == 3 && bp2 == 1 && ax2 >= bx1 - pad then
		hrz := bx1 - pad :: ax2 + pad :: !hrz
	else if bp1 == 0 && bp2 == 2 && by2 >= ay1 - pad then
		ver := ay1 - pad :: by2 + pad :: !ver
	else if bp1 == 2 && bp2 == 0 && ay2 >= by1 - pad then
		ver := by1 - pad :: ay2 + pad :: !ver

	let init = {x= !sx; y= !sy;}
	let endp = {x= !ex; y= !ey;}

	let is_linked2 a b =
		let is_vertical = a.x == b.x
		try
			if is_vertical then
				if a.y < ay2 && b.y > ay1 then
					if ax1 <= a.x && a.x <= ax2 then
						raise Disconnected
				if a.y < by2 && b.y > by1 then
					if bx1 <= a.x && a.x <= bx2 then
						raise Disconnected
			else
				if a.x < ax2 && b.x > ax1 then
					if ay1 <= a.y && a.y <= ay2 then
						raise Disconnected
				if a.x < bx2 && b.x > bx1 then
					if by1 <= a.y && a.y <= by2 then
						raise Disconnected
			true
		with
			| Disconnected ->
				false

	let is_linked a b =
		if a.x != b.x && a.y != b.y then
			false
		else if a.x == cx1 && b.x == cx1 then
			true
		else if a.x == cx2 && b.x == cx2 then
			true
		else if a.y == cy1 && b.y == cy1 then
			true
		else if a.y == cy2 && b.y == cy2 then
			true
		else
			if a.x >= b.x && a.y >= b.y then
				is_linked2 b a
			else
				is_linked2 a b

	let graph = ref []
	let x_arr = Array.of_list !hrz
	let y_arr = Array.of_list !ver

	(* COM: distance to a point *)
	let dist = Hashtbl.create 30
	(* COM: previous element *)
	let prev = Hashtbl.create 30


	(* COM: initialize all *)
	for i=0 to Array.length x_arr - 1 do
		for j=0 to Array.length y_arr -1 do
			let pt = {x= x_arr.(i); y= y_arr.(j);}
			graph := pt::!graph
			Hashtbl.replace prev pt init
			Hashtbl.replace dist pt 999999999

	Hashtbl.replace dist init 0
	let q = ref (!graph)

	try
		while true do
			let func x y =
				let dx = Hashtbl.find dist x
				let dy = Hashtbl.find dist y

				if dx < dy then
					x
				else
					y

			let elem = List.fold_left func endp (!q)
			if elem == endp then
				raise Finished

			q := List.fold_left (fun lst x -> if compare x elem == 0 then lst else x::lst) [] (!q)
			let arr = Array.of_list !q
			for i=0 to Array.length arr - 1 do
				let cand = arr.(i)
				if is_linked cand elem then
					let alt = 1 + abs(elem.x - cand.x) + abs(elem.y - cand.y) + Hashtbl.find dist elem
					if alt < Hashtbl.find dist cand then
						Hashtbl.replace dist cand alt
						Hashtbl.replace prev cand elem
	with
		| Finished ->
			()

	(* COM: now extract the shortest path *)
	let rec out_lst (el:point) =
		try
			let x = Hashtbl.find prev el
			if x == init then
				raise Finished
			(*Printf.printf "%d %d\n" x.x x.y *)
			out_lst(x) @ [el]
		with
			| Finished ->
				[el]

	vertices := Array.of_list (out_lst endp)

let get_vx p =
	(!vertices).(p).x

let get_vy p =
	(!vertices).(p).y

let num_seg () =
	Array.length !vertices

Callback.register "set_rects_fun" set_rects
Callback.register "get_vx_fun" get_vx
Callback.register "get_vy_fun" get_vy
Callback.register "num_seg_fun" num_seg


