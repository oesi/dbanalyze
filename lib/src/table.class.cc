#include "table.class.h"
#include <iostream>
#include <sstream>

table::table(std::string schemaname, std::string tablename)
{
	this->schemaname = schemaname;
	this->tablename = tablename;
}

void table::output()
{
	std::cout << this->schemaname << "." << this->tablename << std::endl;

	for(unsigned int i=0;i < this->columnlist.size();i++)
	{
		this->columnlist[i].output();
	}
}

bool table::loadColumns(GdaConnection *dbconn)
{
	GdaDataModel *data_model;
	GdaSqlParser *parser;
	GdaStatement *stmt;

	std::stringstream sql;
	sql << "SELECT column_name, data_type, character_maximum_length,numeric_precision, numeric_scale,is_nullable ";
	sql << " FROM information_schema.columns ";
	sql << " WHERE table_schema='" << this->schemaname << "' AND table_name='" << this->tablename << "'";

	GError *error = NULL;

	parser = gda_connection_create_parser (dbconn);
	stmt = gda_sql_parser_parse_string (parser, sql.str().c_str(), NULL, NULL);
	data_model = gda_connection_statement_execute_select (dbconn, stmt, NULL, &error);
	g_object_unref (stmt);
    if (!data_model)
        g_error ("Could not get tablelist: %s\n",
                 error && error->message ? error->message : "No detail");
	//gda_data_model_dump (data_model, stdout);

	gint row_id;
	const GValue *value;
	std::string column_name, datatype, character_maximum_length, numeric_precision, numeric_scale,is_nullable;

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

		this->columnlist.push_back(column(column_name,datatype,character_maximum_length, numeric_precision, numeric_scale, is_nullable));

	}

	g_object_unref (data_model);
}