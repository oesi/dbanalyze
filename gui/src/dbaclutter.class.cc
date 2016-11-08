#include "dbaclutter.class.h"
#include "constraint_fk.class.h"
#include <iostream>

dbaclutter::dbaclutter(void* mw)
{
	char** args;
	int cnt = 0;
	if(CLUTTER_INIT_SUCCESS !=clutter_init (&cnt, &args))
	{
		std::cout << "clutter init failed" << std::endl;
		return;
	}

	this->mw = mw;

	clutter0 = gtk_clutter_embed_new();
	Gtk::Widget *mmclutter = Glib::wrap(clutter0);
	this->add(*mmclutter);
	stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter0));
}

dbaclutter::~dbaclutter()
{
}

void dbaclutter::draw(std::vector<table> *tablelist)
{
	ClutterColor stage_color = { 255, 255, 255, 255 };

	//clutter_actor_set_size (stage, 100, 100);
	clutter_actor_set_background_color (stage, &stage_color);

	for(unsigned int i=0; i<tablelist->size();i++)
	{
		drawtable(&tablelist->at(i));
	}
}

void dbaclutter::drawtable(table *tbl)
{
	ClutterLayoutManager *box_layout = clutter_box_layout_new ();
	clutter_box_layout_set_orientation (CLUTTER_BOX_LAYOUT (box_layout), CLUTTER_ORIENTATION_VERTICAL);
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (box_layout), 0);
	ClutterActor *box = clutter_actor_new ();
	clutter_actor_set_layout_manager (box, box_layout);
	ClutterColor box_color = { 56, 147, 254, 255 };
	clutter_actor_set_background_color (box, &box_color);
	clutter_actor_set_position (box, 50/*x*/, 60/*y*/);
	clutter_actor_set_reactive(box, true);
	clutter_actor_add_child(stage, box);

	ClutterActor *headerRect = clutter_actor_new ();
	clutter_actor_set_background_color(headerRect, &box_color);

	clutter_actor_set_x_expand(headerRect, true);
	clutter_actor_add_child(box, headerRect);

	ClutterActor *headerText = clutter_text_new();
	clutter_text_set_text(CLUTTER_TEXT(headerText), tbl->tablename.c_str());
	clutter_actor_add_child(headerRect, headerText);

	ClutterLayoutManager *body_layout = clutter_box_layout_new ();
	clutter_box_layout_set_orientation (CLUTTER_BOX_LAYOUT (body_layout), CLUTTER_ORIENTATION_VERTICAL);
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (body_layout), 5);
	ClutterColor bodyColor = { 192, 214, 255, 255 };
	ClutterActor *bodyRect = clutter_actor_new ();
	clutter_actor_set_layout_manager (bodyRect, box_layout);
	clutter_actor_set_background_color(bodyRect, &bodyColor);

	ClutterMargin bodymargin = {2.f, 2.f, 2.f, 2.f};
	clutter_actor_set_margin(bodyRect, &bodymargin);

	clutter_actor_add_child(box, bodyRect);

	// Draw Columns
	for(unsigned int i = 0; i < tbl->columnlist.size(); i++)
	{
		ClutterActor *rowText = clutter_text_new();
		std::string datatype = tbl->columnlist[i].getColumnType();
		std::string bezeichnung = tbl->columnlist[i].columnname + " " + datatype;
		
		if(tbl->columnlist[i].is_nullable!="TRUE")
			bezeichnung += " NN";

		std::vector<constraint*> colconstr = tbl->getConstraints(tbl->columnlist[i].columnname);
		for(unsigned int j=0;j < colconstr.size();j++)
		{
			constraint_fk* fk = dynamic_cast< constraint_fk* >( colconstr[j] );
			if(fk)
			{
				bezeichnung += " FK";
			}
		}
		if(std::find(tbl->pk.begin(), tbl->pk.end(), tbl->columnlist[i].columnname)!=tbl->pk.end())
			bezeichnung += " PK";

		clutter_text_set_text(CLUTTER_TEXT(rowText), bezeichnung.c_str());
		clutter_actor_set_x_expand(rowText, true);
		clutter_actor_add_child(bodyRect, rowText);
	}


	ClutterAction *dragaction;
	dragaction = clutter_drag_action_new();
	clutter_actor_add_action(box, dragaction);
	//g_signal_connect (dragaction, "drag-motion", G_CALLBACK (on_actor_dragmotion), NULL);

}

void dbaclutter::export_graph(std::string filename, std::string format)
{
	int width = clutter_actor_get_width(this->stage);
	int height = clutter_actor_get_height(this->stage);
	guchar *buffer = clutter_stage_read_pixels((ClutterStage*)this->stage,0,0,width,height);
	pixbuf = Gdk::Pixbuf::create_from_data(buffer, Gdk::COLORSPACE_RGB,true, 8, width, height, width*4);
	pixbuf->save(filename.c_str(),format.c_str());
	pixbuf.reset();
}