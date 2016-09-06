#include "database.class.h"
#include <iostream>
database::database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	GError *error = NULL;

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