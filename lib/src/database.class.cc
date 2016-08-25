#include "database.h"
#include "database.class.h"
#include "table.class.h"
#include <iostream>
#include "log.h"

database::database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	GError *error = NULL;
	GdaSqlParser *parser;
	dbg << "Connecting to Type:" << type
		<< " Host:" << host
		<< " User:" << user
		<< " Password:" << password
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
	const char *sql = "SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN('pg_catalog','information_schema')";
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

bool database::loadColumns()
{
	for(unsigned int i = 0; i < this->tablelist.size(); i++)
	{
		tablelist[i].loadColumns(this->dbconn);
	}
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