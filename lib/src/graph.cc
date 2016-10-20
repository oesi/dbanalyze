#include "table.class.h"
#include "constraint_fk.class.h"
#include "utils.h"
#include <map>
#include <algorithm>
#include <graphviz/gvc.h>

void printGraph(std::vector<table> *tablelist, std::string name)
{
	std::map<std::string,std::vector<table*>> list;
	unsigned int i;
	table *tbl;
	std::map<std::string, Agnode_t*> nodelist;

	Agraph_t *g;

	/* set up a graphviz context */
	GVC_t *gvc = gvContext();
	std::string filename = "-odata/"+name+".png";

	// set outputfile, filetype, and graphtype
	char* args[] = {(char*)"dot",(char*)"-Tpng",(char*)filename.c_str()};

	/* parse command line args - minimally argv[0] sets layout engine */
	gvParseArgs(gvc, sizeof(args)/sizeof(char*), args);

	/* Create a simple digraph */
	g = agopen((char*)"g", Agdirected, 0);

	// cycle throu tables
	for(i = 0; i < tablelist->size(); i++)
	{
		tbl = &tablelist->at(i);
		std::string tname = tbl->schemaname + "_" + tbl->tablename;

		// Create Graph Table Node
		Agnode_t *n = agnode(g, (char*)tname.c_str(), 1);

		// remove shape (border) of node
		agsafeset(n, (char*)"shape", (char*)"none", (char*)"");

		// set html table as node label

		std::string html = "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\
		<TR>\
		<TD COLSPAN=\"3\" BGCOLOR=\"#3893FE\"><B>"+tbl->schemaname + "." + tbl->tablename+"</B></TD>\
		</TR>";

		// print the columns
		for(unsigned int col=0; col < tbl->columnlist.size(); col++)
		{
			std::string datatype = tbl->columnlist[col].getColumnType();
			std::string key = "";
			// Primary Key
			if(std::find(tbl->pk.begin(), tbl->pk.end(), tbl->columnlist[col].columnname)!=tbl->pk.end())
				key += "<FONT COLOR=\"#FF0000\">PK</FONT> ";

			std::vector<constraint*> colconstr = tbl->getConstraints(tbl->columnlist[col].columnname);
			for(unsigned int j=0;j < colconstr.size();j++)
			{
				constraint_fk* fk = dynamic_cast< constraint_fk* >( colconstr[j] );
				if(fk)
				{
					key += "<FONT COLOR=\"#0000FF\">FK</FONT> ";
					break;
				}
			}
			if(tbl->columnlist[col].is_nullable!="TRUE")
			{
				key += "NN ";
			}

			html +=	"<TR>\
				<TD PORT=\""+tbl->columnlist[col].columnname+"\"><FONT POINT-SIZE=\"10\"> "+key+"</FONT></TD>\
				<TD ALIGN=\"LEFT\"><FONT POINT-SIZE=\"10\">"+tbl->columnlist[col].columnname+"</FONT></TD>\
				<TD ALIGN=\"LEFT\"><FONT POINT-SIZE=\"10\">"+datatype+"</FONT></TD>\
				</TR>";
		}

		html += " </TABLE>";
		char* l = agstrdup_html(agroot(n), (char*)html.c_str());
		agset (n,(char*)"label",l);

		agstrfree (g,l);

		nodelist[tname]=n;
	}

	// Draw Edges (Foreign Keys) between Tables
	std::vector<Agedge_t*> edgelist;

	for(i = 0; i < tablelist->size(); i++)
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
					std::string item = tbl->schemaname + "_" + tbl->tablename;
					table *t1 = static_cast<table*>(fk->target->tablepntr);

					std::string target = t1->schemaname + "_" + t1->tablename;
					if(nodelist[target] && nodelist[item])
					{
						Agedge_t *e;
						e = agedge(g, nodelist[item], nodelist[target], 0, 1);

						// set begin and end of edge to the column position (port)
						column *source = static_cast<column*>(tbl->constraintlist[j]->source);
						agsafeset(e,(char*)"headport",(char*)fk->target->columnname.c_str(),(char*)"");
						agsafeset(e,(char*)"tailport",(char*)source->columnname.c_str(),(char*)"");

						// set color of edge
						std::string color;
						color = StringToColor(tbl->constraintlist[j]->constraint_name);
						agsafeset(e,(char*)"color",(char*)color.c_str(),(char*)"");

						edgelist.push_back(e);
					}
					else
					{
						// if fk goes outside the schema create a Node with label and link to it
						std::string target = t1->schemaname + "." + t1->tablename+"."+fk->target->columnname.c_str();
						if(tbl->constraintlist[j]->source)
						{
							column *source = static_cast<column*>(tbl->constraintlist[j]->source);

							Agnode_t *n = agnode(g, (char*)target.c_str(), 1);
							agsafeset(n, (char*)"shape", (char*)"none", (char*)"");
							agsafeset(n, (char*)"fontsize", (char*)"8", (char*)"");

							Agedge_t *e;
							e = agedge(g, nodelist[item], n, 0, 1);

							// set the beginn of the edge to the column position
							agsafeset(e,(char*)"tailport",(char*)source->columnname.c_str(),(char*)"");

							edgelist.push_back(e);
						}
					}
				}
			}
		}
	}
	/* Compute a layout using layout engine from command line args */
	gvLayoutJobs(gvc, g);

	/* Write the graph according to -T and -o options */
	gvRenderJobs(gvc, g);

	/* Free layout data */
	gvFreeLayout(gvc, g);

	/* Free graph structures */
	agclose(g);

	/* close output file, free context */
	gvFreeContext(gvc);
}
