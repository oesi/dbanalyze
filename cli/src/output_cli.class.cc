#include "output_cli.class.h"
#include "formatter.h"
#include <map>
#include <fstream>
#include <algorithm>
#include <graphviz/gvc.h>

#define KEYSYMBOL "⚷"

void output_cli::printConstraint(constraint* constr)
{
	std::cout << KEYSYMBOL;
	std::cout << constr->constraint_name << "(";
	constraint_fk* fk = dynamic_cast< constraint_fk* >( constr );
	if(fk)
	{
		if(fk->target != NULL)
		{
			table *t1 = static_cast<table*>(fk->target->tablepntr);
			std::cout << t1->schemaname << "." << t1->tablename << ".";
			std::cout << fk->target->columnname << " ";

		}
		else
		{
			std::cout << "missing??";
		}
		std::cout << ")";
	}
}

void output_cli::printColumn(column* column)
{
	std::cout << "\t" << column->columnname << " " << column->datatype;
}

void output_cli::printTable(table* table)
{
	std::cout << table->schemaname << "." << table->tablename;

	std::cout << " ( PK:";
	if(table->pk.size()==0)
	{
		std::cout << " missing";
	}
	else
	{
		for(unsigned int i=0;i < table->pk.size();i++)
		{
			std::cout << " " << table->pk[i];
		}
	}
	std::cout << " ) ";
	std::cout << table->row_count << " Entries" << std::endl;

	for(unsigned int i=0;i < table->columnlist.size();i++)
	{
		printColumn(&table->columnlist[i]);
		std::vector<constraint*> colconstr = table->getConstraints(table->columnlist[i].columnname);
		for(unsigned int j=0;j < colconstr.size();j++)
		{
			printConstraint(colconstr[j]);
		}
		std::cout << std::endl;
	}
}

void output_cli::printData(std::vector<table> *tablelist)
{
	unsigned int i;
	std::cout << std::endl << "== Tables ==" << std::endl;
	for(i = 0; i < tablelist->size(); i++)
	{
		printTable(&tablelist->at(i));
	}
	std::cout << "(" << i << " rows)" << std::endl;
}

void output_cli::printStatistic(statistic* stat)
{
	green << "Tables without PK:" << stat->tables_without_pk.size();
	for(unsigned int j = 0; j < stat->tables_without_pk.size(); j++)
	{
		red << "\t" << stat->tables_without_pk[j]->schemaname + "." + stat->tables_without_pk[j]->tablename;
	}
	green << "Empty Tables:" << stat->empty_tables.size();
	for(unsigned int j = 0; j < stat->empty_tables.size(); j++)
	{
		red << "\t" << stat->empty_tables[j]->schemaname + "." + stat->empty_tables[j]->tablename;
	}

	green << "FK datatype missmatch:" << stat->fk_datatype_missmatch.size();
	for(unsigned int j = 0; j < stat->fk_datatype_missmatch.size(); j++)
	{
		constraint_fk* fk = dynamic_cast< constraint_fk* >( stat->fk_datatype_missmatch[j] );
		column *sourcecol = static_cast<column*>(fk->source);

		red << "\t" << KEYSYMBOL + stat->fk_datatype_missmatch[j]->constraint_schema + "." + stat->fk_datatype_missmatch[j]->constraint_name + "." + sourcecol->columnname;
	}

	green << "Number of Tables:" << stat->num_tables;
 	green << "Number of Columns: " << stat->num_columns;
	green << "Number of FK: " << stat->num_fk;
	green << "Number of UK: " << stat->num_uk;

	std::cout << std::endl;

	th << "Score" << "" << std::endl;
	tf << "" << "" << std::endl;
	tr << "Maximum Score" << stat->maximum_score << std::endl;
	tr << "Score" << stat->score << std::endl;
	tr << "Percent" << 100/stat->maximum_score*stat->score << std::endl;

}


void output_cli::printGraph(std::vector<table> *tablelist)
{
	std::map<std::string,std::vector<table*>> list;
	unsigned int i;
	table *tbl;
	std::map<std::string, Agnode_t*> nodelist;

	// get all schema
	for(i = 0; i < tablelist->size(); i++)
	{
		tbl = &tablelist->at(i);
		list[tbl->schemaname].push_back(tbl);
	}

	// create a file per schema
	for(auto &k : list)
	{
		Agraph_t *g;

		/* set up a graphviz context */
		GVC_t *gvc = gvContext();
		std::string filename = "-odata/"+k.first+".png";

		// set outputfile, filetype, and graphtype
		char* args[] = {(char*)"dot",(char*)"-Tpng",(char*)filename.c_str()};

		/* parse command line args - minimally argv[0] sets layout engine */
		gvParseArgs(gvc, sizeof(args)/sizeof(char*), args);

		/* Create a simple digraph */
		g = agopen((char*)"g", Agdirected, 0);

		// cycle throu tables of schema
		for(i = 0; i < k.second.size(); i++)
		{
			tbl = k.second.at(i);
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
			for(unsigned int col=0; col<tbl->columnlist.size(); col++)
			{

				std::string datatype=tbl->columnlist[col].getColumnType();
				std::string key="";
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
					<TD><FONT POINT-SIZE=\"10\"> "+key+"</FONT></TD>\
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
			if(k.first==tbl->schemaname)
			{
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
							// only create FK if it is in the same schema
							if(k.first==t1->schemaname)
							{
								std::string target = t1->schemaname + "_" + t1->tablename;
								Agedge_t *e;
								e = agedge(g, nodelist[item], nodelist[target], 0, 1);
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
}
