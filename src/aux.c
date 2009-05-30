// Thomas Nagy 2007-2009 GPLV3

#include "math.h"
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/alloc.h>

void iterate_pb(int v)
{
	static value *iterate_fun = NULL;
	if (!iterate_fun) iterate_fun = caml_named_value("iterate_fun");
	if (!iterate_fun) return;
	callback(*iterate_fun, Val_int(v));
}

void cleanup_pb()
{
	static value * cleanup_fun = NULL;
	if (!cleanup_fun) cleanup_fun = caml_named_value("cleanup_fun");
	if (!cleanup_fun) return;

	callback(*cleanup_fun, Val_int(0));
}

void add_box(int pos, double x, double y, double w, double h)
{
	CAMLparam0();

	static value * add_box_fun = NULL;
	if (!add_box_fun) add_box_fun = caml_named_value("add_box_fun");

	CAMLlocalN(va, 5);

	va[0] = Val_int(pos);
	va[1] = copy_double(x);
	va[2] = copy_double(y);
	va[3] = copy_double(w);
	va[4] = copy_double(h);

	callbackN(*add_box_fun, 5, va);

	CAMLreturn0;
}

void add_cstr(int x, int y)
{
	static value * add_cstr_fun = NULL;
	if (!add_cstr_fun) add_cstr_fun = caml_named_value("add_cstr_fun");
	callback2(*add_cstr_fun, Val_int(x), Val_int(y));
}

double get_x(int pos)
{
	static value * get_x_fun = NULL;
	if (!get_x_fun) get_x_fun = caml_named_value("get_x_fun");
	double d = Double_val(callback(*get_x_fun, Val_int(pos)));
	if (isnan(d)) d=0.;
	if (d>55000.||d<-55000.) d=0.;
	return d;
}

double get_y(int pos)
{
	static value * get_y_fun = NULL;
	if (!get_y_fun) get_y_fun = caml_named_value("get_y_fun");
	double d = (Double_val(callback(*get_y_fun, Val_int(pos))));
	if (isnan(d)) d=0.;
	if (d>55000.||d<-55000.) d=0.;
	return d;
}

// something different
void set_rectangles(int ax1, int ax2, int ay1, int ay2, int ap,
	int bx1, int bx2, int by1, int by2, int bp)
{
	CAMLparam0();

	static value * set_rects_fun = NULL;
	if (!set_rects_fun) set_rects_fun = caml_named_value("set_rects_fun");

	CAMLlocalN(va, 10);

	va[0] = Val_int(ax1);
	va[1] = Val_int(ax2);
	va[2] = Val_int(ay1);
	va[3] = Val_int(ay2);
	va[4] = Val_int(ap);
	va[5] = Val_int(bx1);
	va[6] = Val_int(bx2);
	va[7] = Val_int(by1);
	va[8] = Val_int(by2);
	va[9] = Val_int(bp);

	callbackN(*set_rects_fun, 10, va);

	CAMLreturn0;
}

int get_vertex_x(int pos)
{
	static value * get_vx_fun = NULL;
	if (!get_vx_fun) get_vx_fun = caml_named_value("get_vx_fun");
	int d = Int_val(callback(*get_vx_fun, Val_int(pos)));
	if (isnan(d)) d=0.;
	if (d>55000.||d<-55000.) d=0.;
	return d;
}

int get_vertex_y(int pos)
{
	static value * get_vy_fun = NULL;
	if (!get_vy_fun) get_vy_fun = caml_named_value("get_vy_fun");
	int d = Int_val(callback(*get_vy_fun, Val_int(pos)));
	if (isnan(d)) d=0.;
	if (d>55000.||d<-55000.) d=0.;
	return d;
}

int num_seg()
{
	static value * num_seg_fun = NULL;
	if (!num_seg_fun) num_seg_fun = caml_named_value("num_seg_fun");
	return Int_val(callback(*num_seg_fun, Val_int(0)));
}

