#include "database.class.h"
#include <iostream>
database::database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	GError *error = NULL;
	std::string connstr, authstr;

	this->dbname = dbname;
	this->type = type;

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

void database::loadTables()
{
	GError *error = NULL;
	this->row_id=-1;
	std::string query;

	gda_connection_update_meta_store(this->dbconn, NULL, &error);

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
		query = "SELECT REPLACE(data_type, 'pg_catalog.','') as data_type, _columns.* FROM _columns JOIN _tables USING(table_catalog, table_schema, table_name) WHERE table_type='BASE TABLE' AND table_schema NOT IN('pg_catalog','information_schema')";
	else if(this->type=="MySQL")
		query = "SELECT _columns.* FROM _columns JOIN _tables USING(table_catalog, table_schema, table_name) WHERE table_type='BASE TABLE' AND table_schema ='"+this->dbname+"'";
	else
		query = "SELECT _columns.* FROM _columns JOIN _tables USING(table_catalog, table_schema, table_name) WHERE table_type='BASE TABLE'";

	this->queryMeta(query);
}

void database::loadConstraints()
{
	std::string query;
	this->row_id=-1;

	if(this->type=="PostgreSQL")
	{
		query = "SELECT \
					_table_constraints.*, rc.*, kc2.column_name as ref_column_name, kc1.column_name as column_name \
				FROM \
					_table_constraints \
					LEFT JOIN _key_column_usage kc1 USING(table_catalog, table_schema, table_name, constraint_name) \
					LEFT JOIN _referential_constraints rc USING(table_catalog, table_schema, table_name, constraint_name) \
					LEFT JOIN _key_column_usage kc2 ON (rc.ref_table_catalog=kc2.table_catalog AND rc.ref_table_schema=kc2.table_schema AND rc.ref_table_name=kc2.table_name AND rc.ref_constraint_name=kc2.constraint_name) \
					WHERE _table_constraints.table_schema NOT IN('pg_catalog','information_schema') AND (kc1.ordinal_position is null OR kc2.ordinal_position is null OR kc1.ordinal_position = kc2.ordinal_position)";
	}
	else if(this->type=="MySQL")
	{
		query = "SELECT \
					_table_constraints.*, rc.*, kc2.column_name as ref_column_name, kc1.column_name as column_name \
				FROM \
					_table_constraints \
					LEFT JOIN _key_column_usage kc1 USING(table_catalog, table_schema, table_name, constraint_name) \
					LEFT JOIN _referential_constraints rc USING(table_catalog, table_schema, table_name, constraint_name) \
					LEFT JOIN _key_column_usage kc2 ON (rc.ref_table_catalog=kc2.table_catalog AND rc.ref_table_schema=kc2.table_schema AND rc.ref_table_name=kc2.table_name AND rc.ref_constraint_name=kc2.constraint_name) \
					WHERE (kc1.ordinal_position is null OR kc2.ordinal_position is null OR kc1.ordinal_position = kc2.ordinal_position) \
					AND _table_constraints.table_schema ='"+this->dbname+"'";
	}
	else
	{
		query = "SELECT \
					_table_constraints.*, rc.*, kc2.column_name as ref_column_name, kc1.column_name as column_name \
				FROM \
					_table_constraints \
					LEFT JOIN _key_column_usage kc1 USING(table_catalog, table_schema, table_name, constraint_name) \
					LEFT JOIN _referential_constraints rc USING(table_catalog, table_schema, table_name, constraint_name) \
					LEFT JOIN _key_column_usage kc2 ON (rc.ref_table_catalog=kc2.table_catalog AND rc.ref_table_schema=kc2.table_schema AND rc.ref_table_name=kc2.table_name AND rc.ref_constraint_name=kc2.constraint_name) \
					WHERE (kc1.ordinal_position is null OR kc2.ordinal_position is null OR kc1.ordinal_position = kc2.ordinal_position)";

	}

	this->queryMeta(query);
}

void database::getTableSize(std::string schemaname, std::string tablename)
{
	std::string query;
	this->row_id=-1;

	if(this->type=="PostgreSQL")
	{
		query = "SELECT count(*)::numeric as row_count FROM "+schemaname+"."+tablename;
	}
	else if(this->type=="MySQL")
	{
			query = "SELECT count(*) as row_count FROM "+this->dbname+"."+tablename;
	}
	else
	{
		query = "SELECT CAST(count(*) as double) as row_count FROM "+schemaname+"."+tablename;
	}
	this->query(query);
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
	const GValue* value;
	value = this->getRecord(columnname);

	if(!gda_value_isa(value, GDA_TYPE_NUMERIC))
	{
		return std::stod(gda_value_stringify(value));
	}
	else
	{
		const GdaNumeric* data = gda_value_get_numeric(value);
		return gda_numeric_get_double(data);
	}
}

const GValue *database::getRecord(std::string columnname)
{
	return gda_data_model_get_value_at(this->data_model,
		gda_data_model_get_column_index (this->data_model,columnname.c_str()),
		this->row_id, NULL);
}