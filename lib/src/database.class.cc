#include "database.h"
#include "database.class.h"
#include "table.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
#include <iostream>
#include "log.h"

database::database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	GError *error = NULL;
	GdaSqlParser *parser;
	dbg << "Connecting to Type:" << type
		<< " Host:" << host
		<< " User:" << user
		<< " DBname:" << dbname;

	std::string connstr, authstr;
	connstr = "DB_NAME="+dbname+";PORT="+std::to_string(port);
	authstr = "USERNAME="+user+";PASSWORD="+password;

	gda_init ();
	this->dbconn = gda_connection_open_from_string (type.c_str(), connstr.c_str(), authstr.c_str(),
	 	GDA_CONNECTION_OPTIONS_NONE,
		&error);

    if (!this->dbconn)
    {
        g_print ("Could not open connection to database: %s\n",
                 error && error->message ? error->message : "No detail");
        exit (1);
    }

	/* create an SQL parser */
	parser = gda_connection_create_parser (this->dbconn);
	if (!parser) /* @cnc does not provide its own parser => use default one */
		parser = gda_sql_parser_new ();
	/* attach the parser object to the connection */
	g_object_set_data_full (G_OBJECT (this->dbconn), "parser", parser, g_object_unref);

}

bool database::loadTables()
{
	GdaDataModel *data_model;
	GdaSqlParser *parser;
	GdaStatement *stmt;
	const char *sql = "SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN('pg_catalog','information_schema') AND table_type='BASE TABLE'";
	GError *error = NULL;

	parser = gda_connection_create_parser (this->dbconn);
	stmt = gda_sql_parser_parse_string (parser, sql, NULL, NULL);
	data_model = gda_connection_statement_execute_select (this->dbconn, stmt, NULL, &error);
	g_object_unref (stmt);
    if (!data_model)
        g_error ("Could not get tablelist: %s\n",
                 error && error->message ? error->message : "No detail");
	//gda_data_model_dump (data_model, stdout);

	gint row_id;
	const GValue *value;
	std::string schemaname, tablename;

	for(row_id=0;row_id < gda_data_model_get_n_rows(data_model); row_id++)
	{

		value = gda_data_model_get_value_at(data_model, 0, row_id, NULL);
		schemaname = gda_value_stringify(value);
		value = gda_data_model_get_value_at(data_model, 1, row_id, NULL);
		tablename = gda_value_stringify(value);
		this->tablelist.push_back(table(schemaname,tablename));
	}

	g_object_unref (data_model);

}

void database::loadColumns()
{
	GdaDataModel *data_model;
	GdaSqlParser *parser;
	GdaStatement *stmt;

	std::stringstream sql;
	sql << "SELECT column_name, data_type, character_maximum_length,";
	sql << "numeric_precision, numeric_scale,is_nullable, table_schema, table_name ";
	sql << " FROM information_schema.columns WHERE table_schema not in('pg_catalog','information_schema') ";
	sql << " AND EXISTS(SELECT 1 FROM information_schema.tables WHERE table_name=columns.table_name AND table_schema=columns.table_schema AND table_type='BASE TABLE') ";
	sql << " ORDER BY table_schema, table_name";

	GError *error = NULL;

	parser = gda_connection_create_parser (dbconn);
	stmt = gda_sql_parser_parse_string (parser, sql.str().c_str(), NULL, NULL);
	data_model = gda_connection_statement_execute_select (dbconn, stmt, NULL, &error);
	g_object_unref (stmt);
	if (!data_model)
		g_error ("Could not get tablelist: %s\n",
			error && error->message ? error->message : "No detail");

	gint row_id;
	const GValue *value;
	std::string column_name, datatype, character_maximum_length, numeric_precision;
	std::string numeric_scale, is_nullable, table_schema, table_name;
	std::string last_table_schema, last_table_name;
	table* tablepntr;
	last_table_schema = "-1";
	last_table_name = "-1";

	for(row_id=0;row_id < gda_data_model_get_n_rows(data_model); row_id++)
	{
		value = gda_data_model_get_value_at(data_model, 0, row_id, NULL);
		column_name = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 1, row_id, NULL);
		datatype = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 2, row_id, NULL);
		character_maximum_length = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 3, row_id, NULL);
		numeric_precision = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 4, row_id, NULL);
		numeric_scale = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 5, row_id, NULL);
		is_nullable = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 6, row_id, NULL);
		table_schema = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 7, row_id, NULL);
		table_name = gda_value_stringify(value);

		if(last_table_schema!=table_schema || last_table_name!=table_name)
		{
			tablepntr = this->getTable(table_schema, table_name);
			last_table_schema = table_schema;
			last_table_name = table_name;
		}

		if(tablepntr)
		{
			tablepntr->columnlist.push_back(column(column_name, datatype, character_maximum_length, numeric_precision, numeric_scale, is_nullable));
		}

	}

	g_object_unref (data_model);
}

void database::loadConstraints()
{
	GdaDataModel *data_model;
	GdaSqlParser *parser;
	GdaStatement *stmt;
	std::stringstream sql;

	sql << "SELECT table_constraints.constraint_type, table_constraints.constraint_schema, table_constraints.constraint_name, constraint_column_usage.table_schema, ";
	sql << " constraint_column_usage.table_name, constraint_column_usage.column_name, ";
	sql << " ccu.table_schema as fk_table_schema, ccu.table_name as fk_table_name, ccu.column_name as fk_column_name ";
	sql << "FROM information_schema.table_constraints ";
	sql << "JOIN information_schema.constraint_column_usage USING(constraint_name, constraint_schema) ";
	sql << "LEFT JOIN information_schema.referential_constraints USING(constraint_name, constraint_schema) ";
	sql << "LEFT JOIN information_schema.constraint_column_usage ccu on(ccu.constraint_name=unique_constraint_name AND ccu.constraint_schema=unique_constraint_schema) ";
	sql << "ORDER BY fk_table_schema, fk_table_name, table_schema, table_name";

	GError *error = NULL;

	parser = gda_connection_create_parser (dbconn);
	stmt = gda_sql_parser_parse_string (parser, sql.str().c_str(), NULL, NULL);
	data_model = gda_connection_statement_execute_select (dbconn, stmt, NULL, &error);
	g_object_unref (stmt);
	if (!data_model)
		g_error ("Could not get Constraints: %s\n",
			error && error->message ? error->message : "No detail");
	//gda_data_model_dump (data_model, stdout);

	gint row_id;
	const GValue *value;
	std::string constraint_type, constraint_schema, constraint_name, table_schema;
	std::string table_name, column_name, fk_table_schema, fk_table_name, fk_column_name;
	std::string last_fk_schema="-1", last_fk_name="-1";
	std::string last_uk_schema="-1", last_uk_name="-1";
	constraint_fk *fk;
	constraint_uk *uk;
	table* tablepntr;

	for(row_id=0;row_id < gda_data_model_get_n_rows(data_model); row_id++)
	{
		value = gda_data_model_get_value_at(data_model, 0, row_id, NULL);
		constraint_type = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 1, row_id, NULL);
		constraint_schema = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 2, row_id, NULL);
		constraint_name = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 3, row_id, NULL);
		table_schema = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 4, row_id, NULL);
		table_name = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 5, row_id, NULL);
		column_name = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 6, row_id, NULL);
		fk_table_schema = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 7, row_id, NULL);
		fk_table_name = gda_value_stringify(value);

		value = gda_data_model_get_value_at(data_model, 8, row_id, NULL);
		fk_column_name = gda_value_stringify(value);

		tablepntr = this->getTable(table_schema, table_name);
		if(!tablepntr)
			continue;

		if(constraint_type=="PRIMARY KEY")
		{
			tablepntr->pk.push_back(column_name);
		}
		else if(constraint_type=="FOREIGN KEY")
		{
			if(last_fk_schema!=constraint_schema || last_fk_name!=constraint_name)
			{
				if(last_fk_schema!="-1")
					tablepntr->constraintlist.push_back(fk);
				fk = new constraint_fk(constraint_schema, constraint_name);
				last_fk_schema = constraint_schema;
				last_fk_name = constraint_name;
			}

			fk->addSource(table_schema, table_name, column_name);
			fk->addTarget(fk_table_schema, fk_table_name, fk_column_name);
		}
		else if(constraint_type=="UNIQUE")
		{
			if(last_uk_schema!=constraint_schema || last_uk_name!=constraint_name)
			{
				if(last_uk_schema!="-1")
					tablepntr->constraintlist.push_back(uk);
				uk = new constraint_uk(constraint_schema, constraint_name);
				last_uk_schema = constraint_schema;
				last_uk_name = constraint_name;
			}
			uk->addSource(table_schema, table_name, column_name);
		}

	}
	if(last_fk_schema!="-1")
		tablepntr->constraintlist.push_back(fk);
	if(last_uk_schema!="-1")
		tablepntr->constraintlist.push_back(uk);
	g_object_unref (data_model);
}

table* database::getTable(std::string schemaname, std::string tablename)
{
	for(unsigned int i = 0; i < this->tablelist.size(); i++)
	{
		if(this->tablelist[i].schemaname == schemaname && this->tablelist[i].tablename==tablename)
		{
			return &this->tablelist[i];
		}
	}
	return NULL;
}


void database::analyze()
{
	/*
	Number of Tables
	Number of Tables Without PK
	AVG Number of Columns per Table
	Number of FK
	Size of Tables
	*/
	this->stat.num_tables = this->tablelist.size();
	this->stat.num_columns = 0;
	this->stat.num_fk = 0;
	this->stat.num_uk = 0;

	for(unsigned int i = 0; i < this->tablelist.size(); i++)
	{
		if(tablelist[i].pk.size()==0)
			this->stat.tables_without_pk.push_back(&tablelist[i]);

		this->stat.num_columns += this->tablelist[i].columnlist.size();

		for(unsigned int j = 0; j < this->tablelist[i].constraintlist.size(); j++)
		{
			constraint_fk* is_a_fk = dynamic_cast< constraint_fk* >( this->tablelist[i].constraintlist[j] );
			constraint_uk* is_a_uk = dynamic_cast< constraint_uk* >( this->tablelist[i].constraintlist[j] );
			if(is_a_fk)
			{
				this->stat.num_fk++;
			}
			else if(is_a_uk)
			{
				this->stat.num_uk++;
			}
		}
	}

	green << "Number of Tables:" << this->stat.num_tables;
	green << "Number of Tables without PK:" << this->stat.tables_without_pk.size();
	for(unsigned int j = 0; j < this->stat.tables_without_pk.size(); j++)
	{
		red << "\t" << this->stat.tables_without_pk[j]->schemaname << "." << this->stat.tables_without_pk[j]->tablename;
	}
	green << "Number of Columns: " << this->stat.num_columns;
	green << "Number of FK: " << this->stat.num_fk;
	green << "Number of UK: " << this->stat.num_uk;
}

void database::output()
{
	unsigned int i;
	std::cout << std::endl << "== Tables ==" << std::endl;
	for(i = 0; i < this->tablelist.size(); i++)
	{
		tablelist[i].output();
	}
	std::cout << "(" << i << " rows)" << std::endl;
}

database::~database()
{
	gda_connection_close (this->dbconn);
}