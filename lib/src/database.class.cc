#include "database.class.h"
#include <iostream>
database::database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	GError *error = NULL;
	std::string connstr, authstr;

	this->dbname = dbname;
	this->type = type;

	if(type=="sqlite")
	{
		connstr = "DB_DIR=.;DB_NAME="+dbname;
	}
	else
	{
		connstr = "DB_NAME="+dbname+";PORT="+std::to_string(port);
		authstr = "USERNAME="+user+";PASSWORD="+password;
	}

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
}

void database::loadTables()
{
	GError *error = NULL;
	this->row_id=-1;
	std::string query;

	gda_connection_update_meta_store(this->dbconn, NULL, &error);
	//this->data_model = gda_connection_get_meta_store_data (this->dbconn, GDA_CONNECTION_META_TABLES, &error, 1);

	if(this->type=="PostgreSQL")
		query = "SELECT * FROM _tables WHERE table_type='BASE TABLE' AND table_schema not in('information_schema','pg_catalog')";
	else if(this->type=="MySQL")
		query = "SELECT * FROM _tables WHERE table_type='BASE TABLE' AND table_schema ='" + this->dbname + "'";
	else
		query = "SELECT * FROM _tables WHERE table_type='BASE TABLE'";

	this->queryMeta(query);

}

void database::loadColumns()
{
	this->row_id=-1;
	std::string query;

	if(this->type=="PostgreSQL")
		query = "SELECT * FROM _columns WHERE table_schema NOT IN('pg_catalog','information_schema')";
	else if(this->type=="MySQL")
		query = "SELECT * FROM _columns WHERE table_schema ='"+this->dbname+"'";
	else
		query = "SELECT * FROM _columns";

	this->queryMeta(query);
}

database::~database()
{
	g_object_unref (this->data_model);
	gda_connection_close (this->dbconn);
}

bool database::query(std::string sql)
{
	GdaSqlParser *parser;
	GdaStatement *stmt;
	GError *error = NULL;
	this->row_id=-1;

	parser = gda_connection_create_parser (this->dbconn);
	stmt = gda_sql_parser_parse_string (parser, sql.c_str(), NULL, NULL);
	this->data_model = gda_connection_statement_execute_select (this->dbconn, stmt, NULL, &error);
	g_object_unref (stmt);

	if (!this->data_model)
	{
		g_error ("Could not execute query: %s\n",
			error && error->message ? error->message : "No detail");
		return false;
	}

	return true;
}

bool database::queryMeta(std::string sql)
{
	GdaSqlParser *parser;
	GdaStatement *stmt;
	GError *error = NULL;
	this->row_id=-1;

	GdaMetaStore* store;

	store = gda_connection_get_meta_store(this->dbconn);

	parser = gda_connection_create_parser (this->dbconn);
	stmt = gda_sql_parser_parse_string (parser, sql.c_str(), NULL, NULL);
	this->data_model = gda_meta_store_extract (store, sql.c_str(), NULL, &error);
	g_object_unref (stmt);

	if (!this->data_model)
	{
		g_error ("Could not execute query: %s\n",
			error && error->message ? error->message : "No detail");
		return false;
	}

	return true;
}

bool database::nextRow()
{
	if(this->row_id < gda_data_model_get_n_rows(this->data_model)-1)
	{
		this->row_id++;
		return true;
	}
	else
	{
		return false;
	}
}

std::string database::get(std::string columnname)
{
	return gda_value_stringify(this->getRecord(columnname));
}

double database::getNumber(std::string columnname)
{
	const GdaNumeric* data = gda_value_get_numeric(this->getRecord(columnname));
	return gda_numeric_get_double(data);
}

const GValue *database::getRecord(std::string columnname)
{
	return gda_data_model_get_value_at(this->data_model,
		gda_data_model_get_column_index (this->data_model,columnname.c_str()),
		this->row_id, NULL);
}