#include "table.class.h"
#include "log.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
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

	std::cout << " ( PK:";
	if(this->pk.size()==0)
	{
		red << " missing";
	}
	else
	{
		for(unsigned int i=0;i < this->pk.size();i++)
		{
			std::cout << " " << this->pk[i];
		}
	}
	std::cout << " ) ";

	for(unsigned int i=0;i < this->constraintlist.size();i++)
	{
		this->constraintlist[i].output();
		std::cout << " ";
	}
	std::cout << std::endl;

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

bool table::loadConstraints(GdaConnection *dbconn)
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
	sql << "LEFT JOIN information_schema.constraint_column_usage ccu on(ccu.constraint_name=unique_constraint_name AND ccu.constraint_schema=unique_constraint_schema)";
	sql << "WHERE table_constraints.table_schema='" << this->schemaname << "' AND table_constraints.table_name='" << this->tablename << "'";

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
	constraint_fk fk;
	constraint_uk uk;

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

		if(constraint_type=="PRIMARY KEY")
		{
			this->pk.push_back(column_name);
		}
		else if(constraint_type=="FOREIGN KEY")
		{
			if(last_fk_schema!=constraint_schema || last_fk_name!=constraint_name)
			{
				if(last_fk_schema!="-1")
					this->constraintlist.push_back(fk);
				fk = constraint_fk(constraint_schema, constraint_name);
				last_fk_schema = constraint_schema;
				last_fk_name = constraint_name;
			}

			fk.addSource(this->schemaname, this->tablename, column_name);
			fk.addTarget(fk_table_schema, fk_table_name, fk_column_name);
		}
		else if(constraint_type=="UNIQUE")
		{
			if(last_uk_schema!=constraint_schema || last_uk_name!=constraint_name)
			{
				if(last_uk_schema!="-1")
					this->constraintlist.push_back(uk);
				uk = constraint_uk(constraint_schema, constraint_name);
				last_uk_schema = constraint_schema;
				last_uk_name = constraint_name;
			}
			uk.addSource(this->schemaname, this->tablename, column_name);
		}

	}
	if(last_fk_schema!="-1")
		this->constraintlist.push_back(fk);
	if(last_uk_schema!="-1")
		this->constraintlist.push_back(uk);
	g_object_unref (data_model);
}