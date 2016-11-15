#include "dbaclutter.class.h"
#include "constraint_fk.class.h"
#include <iostream>
#include <gtkmm.h>


void on_actor_dragmotion (ClutterDragAction* /*action*/,
	ClutterActor *actor,
	gfloat /*delta_x*/,
	gfloat /*delta_y*/,
	gpointer user_data)
{
	dbaclutter *obj = reinterpret_cast<dbaclutter*>(user_data);
	ClutterActor *parent;
	parent = clutter_actor_get_parent(actor);

	// Move Draged Actor to foreground
	clutter_actor_set_child_above_sibling(parent,actor,NULL);

	std::string name = clutter_actor_get_name(actor);
	ClutterActor *t1;
	ClutterActor *t2;
	ClutterActor *line;
	for(auto &i:obj->constraintlist)
	{
		std::string key = i.first;
		constraint_fk *fk = i.second;
		t1=NULL;
		t2=NULL;
		column *col = static_cast<column*>(fk->source);
		table *tbl = static_cast<table*>(col->tablepntr);
		std::string tablename_source = tbl->schemaname+"."+tbl->tablename;

		column *col_target = static_cast<column*>(fk->target);
		table *tbl_target = static_cast<table*>(col_target->tablepntr);
		std::string tablename_target = tbl_target->schemaname+"."+tbl_target->tablename;

		if(name == tablename_source || name == tablename_target)
		{
			// Source found
			t1 = clutter_container_find_child_by_name(CLUTTER_CONTAINER(parent), tablename_source.c_str());
			t2 = clutter_container_find_child_by_name(CLUTTER_CONTAINER(parent), tablename_target.c_str());
		}

		if(t1 && t2)
		{
			line = clutter_container_find_child_by_name(CLUTTER_CONTAINER(parent), key.c_str());
			obj->redrawLine(line, t1, t2);
		}
	}
}
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

	GList *children = clutter_actor_get_children(stage);
	for(ClutterActor *i;children; children = children->next)
	{
		i = static_cast<ClutterActor*>(children->data);

		clutter_actor_hide(i);
	}

	for(unsigned int i=0; i<tablelist->size();i++)
	{
		drawtable(&tablelist->at(i));
	}

	table *tbl;
	// Draw FKs
	for(unsigned int i = 0; i < tablelist->size(); i++)
	{
		tbl = &tablelist->at(i);
		for(unsigned int j=0;j < tbl->constraintlist.size();j++)
		{
			constraint *constr = tbl->constraintlist[j];
			constraint_fk* fk = dynamic_cast< constraint_fk* >( constr );
			if(fk)
			{
				if(fk->target != NULL)
				{
					std::string item = tbl->schemaname + "." + tbl->tablename;
					table *t1 = static_cast<table*>(fk->target->tablepntr);
					constraintlist[constr->constraint_schema+"."+constr->constraint_name]=fk;
					std::string target = t1->schemaname + "." + t1->tablename;

					ClutterActor *ActorT1,*ActorT2;

					ActorT1 = clutter_container_find_child_by_name(CLUTTER_CONTAINER(stage), item.c_str());
					ActorT2 = clutter_container_find_child_by_name(CLUTTER_CONTAINER(stage), target.c_str());
					drawLine(constr->constraint_schema+"."+constr->constraint_name, ActorT1, ActorT2);

				}
			}
		}
	}

}

void dbaclutter::drawtable(table *tbl)
{

	std::string name = tbl->schemaname + "." + tbl->tablename;

	GList *children = clutter_actor_get_children(stage);
	for(ClutterActor *i;children; children = children->next)
	{
		i = static_cast<ClutterActor*>(children->data);
		std::string tmpname(clutter_actor_get_name(i));

		if(tmpname==name)
		{
			clutter_actor_show(i);
			return;
		}
	}
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

	clutter_actor_set_name(box, name.c_str());

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
	g_signal_connect (dragaction, "drag-motion", G_CALLBACK (on_actor_dragmotion), this);

	//m_drawgraph_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_drawgraph_button_clicked));

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


void dbaclutter::drawLine(std::string name, ClutterActor *actor1, ClutterActor *actor2)
{
	float x,y,len,angle;

	GList *children = clutter_actor_get_children(stage);
	for(ClutterActor *i;children; children = children->next)
	{
		i = static_cast<ClutterActor*>(children->data);
		std::string tmpname(clutter_actor_get_name(i));

		if(tmpname==name)
		{
			clutter_actor_show(i);
			return;
		}
	}

	ClutterActor *parent;
	parent = clutter_actor_get_parent(actor1);

	get_line_position(actor1, actor2, &x, &y, &len, &angle);

	ClutterActor *myline = clutter_actor_new ();
	ClutterColor myLineColor = { 255, 0, 0, 255 };
	clutter_actor_set_background_color(myline, &myLineColor);
	clutter_actor_add_child(parent, myline);
	clutter_actor_set_position(myline, x, y);
	clutter_actor_set_size(myline, len, 1);
	clutter_actor_set_rotation_angle (myline, CLUTTER_Z_AXIS, angle);

	clutter_actor_set_name(myline, name.c_str());
}

void dbaclutter::redrawLine(ClutterActor *line, ClutterActor *actor1, ClutterActor *actor2)
{
	float x,y,len,angle;

	get_line_position(actor1, actor2, &x, &y, &len, &angle);

	clutter_actor_set_position(line, x, y);
	clutter_actor_set_size(line, len, 1);
	clutter_actor_set_rotation_angle (line, CLUTTER_Z_AXIS, angle);
}

void dbaclutter::get_line_position(ClutterActor *actor1, ClutterActor *actor2, float *x, float *y, float *len, float *angle)
{
	float actor1x, actor1y, actor2x, actor2y;
	float actorRightX, actorRightY, actorLeftX, actorLeftY;
	float lenX, lenY, anglecorrection;
	bool p1;

	helper_clutter_actor_get_center_position(actor1, &actor1x, &actor1y);
	helper_clutter_actor_get_center_position(actor2, &actor2x, &actor2y);

	if(actor1x>=actor2x)
	{
		// Actor1 is right
		actorLeftX = actor2x;
		actorLeftY = actor2y;
		actorRightX = actor1x;
		actorRightY = actor1y;

		if(actor1y<actor2y)
		{
			// Actor1 top right
			p1=false;
			anglecorrection=90;
		}
		else
		{
			// Actor1 is bottom right
			p1=true;
			anglecorrection=180;
		}
	}
	else
	{
		// Actor1 is left
		actorLeftX = actor1x;
		actorLeftY = actor1y;
		actorRightX = actor2x;
		actorRightY = actor2y;

		if(actor1y<actor2y)
		{
			// Actor1 is top left
			p1=true;
			anglecorrection=0;
		}
		else
		{
			// Actor1 is bottom left
			p1=false;
			anglecorrection=-90;
		}
	}

	lenX = (actorRightX-actorLeftX);
	if(actorRightY<actorLeftY)
		lenY = actorLeftY - actorRightY;
	else
		lenY = actorRightY - actorLeftY;

	*x = actor1x;
	*y = actor1y;
	*len = sqrt((lenX*lenX) + (lenY*lenY));
	if(p1)
		*angle = (atan(lenY / lenX)/(2*M_PI)*360)+anglecorrection;
	else
		*angle = (atan(lenX / lenY)/(2*M_PI)*360)+anglecorrection;
}

void dbaclutter::helper_clutter_actor_get_center_position(ClutterActor *actor, float *x, float *y)
{
	float sizeX, sizeY;
	float posX, posY;

	clutter_actor_get_size(actor, &sizeX, &sizeY);
	clutter_actor_get_position(actor, &posX, &posY);

	*x = posX + (sizeX/2);
	*y = posY + (sizeY/2);
}
